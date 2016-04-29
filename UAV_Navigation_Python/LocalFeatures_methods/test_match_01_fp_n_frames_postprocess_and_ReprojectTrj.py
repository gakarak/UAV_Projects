#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import numpy.matlib
import os
import sys

import libphcorr as ph
import libfp as fp
import matplotlib.pyplot as plt


# fimg0='/home/ar/img/lena.png'
# fimg1='/home/ar/img/lena_b_rot_scale_crop.png'


# fimg0='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_proc2/image-021.jpeg'
# fimg1='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc2/image-293.jpeg'

# fimg0='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/image-101.jpeg'
# fimg1='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/frm_00465.png'

# fvideo0='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/idx.csv'
# fvideo1='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'

fvideo0='/home/ar/video/UAV_PHONE/DJI/DJI_0005_MOV_CROP_StonePark/frames-crop1/idx.csv'
fvideo1='/home/ar/video/UAV_PHONE/DJI/DJI_0005_MOV_CROP_StonePark/frames-crop4/idx.csv'

FLANN_INDEX_KDTREE=1

####################################
def resizeImageToMaxSize(img, maxSize):
    if max(img.shape)>maxSize:
        nr=img.shape[0]
        nc=img.shape[1]
        newSize=(maxSize, int(round(float(nr)*maxSize/float(nc))))
        if nr>nc:
            newSize=(int(round(float(nc)*maxSize/float(nr))), maxSize)
        return cv2.resize(img, newSize)
    return img.copy()

def drawMatches(img1, img2, lkp1, lkp2, status, num=10):
    imgm1=cv2.drawKeypoints(img1, lkp1, color=(0,0,255))
    imgm2=cv2.drawKeypoints(img2, lkp2, color=(0,0,255))
    w1=imgm1.shape[1]
    totimg=np.concatenate((imgm1.copy(),imgm2.copy()), axis=1).copy()
    cnt=0
    cntb=0
    for pp1,pp2 in zip(lkp1,lkp2):
        if status[cnt]==1:
            p1=(int(pp1.pt[0]),    int(pp1.pt[1]))
            p2=(int(pp2.pt[0])+w1, int(pp2.pt[1]))
            cv2.line(totimg,p1,p2,(0,255,0))
            if cntb>num:
                break
            cntb+=1
        cnt+=1
    return totimg

def drawMatchesAndHomography(img1, img2, lkp1, lkp2, status, H, num=10):
    imgm1=cv2.drawKeypoints(img1, lkp1, color=(0,0,255))
    imgm2=cv2.drawKeypoints(img2, lkp2, color=(0,0,255))
    w1=imgm1.shape[1]
    #
    siz1=(img1.shape[1],img1.shape[0])
    arrROI=np.array([(0,0,1), (siz1[0],0,1), (siz1[0],siz1[1],1), (0,siz1[1],1), (0,0,1)])
    prjROI=np.transpose(np.dot(H,np.transpose(arrROI)))
    prjROI=prjROI/np.matlib.repmat(prjROI[:,2],3,1).transpose()
    # img2n=imgm2.copy()
    for ii in xrange(arrROI.shape[0]-1):
            p1=(int(prjROI[ii+0,0] + 0*siz1[0]), int(prjROI[ii+0,1]))
            p2=(int(prjROI[ii+1,0] + 0*siz1[0]), int(prjROI[ii+1,1]))
            cv2.line(imgm2, p1,p2, color=(0,255,0), thickness=3)
    #
    totimg=np.concatenate((imgm1.copy(),imgm2.copy()), axis=1).copy()
    cnt=0
    cntb=0
    for pp1,pp2 in zip(lkp1,lkp2):
        if status[cnt]==1:
            p1=(int(pp1.pt[0]),    int(pp1.pt[1]))
            p2=(int(pp2.pt[0])+w1, int(pp2.pt[1]))
            cv2.line(totimg,p1,p2,(0,255,0))
            if cntb>num:
                break
            cntb+=1
        cnt+=1
    return totimg

