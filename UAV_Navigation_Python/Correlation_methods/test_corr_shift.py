#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import math
import pylab as pl

fnfrm1='/home/ar/video/drone_project/2/frame_364_640x480.3.png'
fnfrm2='/home/ar/video/drone_project/2/frame_420_640x480.3.png'

# fnfrm1='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-013.jpeg'
# fnfrm2='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-014.jpeg'

xy_frm1=np.array([825, 618])
xy_frm2=np.array([1147, 675])

kdif=1.0
ksiz=0.3

def calcMatchTemplate(fimg1, fimg2, parMethod=cv2.TM_CCORR_NORMED):
    frm1=cv2.imread(fnfrm1, 0)
    frm2=cv2.imread(fnfrm2, 0)
    frm1=cv2.resize(frm1, (int(frm1.shape[1]/kdif), int(frm1.shape[0]/kdif)))
    frm2=cv2.resize(frm2, (int(frm2.shape[1]/kdif), int(frm2.shape[0]/kdif)))
    fsiz=np.array((frm1.shape[1], frm1.shape[0]))
    tsiz=np.floor(fsiz*ksiz)
    p0=np.floor((fsiz-tsiz)/2)
    frm2p=frm2[p0[1]:p0[1]+tsiz[1], p0[0]:p0[0]+tsiz[0]].copy()
    CC=cv2.matchTemplate(frm1, frm2p, cv2.TM_CCORR_NORMED)
    minVal,maxVal,minLoc,maxLoc = cv2.minMaxLoc(CC)
    dxy=p0-maxLoc
    # print maxVal
    # frm2_shift=np.roll(frm2, int(math.floor(-dxy[0])), 1)
    # frm2_shift=np.roll(frm2_shift, int(math.floor(-dxy[1])), 0)
    # tmp=np.zeros((frm1.shape[0], frm1.shape[1], 3), np.uint8)
    # tmp[:,:,2]=frm1
    # tmp[:,:,1]=frm2_shift
    # tmp[:,:,0]=0
    # cv2.imshow("win-frm1", frm1)
    # cv2.imshow("win-prt2", frm2p)
    # cv2.imshow("win-shift", tmp)
    # cv2.waitKey(0)
    # pl.imshow(CC)
    # pl.show()
    return dxy

if __name__=='__main__':
    dxy=calcMatchTemplate(fnfrm1, fnfrm2)
    # dxyReal=xy_frm2-xy_frm1
    print dxy
    # print dxyReal
