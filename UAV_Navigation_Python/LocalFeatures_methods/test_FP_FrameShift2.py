#!/usr/bin/python
__author__ = 'ar'

import os
import sys
import numpy as np
import numpy.matlib
import cv2
import libphcorr as ph
import matplotlib.pyplot as plt
import shutil

import libfp as fp

# fimg1='/home/ar/video/data_test_frames_2/frame_00008.png'
# fimg2='/home/ar/video/data_test_frames_2/frame_00009.png'

fimg1='/home/ar/video/data_test_frames_ShiftX_0/frame_00008.png'
fimg2='/home/ar/video/data_test_frames_ShiftX_0/frame_00009.png'

# fvideo='/home/ar/video/data_test_frames_rot_0/idx.csv'
# fvideo='/home/ar/video/data_test_frames_scl_0/idx.csv'
# fvideo='/home/ar/video/data_test_frames_rot_scl_0/idx.csv'

# fvideo='/home/ar/video/data_test_frames_ShiftX_0/idx.csv'
# fvideo='/home/ar/video/UAV_PHONE/part_2/test_vid0_from_1to2.mp4'
# fvideo='/home/ar/video/data_AlexKravchonok/video.avi'
fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'

def nrmMat(mat):
    return cv2.normalize(mat, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)

def plotFPoints(img,fps, color=(0,255,0), rad=-1):
    for pp in fps:
        trad=int(pp.size)
        if rad>0:
            trad=rad
        cv2.circle(img, (int(pp.pt[0]), int(pp.pt[1])), trad, color)

def plotLines(img, lp1, lp2, color=(255,255,0)):
    for p1,p2 in zip(lp1[:,0:2], lp2[:,0:2]):
        tp1=(int(p1[0]), int(p1[1]))
        tp2=(int(p2[0]), int(p2[1]))
        cv2.line(img, tp1,tp2, color)

def imregFeaturePoins(img1, img2, kpdata,detector=cv2.SURF(), matcher=cv2.BFMatcher(cv2.NORM_L2), isDebug=True, minNumGoodPairs=10, isUseCorrQuality=True):
    # kk1,dd1=detector.detectAndCompute(img1, None)
    # kk2,dd2=detector.detectAndCompute(img2, None)
    kk1,dd1=kpdata[0]
    kk2,dd2=kpdata[1]
    matches0=matcher.match(dd1, dd2)
    matches1=matcher.match(dd2, dd1)
    listGoodMatches=[]
    listP1=[]
    listP2=[]
    listDistances=[]
    for mm0 in matches0:
        if mm0.queryIdx==matches1[mm0.trainIdx].trainIdx:
            listGoodMatches.append(mm0)
            listDistances.append(mm0.distance)
            listP1.append(kk1[mm0.queryIdx].pt)
            listP2.append(kk2[mm0.trainIdx].pt)
    if isDebug:
        # print "good/total = %d/%d, dist min/max=(%0.2f, %0.2f)" % (len(listGoodMatches), len(matches0), min(listDistances), max(listDistances))
        pass
    if len(listGoodMatches)<minNumGoodPairs:
        quality=0.0
        return (None, quality)
    listP1=np.array(listP1, dtype=np.float32)
    listP2=np.array(listP2, dtype=np.float32)
    H,status = cv2.findHomography(listP2,listP1,cv2.RANSAC, 5.0)
    dxyas=calcShiftAngleScale2(H,np.array(img1.shape[:2][::-1])/2.0)
    print dxyas
    pano=None
    quality=float(np.sum(status))/len(status)
    if isUseCorrQuality:
        img2w=cv2.warpPerspective(img2.copy(), H, img1.shape[:2][::-1])
        pcnt=(img1.shape[0]/2, img1.shape[1]/2)
        img1p=img1 [pcnt[0]-pcnt[0]/2:pcnt[0]+pcnt[0]/2, pcnt[1]-pcnt[1]/2:pcnt[1]+pcnt[1]/2].reshape(-1)
        img2p=img2w[pcnt[0]-pcnt[0]/2:pcnt[0]+pcnt[0]/2, pcnt[1]-pcnt[1]/2:pcnt[1]+pcnt[1]/2].reshape(-1)
        img1_std=np.std(img1p)
        img2_std=np.std(img2p)
        if img1_std<0.01:
            img1_std=1.0
        if img2_std<0.01:
            img2_std=1.0
        img1p=(img1p-np.mean(img1p))/img1_std
        img2p=(img2p-np.mean(img2p))/img2_std
        quality=np.sum(np.dot(img1p, img2p))/len(img1p)
    if isDebug:
        # print '%d / %d  inliers/matched' % (np.sum(status), len(status))
        img1c=None
        img2c=None
        if len(img1.shape)<3:
            img1c=cv2.cvtColor(img1, cv2.COLOR_GRAY2BGR)
            img2c=cv2.cvtColor(img2, cv2.COLOR_GRAY2BGR)
        else:
            img1c=img1.copy()
            img2c=img2.copy()
        img1v=img1c.copy()
        img2v=img2c.copy()
        plotFPoints(img1v, kk1, rad=2)
        plotFPoints(img2v, kk2, rad=2)
        listP1Hom=[[ii[0], ii[1], 1.0] for ii in listP1]
        listP1Prj=np.transpose(np.dot(H,np.transpose(listP1Hom)))
        listP1Prj=listP1Prj/np.matlib.repmat(listP1Prj[:,2],3,1).transpose()
        # cv2.imshow("win-points-detected", np.concatenate((img1v,img2v), axis=1))
        img1t=img1c.copy()
        plotFPoints(img1t, kk1, rad=2, color=(255,255,0))
        plotLines(img1t, listP1, listP1Prj)
        # cv2.imshow("win-pts-shifts", img1t)
        img2w=cv2.warpPerspective(img2.copy(), H, img1.shape[:2][::-1])
        imgDIFF=np.dstack((img1,img2w,img1)).copy()
        txt="Q=%0.2f, %0.1f,%0.1f, %0.1f, %0.2f" % (quality, dxyas[0], dxyas[1], dxyas[2], dxyas[3])
        cv2.putText(imgDIFF, txt, (5,20), cv2.FONT_HERSHEY_PLAIN, 1.0, (0,0,0), 3)
        cv2.putText(imgDIFF, txt, (5,20), cv2.FONT_HERSHEY_PLAIN, 1.0, (255,255,255), 1)
        pano=np.concatenate( (np.concatenate((img1v,img2v), axis=1), np.concatenate( (img1t, imgDIFF), axis=1) ))
        cv2.imshow("win-points-detected-shifts-warps", pano)
        if (quality<0.3) or (dxyas[3]<0.8) or (dxyas[3]>1.4):
            cv2.waitKey(0)
        else:
            cv2.waitKey(1)
    return (H, quality, pano)