def getHomography(pk1, pd1, pk2, pd2):
    matcher=cv2.BFMatcher(cv2.NORM_L2)
    matches0=matcher.match(pd1, pd2)
    matches1=matcher.match(pd2, pd1)
    listGoodMatches=[]
    listP1=[]
    listP2=[]
    retlP1=[]
    retlP2=[]
    listDistances=[]
    for mm0 in matches0:
        if mm0.queryIdx==matches1[mm0.trainIdx].trainIdx:
            listGoodMatches.append(mm0)
            listDistances.append(mm0.distance)
            listP1.append(pk1[mm0.queryIdx].pt)
            listP2.append(pk2[mm0.trainIdx].pt)
            retlP1.append(pk1[mm0.queryIdx])
            retlP2.append(pk2[mm0.trainIdx])
    listP1=np.array(listP1, dtype=np.float32)
    listP2=np.array(listP2, dtype=np.float32)
    H,status = cv2.findHomography(listP2,listP1,cv2.RANSAC, 5.0)
    status=status.reshape(-1)
    arrDst=np.array([xx.distance for xx in listGoodMatches])
    arrDst=arrDst[status==1]
    parScore=(np.sum(status), 100.*np.sum(status)/len(status), np.mean(arrDst), np.std(arrDst))
    return (H, status.reshape(-1), retlP1, retlP2, listGoodMatches, parScore)

def filter_matches(kp1, kp2, matches, ratio = 0.75):
    mkp1, mkp2 = [], []
    gmatches=[]
    for m in matches:
        if len(m) == 2 and m[0].distance < m[1].distance * ratio:
            m = m[0]
            mkp1.append( kp1[m.queryIdx] )
            mkp2.append( kp2[m.trainIdx] )
            gmatches.append(m)
    p1 = np.float32([kp.pt for kp in mkp1])
    p2 = np.float32([kp.pt for kp in mkp2])
    kp_pairs = zip(mkp1, mkp2)
    return p1, p2, kp_pairs, mkp1, mkp2, gmatches

def getHomography2(pk1, pd1, pk2, pd2):
    flann_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
    matcher = cv2.FlannBasedMatcher(flann_params, {})
    matcher.add([pd2])
    matcher.train()
    # raw_matches = matcher.knnMatch(pd1, trainDescriptors = pd2, k = 2)
    raw_matches = matcher.knnMatch(pd1, k = 2)
    listP1, listP2, _, retlP1, retlP2, listGoodMatches = filter_matches(pk1, pk2, raw_matches)
    H,status = cv2.findHomography(listP2,listP1,cv2.RANSAC, 5.0)
    status=status.reshape(-1)
    arrDst=np.array([xx.distance for xx in listGoodMatches])
    arrDst=arrDst[status==1]
    parScore=(np.sum(status), 100.*np.sum(status)/len(status), np.mean(arrDst), np.std(arrDst))
    return (H, status.reshape(-1), retlP1, retlP2, listGoodMatches, parScore)

def calcCorrCoeff(img1, img2):
    tmp1=img1.reshape(-1).astype(np.float)
    tmp2=img2.reshape(-1).astype(np.float)
    tmp1=(tmp1-np.mean(tmp1))/np.std(tmp1)
    tmp2=(tmp2-np.mean(tmp2))/np.std(tmp2)
    ret=np.sum(tmp1*tmp2)/len(tmp1)
    return ret

def calcCCHomography(imgBig, imgSmall, H):
    siz=imgSmall.shape
    borderSiz=(0.15*np.array(siz)).astype(np.int)
    imgBigWarpCrop=cv2.warpPerspective(imgBig, np.linalg.inv(H), (siz[1], siz[0]))
    imgBigWarpCrop=imgBigWarpCrop[borderSiz[0]:-borderSiz[0], borderSiz[1]:-borderSiz[1]]
    imgSmallCrop=imgSmall[borderSiz[0]:-borderSiz[0], borderSiz[1]:-borderSiz[1]]
    return calcCorrCoeff(imgBigWarpCrop, imgSmallCrop)

