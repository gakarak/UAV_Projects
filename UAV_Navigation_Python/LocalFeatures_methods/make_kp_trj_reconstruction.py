#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import os
import sys

import matplotlib.pyplot as plt

import pickle as pik

import libfp as fp
import libphcorr as ph

# fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_proc/idx.csv'
# fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'

# fvideo='/home/ar/video/UAV_PHONE/part_2/vid1_1to2.mp4_proc/idx.csv'
# fvideo='/home/ar/video/UAV_PHONE/part_2/vid1_2to1.mp4_proc/idx.csv'

# fvideo='/home/ar/video/UAV_PHONE/part_2/vid1_1to2.mp4_proc2/idx.csv'
# fvideo='/home/ar/video/UAV_PHONE/part_2/vid1_2to1.mp4_proc2/idx.csv'
# fvideo='/media/big4/big.data/shared.vmware/git-bitbucket.org/build-FlightSimulator-Qt_5_4_1_x64-Debug/datalog_22.05.2015_18-28/screenfg_good/idx.csv'

# fvideo='/home/ar/video/cam_video/VID_20150402_104326_crop1.mp4_proc/idx.csv'
# fvideo='/home/ar/video/cam_video/VID_20150402_104326_crop2.mp4_proc/idx.csv'

# fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/idx.csv'

fvideo='/home/ar/video/Video_Murashko/data_1fps_resize/idx.csv'

# pcFrm=(768/2, 432/2)
# pcFrm=(311/2, 342/2)
pcFrm=(640/2, 512/2)

# fvideo='/home/ar/video/data_test_frames_rot_scl_0/idx.csv'
# pcFrm=(257/2, 257/2)
dscName='SURF'


###############################
def getHomography(pk1, pd1, pk2, pd2):
    matcher=cv2.BFMatcher(cv2.NORM_L2)
    matches0=matcher.match(pd1, pd2)
    matches1=matcher.match(pd2, pd1)
    listGoodMatches=[]
    listP1=[]
    listP2=[]
    # retlP1=[]
    # retlP2=[]
    listDistances=[]
    for mm0 in matches0:
        if mm0.queryIdx==matches1[mm0.trainIdx].trainIdx:
            listGoodMatches.append(mm0)
            listDistances.append(mm0.distance)
            listP1.append(pk1[mm0.queryIdx].pt)
            listP2.append(pk2[mm0.trainIdx].pt)
            # retlP1.append(pk1[mm0.queryIdx])
            # retlP2.append(pk2[mm0.trainIdx])
    listP1=np.array(listP1, dtype=np.float32)
    listP2=np.array(listP2, dtype=np.float32)
    H,status = cv2.findHomography(listP1,listP2,cv2.RANSAC, 5.0)
    # return (H, status, retlP1, retlP2, listGoodMatches)
    return (H, np.sum(status), len(status))

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
    #
    retCurl=np.abs(H[0,1]-H[1,0])
    retDfrm=np.abs(H[0,0]-H[1,1])
    a0=(180./np.pi)*np.arctan2(-H[0,1],H[0,0])
    a1=(180./np.pi)*np.arctan2(+H[1,0],H[1,1])
    # return (dxy[0], dxy[1], (ax+ay)/2., (sx+sy)/2.)
    return (-dxy[0], -dxy[1], (ax+ay)/2., np.sqrt((sx**2+sy**2)/2.),   (ax,ay), (sx,sy), (retCurl,retDfrm),  (a0, a1))

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
    # return (-dxy[0], -dxy[1], (ax+ay)/2., (sx+sy)/2.)
    retCurl=np.abs(H[0,1]-H[1,0])
    retDfrm=np.abs(H[0,0]-H[1,1])
    a0=(180./np.pi)*np.arctan2(-H[0,1],H[0,0])
    a1=(180./np.pi)*np.arctan2(+H[1,0],H[1,1])
    return (-dxy[0], -dxy[1], (ax+ay)/2., np.sqrt((sx**2+sy**2)/2.),   (ax,ay), (sx,sy), (retCurl,retDfrm),  (a0, a1))
    # return ((ax,ay), (sx,sy), (retCurl,retDfrm),  (a0, a1))
    # return ((ax+ay)/2.0, (sx,sy), (retCurl,retDfrm),  (a0+a1)/2.0)