def getAngleV(v1,v2, isNorm=False):
    pdot=np.dot(v1,v2)
    pcrs=np.cross(v1,v2)
    ang=(180./np.pi)*np.arctan2(pcrs,pdot)
    if isNorm:
        if ang<0.:
            return 360.+ang
    return (180./np.pi)*np.arctan2(pcrs,pdot)

def projV2H(H,v):
    v=np.array((v[0],v[1],1.))
    ret=np.dot(H,v)
    return np.array( (ret[0]/ret[2], ret[1]/ret[2]) )

"""
:return (mean-angle, mean-scale)
"""
def calcScaleAndAngle(H, isNormAngle=False):
    v0=np.array((0,0,1))
    vx=np.array((1,0,1)) #FIXME: check this point (X,Y)
    vy=np.array((0,1,1))
    v0p=np.dot(H,v0)
    v0p=v0p[:2]/v0p[2]
    vxp=np.dot(H,vx)
    vyp=np.dot(H,vy)
    vxp=vxp[:2]/vxp[2]
    vyp=vyp[:2]/vyp[2]
    vxp-=v0p
    vyp-=v0p
    sx=np.sqrt(np.dot(vxp,vxp))
    sy=np.sqrt(np.dot(vyp,vyp))
    ax=getAngleV(vx[:2], vxp, isNorm=isNormAngle)
    ay=getAngleV(vy[:2], vyp, isNorm=isNormAngle)
    print "v0p=%s, vxp=%s, vyp=%s, sx/sy=%f/%f, ax/ay=%f/%f" % (v0p, vxp, vyp, sx,sy, ax,ay)
    return ((ax+ay)/2., (sx+sy)/2.)

"""
:return (dx,dy, mean-scale, mean-angle)
"""
def calcShiftAngleScale(H, pc, isNormAngle=False):
    assert(len(pc)==2)
    v0=np.array((pc[0],pc[1],1))
    vx=v0+np.array((1,0,0)) #FIXME: check this point (X,Y)
    vy=v0+np.array((0,1,0))
    v0p=np.dot(H,v0)
    vxp=np.dot(H,vx)-v0p
    vyp=np.dot(H,vy)-v0p
    sx=np.sqrt(np.dot(vxp[:2],vxp[:2]))
    sy=np.sqrt(np.dot(vyp[:2],vyp[:2]))
    ax=getAngleV(vx[:2], vxp[:2], isNorm=isNormAngle)
    ay=getAngleV(vy[:2], vyp[:2], isNorm=isNormAngle)
    dxy=v0p-v0
    return (dxy[0], dxy[1], (ax+ay)/2., (sx+sy)/2.)