####################################
if __name__=='__main__':
    dscName='SURF'
    cap0=ph.VideoCSVReader(fvideo0)
    cap1=ph.VideoCSVReader(fvideo1)
    numFrm0=cap0.getNumFrames()
    numFrm1=cap1.getNumFrames()
    #
    fcsvTrj0='%s_trjfp.csv' % fvideo0
    fcsvTrj1='%s_trjfp.csv' % fvideo1
    if not os.path.isfile(fcsvTrj0):
        print 'ERROR: cant find Trj-file [%s]' % fcsvTrj0
        sys.exit(1)
    if not os.path.isfile(fcsvTrj1):
        print 'ERROR: cant find Trj-file [%s]' % fcsvTrj1
        sys.exit(1)
    trj0=np.genfromtxt(fcsvTrj0, delimiter=',')
    trj1=np.genfromtxt(fcsvTrj1, delimiter=',')
    #
    dir0=os.path.basename(os.path.dirname(fvideo0))
    dir1=os.path.basename(os.path.dirname(fvideo1))
    wdir=os.path.dirname(os.path.dirname(fvideo0))
    foutDstCorr="%s/%s_match_to_%s.csv" % (wdir,dir0,dir1)
    foutPts2Pts="%s_pts2pts.csv" % foutDstCorr
    #
    dstCorr=np.genfromtxt(foutDstCorr, delimiter=',')
    dstCorrNorm=cv2.normalize(dstCorr,None,0,255,cv2.NORM_MINMAX,cv2.CV_8U)
    dstCorrNorm=cv2.cvtColor(dstCorrNorm, cv2.COLOR_GRAY2BGR)
    plt.imshow(dstCorr)
    plt.title('Correlation MAP: Frame Sequence-to-Sequence')
    plt.show()
    tsiz=(dstCorrNorm.shape[1]*4,dstCorrNorm.shape[0]*4)
    cv2.imshow("corr-map", cv2.resize(dstCorrNorm,tsiz))
    # cv2.imshow("corr-map", dstCorrNorm)
    cv2.waitKey()
    numFrm0=dstCorr.shape[0]
    numFrm1=dstCorr.shape[1]
    dirOut='%s/%s_%s_matches' % (wdir, dir0, dir1)
    if not os.path.isdir(dirOut):
        os.mkdir(dirOut)
    arrPts2Pts=[]
    isStart=False
    ccTresh=0.7
    for ii0 in xrange(numFrm0):
        tdst=dstCorr[ii0,:]
        idxMax=np.argmax(tdst)
        ccMax=tdst[idxMax]
        if ccMax>ccTresh:
            ii1=idxMax
            _,img0=cap0.read(ii0)
            _,img1=cap1.read(ii1)
            fkp0=cap0.getKPByID(ii0, dscName)
            kp0,kd0=fp.file2kp(fkp0)
            fkp1=cap1.getKPByID(ii1, dscName)
            kp1,kd1=fp.file2kp(fkp1)
            # H, status, mKP0, mKP1, listGoodMatches, scores= getHomography2(kp0, kd0, kp1, kd1)
            H, status, mKP1, mKP0, listGoodMatches, scores= getHomography2(kp0, kd0, kp1, kd1)
            cc=calcCCHomography(img0, img1, H)
            if cc<ccTresh:
                continue
            imgMatch=drawMatchesAndHomography(img1, img0, mKP0, mKP1, status, H, num=100)
            imgMatch=resizeImageToMaxSize(imgMatch,1700)
            #
            pCenter=np.array( (img0.shape[1]/2, img0.shape[0]/2, 1) )
            pCenterH=np.dot(H,pCenter)
            pCenterH=pCenterH/pCenterH[2]
            dx=pCenterH[0]-img0.shape[1]/2
            dy=pCenterH[1]-img0.shape[0]/2
            da=np.deg2rad(trj0[ii0,2])
            ds=np.deg2rad(trj0[ii0,3])
            cosa=np.cos(da)
            sina=np.sin(da)
            dx2=+dx*cosa+dy*sina
            dy2=-dx*sina+dy*cosa
            Xnew=trj0[ii0,0]+dx2*ds
            Ynew=trj0[ii0,1]+dy2*ds
            Xold=trj1[ii1,0]
            Yold=trj1[ii1,1]
            arrPts2Pts.append(((Xold,Yold,Xnew,Ynew)))
            #
            pCenterH[0]+=img0.shape[1]
            cv2.circle(imgMatch,tuple(pCenterH[:2].astype(int)), 15, (255,255,255), 2)
            #
            winName='[%d,%d] corr=%0.2f, Score=%d/%d (%0.1f%%)' % (ii0, ii1, cc, np.sum(status), len(status), 100.*float(np.sum(status))/float(len(status)))
            fimgOut='%s/match_%d_to_%d_corr=%0.1f.png' % (dirOut, ii0, ii1, cc)
            cv2.putText(imgMatch, winName, (30,30), cv2.FONT_HERSHEY_PLAIN,1.4,(0,0,0),6)
            cv2.putText(imgMatch, winName, (30,30), cv2.FONT_HERSHEY_PLAIN,1.4,(255,255,255),2)
            cv2.imwrite(fimgOut, imgMatch)
            # cv2.imshow(winName, imgMatch)
            cv2.imshow('match-frames', imgMatch)
            #
            tmpCCMap=dstCorrNorm.copy()
            cv2.circle(tmpCCMap,(ii1,ii0),4,(0,0,255),2)
            tsiz=(tmpCCMap.shape[1]*4,tmpCCMap.shape[0]*4)
            cv2.imshow("corr-map", cv2.resize(tmpCCMap,tsiz))
            if not isStart:
                cv2.waitKey()
                isStart=True
            else:
                cv2.waitKey(20)
            # print 'matches:\t%s\n->\t%s\n\n' % (fkp0, fkp1)
    arrPts2PtsNP=np.array(arrPts2Pts)
    np.savetxt(foutPts2Pts, arrPts2PtsNP, delimiter=',')
    cv2.waitKey()
    #
    # dstCorr=np.genfromtxt(foutDstCorr, delimiter=',')
    # RR,CC=np.where(dstCorr>0.4)
    # RR=[71]
    # CC=[145]
    # numGood=len(RR)
    # for ii in xrange(numGood):
    #     ii0=RR[ii]
    #     ii1=CC[ii]
    #     _,img0=cap0.read(ii0)
    #     _,img1=cap1.read(ii1)
    #     fkp0=cap0.getKPByID(ii0, dscName)
    #     kp0,kd0=fp.file2kp(fkp0)
    #     fkp1=cap1.getKPByID(ii1, dscName)
    #     kp1,kd1=fp.file2kp(fkp1)
    #     H, status, mKP0, mKP1, listGoodMatches, scores= getHomography2(kp0, kd0, kp1, kd1)
    #     cc=calcCCHomography(img0, img1, H)
    #     imgMatch=drawMatchesAndHomography(img1, img0, mKP0, mKP1, status, H, num=100)
    #     winName='[%d,%d] corr=%0.2f' % (ii0, ii1, cc)
    #     cv2.imshow(winName, imgMatch)
    #     cv2.waitKey(10)
    #     print 'matches:\t%s\n->\t%s\n\n' % (fkp0, fkp1)
    # cv2.waitKey(0)

    # dstCorr=np.zeros( (numFrm0, numFrm1) )
    # for ii0 in xrange(numFrm0):
    #     _,img0=cap0.read(ii0)
    #     fkp0=cap0.getKPByID(ii0, dscName)
    #     kp0,kd0=fp.file2kp(fkp0)
    #     dstCorr0=np.zeros( (numFrm1,1) )
    #     foutDstCorr0="%s/%s_match_to_%s_%d.csv" % (wdir,dir0,dir1, ii0)
    #     if os.path.isfile(foutDstCorr0):
    #         continue
    #     for ii1 in xrange(numFrm1):
    #         _,img1=cap1.read(ii1)
    #         fkp1=cap1.getKPByID(ii1, dscName)
    #         kp1,kd1=fp.file2kp(fkp1)
    #         H, status, mKP0, mKP1, listGoodMatches, scores= getHomography2(kp0, kd0, kp1, kd1)
    #         cc=calcCCHomography(img0, img1, H)
    #         dstCorr[ii0,ii1]=cc
    #         dstCorr0[ii1]=cc
    #         if cc>0.2:
    #             print "Corr[%d,%d]=%0.2f !!!!" % (ii0,ii1,cc)
    #         else:
    #             print "Corr[%d,%d]=%0.2f" % (ii0,ii1,cc)
    #     np.savetxt(foutDstCorr0, dstCorr0, delimiter=',')

