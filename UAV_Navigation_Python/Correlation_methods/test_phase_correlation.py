#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import math


fnfrm1='/home/ar/video/drone_project/2/frame_364_640x480.3.png'
fnfrm2='/home/ar/video/drone_project/2/frame_420_640x480.3.png'

# fnfrm1='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-013.jpeg'
# fnfrm2='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-014.jpeg'

xy_frm1=np.array([825, 618])
xy_frm2=np.array([1147, 675])

if __name__=='__main__':
    frm1=cv2.imread(fnfrm1, 0)
    frm2=cv2.imread(fnfrm2, 0)
    frm1=frm1.astype(np.float)
    frm2=frm2.astype(np.float)
    hann=cv2.createHanningWindow((frm1.shape[1], frm1.shape[0]), cv2.CV_64F)
    hann2=cv2.createHanningWindow((100,100), cv2.CV_64F)
    hann2_nrm=cv2.normalize(hann2, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    cv2.imshow("FUCK", hann2_nrm)
    shift = cv2.phaseCorrelate(frm1, frm2, hann)
    dxy=shift[0]
    print shift
    print xy_frm2-xy_frm1
    hann_nrm=cv2.normalize(hann, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    frm1_nrm=cv2.normalize(frm1, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    frm2_nrm=cv2.normalize(frm2, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)
    cv2.imshow("hanning",   hann_nrm)
    cv2.imshow("frame #1",  frm1_nrm)
    # cv2.imshow("frame #2",  frm2_nrm)
    frm2_nrm_shift=np.roll(frm2_nrm, int(math.floor(-dxy[0])), 1)
    frm2_nrm_shift=np.roll(frm2_nrm_shift, int(math.floor(-dxy[1])), 0)
    tmp=np.zeros((frm1.shape[0], frm1.shape[1], 3), np.uint8)
    tmp[:,:,2]=frm1_nrm
    tmp[:,:,0]=frm2_nrm_shift
    tmp[:,:,1]=0
    cv2.imshow("frame #2 shift",  frm2_nrm_shift)
    cv2.imshow("frame diff",  tmp)
    while True:
        key = cv2.waitKey(0)
        if key==27:
            break
