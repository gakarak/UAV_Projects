#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import math
import pylab as pl

kdif=2.0

# fnfrm1='/home/ar/video/drone_project/2/frame_364_640x480.3.png'
# fnfrm2='/home/ar/video/drone_project/2/frame_420_640x480.3.png'

# wdir='/media/ar/Igor_1_TB/@UAV/TestVideoSimple1'
wdir='/media/Igor_1_TB/@UAV/TestVideoSimple1'
lst_imdir=['VID_20150324_020549', 'VID_20150324_020623', 'VID_20150324_020646']

# fnfrm1='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-013.jpeg'
# fnfrm2='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-014.jpeg'

# xy_frm1=np.array([825, 618])
# xy_frm2=np.array([1147, 675])

def calcPhaseCorrShift(fimg1, fimg2):
    frm1=cv2.imread(fimg1, 0)
    frm2=cv2.imread(fimg2, 0)
    frm1=cv2.resize(frm1.astype(np.float).copy(), (int(frm1.shape[1]/kdif), int(frm1.shape[0]/kdif)))
    frm2=cv2.resize(frm2.astype(np.float).copy(), (int(frm2.shape[1]/kdif), int(frm2.shape[0]/kdif)))
    frm1_nrm=cv2.normalize(frm1.copy(), None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    frm2_nrm=cv2.normalize(frm2.copy(), None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    hann=cv2.createHanningWindow((frm1.shape[1], frm1.shape[0]), cv2.CV_64F)
    #
    shift = cv2.phaseCorrelate(frm1, frm2, hann)
    dxy=shift[0]
    frm2_nrm_shift=np.roll(frm2_nrm, int(math.floor(-dxy[0])), 1)
    frm2_nrm_shift=np.roll(frm2_nrm_shift, int(math.floor(-dxy[1])), 0)
    tmp=np.zeros((frm1.shape[0], frm1.shape[1], 3), np.uint8)
    tmp[:,:,2]=frm1_nrm
    tmp[:,:,1]=frm2_nrm_shift
    tmp[:,:,0]=0
    # tmp[:,:,1]=frm2_nrm
    cv2.imshow("frame #2 shift",  cv2.resize(tmp, (tmp.shape[1]/1, tmp.shape[0]/1)))
    return dxy

def calcGFTTShift(fimg1, fimg2):
    frm1=cv2.imread(fimg1, 0)
    frm2=cv2.imread(fimg2, 0)
    frm1=cv2.resize(frm1, (int(frm1.shape[1]/kdif), int(frm1.shape[0]/kdif)))
    frm2=cv2.resize(frm2, (int(frm2.shape[1]/kdif), int(frm2.shape[0]/kdif)))
    pts1=cv2.goodFeaturesToTrack(frm1, 1000, 0.01, 30)
    pts2=cv2.goodFeaturesToTrack(frm2, 1000, 0.01, 30)
    nextPts, status, err = cv2.calcOpticalFlowPyrLK(frm1, frm2, pts1, pts2)
    # print status
    pts1Good=pts1[ status==1 ]
    # pts1Good=np.reshape(pts1Good, (pts1Good.shape[0],1,pts1Good.shape[1]))
    nextPtsG=nextPts[ status==1 ]
    # nextPtsG=np.reshape(nextPtsG, (nextPtsG.shape[0],1,nextPtsG.shape[1]))
    # T=cv2.estimateRigidTransform(pts1Good, nextPtsG, True)
    T,msk=cv2.findHomography(pts1Good, nextPtsG, cv2.RANSAC)
    print T
    if T==None:
        dxy=(0,0)
    else:
        dx,dy=T[0,2],T[1,2]
        dxy=(dx,dy)
    tmp=np.zeros((frm1.shape[0], frm1.shape[1], 3), np.uint8)
    frm2_shift=np.roll(frm2, int(math.floor(-dxy[0])), 1)
    frm2_shift=np.roll(frm2_shift, int(math.floor(-dxy[1])), 0)
    tmp[:,:,2]=frm1
    tmp[:,:,1]=frm2_shift
    tmp[:,:,0]=0
    cv2.imshow("frame #2 shift",  cv2.resize(tmp, (tmp.shape[1]/1, tmp.shape[0]/1)))
    return dxy

def calcMatchTemplate(fimg1, fimg2, parMethod=cv2.TM_CCORR_NORMED):
    ksiz=0.3
    frm1=cv2.imread(fimg1, 0)
    frm2=cv2.imread(fimg2, 0)
    frm1=cv2.resize(frm1, (int(frm1.shape[1]/kdif), int(frm1.shape[0]/kdif)))
    frm2=cv2.resize(frm2, (int(frm2.shape[1]/kdif), int(frm2.shape[0]/kdif)))
    fsiz=np.array((frm1.shape[1], frm1.shape[0]))
    tsiz=np.floor(fsiz*ksiz)
    p0=np.floor((fsiz-tsiz)/2)
    frm2p=frm2[p0[1]:p0[1]+tsiz[1], p0[0]:p0[0]+tsiz[0]].copy()
    CC=cv2.matchTemplate(frm1, frm2p, parMethod)
    minVal,maxVal,minLoc,maxLoc = cv2.minMaxLoc(CC)
    dxy=p0-maxLoc
    # print maxVal
    frm2_shift=np.roll(frm2, int(math.floor(-dxy[0])), 1)
    frm2_shift=np.roll(frm2_shift, int(math.floor(-dxy[1])), 0)
    tmp=np.zeros((frm1.shape[0], frm1.shape[1], 3), np.uint8)
    tmp[:,:,2]=frm1
    tmp[:,:,1]=frm2_shift
    tmp[:,:,0]=0
    # cv2.imshow("win-frm1", frm1)
    # cv2.imshow("win-prt2", frm2p)
    cv2.imshow("win-shift", tmp)
    # cv2.waitKey(0)
    # pl.imshow(CC)
    # pl.show()
    return dxy

#################################
if __name__=='__main__':
    numTrack=len(lst_imdir)
    # for ii in xrange(0,numTrack):
    for ii in xrange(1,2):
        fdir=lst_imdir[ii]
        print "%s" % fdir
        fcsv='%s/%s_pointers.csv' % (wdir, fdir)
        tdata=np.genfromtxt(fcsv, delimiter=',')
        numPts=len(tdata)
        for pp in xrange(1,numPts):
            # print tdata[pp,:]
            dxy=(tdata[pp,3:4+1]-tdata[pp,1:2+1])/kdif
            dr=np.sqrt(dxy[0]**2 + dxy[1]**2)
            fimg1="%s/%s/image-01-%03d.jpeg" % (wdir, fdir, pp+0)
            fimg2="%s/%s/image-01-%03d.jpeg" % (wdir, fdir, pp+1)
            dxyCalc=calcPhaseCorrShift(fimg1, fimg2)
            # dxyCalc=calcGFTTShift(fimg1, fimg2)
            # dxyCalc=calcMatchTemplate(fimg1, fimg2)
            drErr=np.array(dxyCalc)-dxy
            drErr=np.sqrt(drErr[0]**2 + drErr[1]**2)
            print "[%d] : dxyMeasured=%s, dxyCalc=%s, drErr=%f%% [%f (px)]" % (pp+1, dxy, dxyCalc, 100.*drErr/dr, drErr)
            # print "[%d] : dxyMeasured=%s, dxyCalc=%s, drErr=%fpx" % (pp+1, dxy, dxyCalc, drErr)
            while True:
                key = cv2.waitKey(0)
                if key==27:
                    break
        # print len(tdata)
        # pl.plot(tdata[:,0], tdata[:,1])
    # pl.show()

    # frm1=cv2.imread(fnfrm1, 0)
    # frm2=cv2.imread(fnfrm2, 0)
    # frm1=frm1.astype(np.float)
    # frm2=frm2.astype(np.float)
    # hann=cv2.createHanningWindow((frm1.shape[1], frm1.shape[0]), cv2.CV_64F)
    # hann2=cv2.createHanningWindow((100,100), cv2.CV_64F)
    # hann2_nrm=cv2.normalize(hann2, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    # cv2.imshow("FUCK", hann2_nrm)
    # shift = cv2.phaseCorrelate(frm1, frm2, hann)
    # dxy=shift
    # print shift
    # print xy_frm2-xy_frm1
    # hann_nrm=cv2.normalize(hann, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    # frm1_nrm=cv2.normalize(frm1, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    # frm2_nrm=cv2.normalize(frm2, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    # cv2.imshow("hanning",   hann_nrm)
    # cv2.imshow("frame #1",  frm1_nrm)
    # # cv2.imshow("frame #2",  frm2_nrm)
    # frm2_nrm_shift=np.roll(frm2_nrm, int(math.floor(-dxy[0])), 1)
    # frm2_nrm_shift=np.roll(frm2_nrm_shift, int(math.floor(-dxy[1])), 0)
    # tmp=np.zeros((frm1.shape[0], frm1.shape[1], 3), np.uint8)
    # tmp[:,:,2]=frm1_nrm
    # tmp[:,:,0]=frm2_nrm_shift
    # tmp[:,:,1]=0
    # cv2.imshow("frame #2 shift",  frm2_nrm_shift)
    # cv2.imshow("frame diff",  tmp)
    # while True:
    #     key = cv2.waitKey(0)
    #     if key==27:
    #         break