###############################
class HomoBuff():
    def __init__(self, pcf):
        self.thCurl=0.01
        self.thDfrm=0.01
        self.reset(pcf)
        self.a2r=(np.pi/180.)
    def reset(self, pcf):
        self.currH=np.eye(3)
        self.stopCoord=[0.0, 0.0, 0.0, 1.0]
        self.listTrj=[]
        self.listTrj.append(self.stopCoord)
        self.pcf=pcf
    # def appendCoord(self, coord):
    #     tmp=self.currCoord[-1]
    #     self.currCoord[0]+=
    def resetStopPoint(self, pCoord):
        self.stopCoord[0]+=pCoord[0]
        self.stopCoord[1]+=pCoord[1]
        self.stopCoord[2]+=pCoord[2]
        self.stopCoord[3]*=pCoord[3]
        self.currH=np.eye(3)
    def getCurlDeform(self, pH):
        retCurl=np.abs(pH[0,1]-pH[1,0])
        retDfrm=np.abs(pH[0,0]-pH[1,1])
        return (retCurl, retDfrm)
    def pushH(self, pH):
        tmpH=np.dot(self.currH, pH)
        retCD=self.getCurlDeform(self.currH)
        if (retCD[0]>self.thCurl) or (retCD[1]>self.thDfrm):
            self.stopCoord=self.listTrj[-1]
            self.currH=pH.copy()
            print '***RESET***   * HomoBuff()'
        else:
            self.currH=tmpH.copy()
        tmpRet=calcShiftAngleScale2(self.currH, self.pcf, isNormAngle=False)
        angOld=self.stopCoord[2]
        dx=self.stopCoord[0] - (tmpRet[0]*np.cos(self.a2r*angOld) - tmpRet[1]*np.sin(self.a2r*angOld))*self.stopCoord[3]
        dy=self.stopCoord[1] - (tmpRet[0]*np.sin(self.a2r*angOld) + tmpRet[1]*np.cos(self.a2r*angOld))*self.stopCoord[3]
        da=+tmpRet[2]+self.stopCoord[2]
        ds=+tmpRet[3]*self.stopCoord[3]
        newCoord=(dx,dy,da,ds)
        self.listTrj.append(newCoord)
    def getTrj(self):
        return np.array(self.listTrj[1:])
    def printInfo(self):
        print self.getTrj()

def calcFrameKPScore(lstKP):
    kpResp=[xx.response for xx in lstKP]
    return (np.mean(kpResp), np.std(kpResp), len(lstKP))

###############################
if __name__=='__main__':
    cap=ph.VideoCSVReader(fvideo)
    cap.printInfo()
    lstKP=[]
    lstKD=[]
    lstKPData=[]
    numFrm=cap.getNumFrames()
    numFrm=6
    totH=None
    homoBuff=HomoBuff(pcFrm)
    curlDefData=np.zeros((numFrm,2))
    arrCoords=[]
    imgOld=None
    imgCurr=None
    for ii in xrange(0, numFrm):
        fkp=cap.getKPByID(ii, dscName)
        kpData=fp.file2kp(fkp)
        kpScore=calcFrameKPScore(kpData[0])
        # print kpScore
        # lstKP.append(kpData[0])
        # lstKD.append(kpData[1])
        tmpH=np.eye(3)
        if ii==0:
            # totH=np.eye(3)
            totH=tmpH.copy()
            imgOld=cv2.imread(cap.listFImg[ii], 0)
        else:
            oldKPData=lstKPData[-1]
            tmpH,_,_=getHomography(kpData[0], kpData[1],   oldKPData[0], oldKPData[1])
            # tmpH,_,_=getHomography(oldKPData[0], oldKPData[1],   kpData[0], kpData[1])
            totH=np.dot(totH, tmpH)
            imgCurr=cv2.imread(cap.listFImg[ii], 0)
            # imgOldWrap=cv2.warpPerspective(imgOld, tmpH, (imgOld.shape[1], imgOld.shape[0]))
            imgCurrWrap=cv2.warpPerspective(imgCurr, tmpH, (imgCurr.shape[1], imgCurr.shape[0]))
            tStr="win (%d)->(%d)" % (ii-1, ii)
            cv2.imshow(tStr, np.dstack( (imgOld, imgCurrWrap, imgOld) ))
            print "(*) ", tStr, '\n' , tmpH
            cv2.waitKey(3)
            imgOld=imgCurr.copy()
        homoBuff.pushH(tmpH)
        lstKPData.append(kpData)
        tmpRet=calcShiftAngleScale(tmpH, pcFrm)
        print ii, "/", numFrm, "  *  ", tmpRet
        curlDefData[ii,0]=tmpRet[-2][0]
        curlDefData[ii,1]=tmpRet[-2][1]
        tmpCoord=(-tmpRet[0],-tmpRet[1],tmpRet[2],tmpRet[3],0.9, kpScore[0], kpScore[2])
        arrCoords.append(tmpCoord)
        # print fkp
    # print lstH
    print '-------------'
    print calcShiftAngleScale2(totH, pcFrm)
    print '-------------'
    #
    homoBuff.printInfo()
    ftrjOut='%s_trjfp.csv' % fvideo
    # np.savetxt(ftrjOut, np.array(arrCoords, dtype=float), delimiter=',')
    np.savetxt(ftrjOut, np.array(homoBuff.getTrj(), dtype=float), delimiter=',')
    #
    # cmdRun="python gui_qt4_trj_preview.py %s" % fvideo
    # os.system(cmdRun)
    cv2.waitKey(0)
    #
    # plt.hold(True)
    # plt.plot(curlDefData[:,0])
    # plt.plot(curlDefData[:,1])
    # plt.grid(True)
    # plt.show()