def calcShiftAngleScale2(H, pc, isNormAngle=False):
    assert(len(pc)==2)
    v0=np.array((pc[0],pc[1]))
    v0p=projV2H(H,v0)
    dxy=v0p-v0
    #
    vx=np.array((1,0))
    vy=np.array((0,1))
    H22=H[2,2]**2
    drxx=(H[0,0]*H[2,2]-H[0,2]*H[2,0])/H22
    drxy=(H[1,0]*H[2,2]-H[1,2]*H[2,0])/H22
    dryx=(H[0,1]*H[2,2]-H[2,1]*H[0,2])/H22
    dryy=(H[1,1]*H[2,2]-H[1,2]*H[2,1])/H22
    drx=np.array((drxx,drxy))
    dry=np.array((dryx,dryy))
    sx=np.sqrt(np.dot(drx,drx))
    sy=np.sqrt(np.dot(dry,dry))
    ax=getAngleV(vx, drx, isNorm=isNormAngle)
    ay=getAngleV(vy, dry, isNorm=isNormAngle)
    return (-dxy[0], -dxy[1], (ax+ay)/2., (sx+sy)/2.)

if __name__=='__main__':

    # v1=np.array((1,0))
    # v2=np.array((0,-1))
    # print getAngleV(v1,v2, isNorm=True)
    # sys.exit()

    # plt.figure(0)
    # numv=36
    # arrV=np.zeros((numv,2), np.float)
    # for ii in xrange(numv):
    #     ang=(np.pi/180.)*360.*ii/(numv-1)
    #     arrV[ii,0]=np.cos(ang)
    #     arrV[ii,1]=np.sin(ang)
    # plt.subplot(121)
    # plt.plot(arrV[:,0], arrV[:,1])
    # plt.grid(True)
    # arrA=np.zeros((numv,1))
    # for ii in xrange(arrV.shape[0]):
    #     arrA[ii]=getAngleV(arrV[ii,:], np.array([1,0]), isNorm=True)
    #     # print arrV[ii,:]
    # plt.subplot(122)
    # plt.plot(arrA)
    # plt.grid(True)
    # plt.show()
    # sys.exit(0)

    # img1=cv2.imread(fimg1,0)
    # img2=cv2.imread(fimg2,0)
    # ret=imregFeaturePoins(img1,img2)
    # detector=cv2.SIFT()
    detector=cv2.SURF()
    # detector=cv2.ORB(nfeatures=1000)


    wdirOut='%s_processing' % fvideo
    if os.path.isdir(wdirOut):
        shutil.rmtree(wdirOut)
    os.mkdir(wdirOut)

    parThresholdQ=0.3
    parMaxFrameSize=320
    numSkip=3
    numSkipFirst=0
    videoReader=None
    isTrueVideo=False
    try:
        videoReader=ph.VideoCSVReader(fvideo)
        videoReader.printInfo()
    except:
        videoReader=cv2.VideoCapture(fvideo)
        isTrueVideo=True

    isFirstFrame=True
    frmPrev=None
    frmPrevName=None
    listH=[]
    listQ=[]
    fsiz=None
    cnt=0
    dscName='SURF'
    while True:
        if isFirstFrame:
            for ii in xrange(numSkipFirst):
                ret,frm,frmName=videoReader.readWithName()
                cnt +=1
                print 'skip %d' % cnt
        ret,frm,frmName=videoReader.readWithName()
        cnt+=1
        if isTrueVideo:
            for kk in xrange(numSkip):
                ret,frm,frmName=videoReader.readWithName()
                cnt+=1
        # if cnt>60:
        #     break
        if ret:
            if len(frm.shape)>2:
                frm=cv2.cvtColor(frm, cv2.COLOR_BGR2GRAY)
            if parMaxFrameSize>1:
                frm=ph.resizeToMaxSize(frm, parMaxFrameSize)
            frmOut='%s/frame_%04d.png' % (wdirOut, cnt)
            frmOutPano='%s_pano.png' % frmOut
            cv2.imwrite(frmOut, frm)
            if isFirstFrame:
                fsiz=np.array(frm.shape[0:2][::-1], np.float)
                frmPrev=frm
                frmPrevName=frmName
                isFirstFrame=False
                continue

            fkpPrev='%s_%s.kp' % (frmPrevName, dscName)
            fkp    ='%s_%s.kp' % (frmName,     dscName)
            kpPrev=fp.file2kp(fkpPrev)
            kp    =fp.file2kp(fkp)
            H,Q,Pano=imregFeaturePoins(frmPrev, frm, (kpPrev, kp), isDebug=True, detector=detector)
            cv2.imwrite(frmOutPano, Pano)
            if Q<parThresholdQ:
                if len(listH)==0:
                    H=np.eye(3,3)
                else:
                    H=listH[-1].copy()
            if len(listH)>0:
                H0=listH[-1]
                listH.append(np.dot(H0,H))
            else:
                listH.append(H)
            #
            tdxyas=calcShiftAngleScale2(listH[-1],np.array(frm.shape[:2][::-1])/2.0)
            if (tdxyas[3]<0.8) or (tdxyas[3]>1.4):
                print '--------------------------------------'
                print listH[-3]
                print listH[-2]
                print listH[-1]
                print "!!!WARNING!!! ", tdxyas
                cv2.waitKey(0)
            #
            listQ.append(Q)
            frmPrev=frm.copy()
            frmPrevName=frmName
        else:
            print 'ERROR: bad frame ... [skip]'
            break
    print listQ

    nump=len(listQ)
    arrXYASQ=np.zeros((nump,5))
    for ii in xrange(nump):
        H=listH[ii]
        # print H
        vc=np.array([fsiz[0]/2.0,fsiz[1]/2.0, 1.0])
        vcp=np.dot(H,vc)
        vcp/=vcp[2]
        # arrXY[ii,0]=vcp[0]-vc[0]
        # arrXY[ii,1]=vcp[1]-vc[1]
        txyas=calcShiftAngleScale2(H, vc[:2], isNormAngle=False)
        arrXYASQ[ii,0]=txyas[0]
        arrXYASQ[ii,1]=txyas[1]
        arrXYASQ[ii,2]=txyas[2]
        arrXYASQ[ii,3]=txyas[3]
        arrXYASQ[ii,4]=listQ[ii]
    #
    foutCSVTrj='%s/trajectory_info.csv' % wdirOut
    np.savetxt(foutCSVTrj, arrXYASQ, delimiter=',')
    #
    plt.figure(1)
    plt.subplot(221)
    plt.plot(arrXYASQ[:,4])
    plt.ylim(0,1.2)
    plt.grid(True)
    plt.title('Quality')
    #
    plt.subplot(222)
    plt.plot(arrXYASQ[:,2])
    plt.title('Angle')
    plt.grid(True)
    #
    plt.subplot(223)
    plt.plot(arrXYASQ[:,3])
    plt.title('Scale')
    plt.grid(True)
    #
    plt.subplot(224)
    plt.plot(arrXYASQ[:,0], arrXYASQ[:,1])
    vmin=np.min(np.abs(arrXYASQ[:,0:2]))
    vmax=np.max(np.abs(arrXYASQ[:,0:2]))
    print listH[ 0]
    print listH[-1]
    # cv2.destroyAllWindows()
    plt.xlim(-vmax, vmax)
    plt.ylim(-vmax, vmax)
    plt.grid(True)
    plt.show()


    # detector=cv2.SIFT()
    # # detector=cv2.SURF()
    # # detector=cv2.ORB(nfeatures=500)
    # matcher=cv2.BFMatcher(cv2.NORM_L2)
    # kk1,dd1=detector.detectAndCompute(img1, None)
    # kk2,dd2=detector.detectAndCompute(img2, None)
    # matches0=matcher.match(dd1, dd2)
    # matches1=matcher.match(dd2, dd1)
    # listGoodMatches=[]
    # listP1=[]
    # listP2=[]
    # listDistances=[]
    # for mm0 in matches0:
    #     if mm0.queryIdx==matches1[mm0.trainIdx].trainIdx:
    #         listGoodMatches.append(mm0)
    #         listDistances.append(mm0.distance)
    #         listP1.append(kk1[mm0.queryIdx].pt)
    #         listP2.append(kk2[mm0.trainIdx].pt)
    #         # print "(%d) %d -> %d -> %d : %0.2f/%0.2f" % (mm0.imgIdx, mm0.queryIdx, mm0.trainIdx, matches1[mm0.trainIdx].trainIdx, mm0.distance, matches1[mm0.trainIdx].distance)
    # print "good/total = %d/%d, dist min/max=(%0.2f, %0.2f)" % (len(listGoodMatches), len(matches0), min(listDistances), max(listDistances))
    # listP1=np.array(listP1, dtype=np.float32)
    # listP2=np.array(listP2, dtype=np.float32)
    # H,status = cv2.findHomography(listP1,listP2,cv2.RANSAC, 5.0)
    # print '%d / %d  inliers/matched' % (np.sum(status), len(status))
    # print H
    # #
    # img1v=img1.copy()
    # img2v=img2.copy()
    # plotFPoints(img1v, kk1, rad=3)
    # plotFPoints(img2v, kk2, rad=3)
    # cv2.imshow("win-original", np.concatenate((img1v,img2v), axis=1))
    cv2.waitKey(0)
