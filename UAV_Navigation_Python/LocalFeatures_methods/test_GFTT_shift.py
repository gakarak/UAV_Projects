#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np

# fnfrm1='/home/ar/video/drone_project/2/frame_364_640x480.3.png'
# fnfrm2='/home/ar/video/drone_project/2/frame_420_640x480.3.png'

fnfrm1='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-013.jpeg'
fnfrm2='/home/ar/data/UAV_Drone_Project/VID_20150324_012829/image-01-014.jpeg'


if __name__=='__main__':
    frm1=cv2.imread(fnfrm1, 0)
    frm2=cv2.imread(fnfrm2, 0)
    frm2c=cv2.imread(fnfrm2, 1)
    pts1=cv2.goodFeaturesToTrack(frm1, 200, 0.01, 300)
    pts2=cv2.goodFeaturesToTrack(frm2, 200, 0.01, 300)
    nextPts, status, err = cv2.calcOpticalFlowPyrLK(frm1, frm2, pts1, pts2)
    numPts=len(nextPts)
    nextPtsGood=np.array([], pts2.dtype)
    for ii in xrange(0,numPts):
        tpts1=pts1[ii][0]
        tpts2=nextPts[ii][0]
        # if status[ii][0]==1:
        if (status[ii]==1) and (err[ii]<20):
            cv2.circle(frm2c, tuple(tpts1), 3, (255,0,0))
            cv2.line(frm2c, tuple(tpts1), tuple(tpts2), (255,255,0))
            cv2.circle(frm2c, tuple(tpts2), 3, (0,0,255))
    # pts1Good=pts1[ ((status==1) & (err<20))]
    pts1Good=pts1[ status==1 ]
    pts1Good=np.reshape(pts1Good, (pts1Good.shape[0],1,pts1Good.shape[1]))
    # nextPtsG=nextPts[((status==1) & (err<20))]
    nextPtsG=nextPts[ status==1 ]
    nextPtsG=np.reshape(nextPtsG, (nextPtsG.shape[0],1,nextPtsG.shape[1]))
    # print pts1Good.shape
    # print nextPtsG.shape
    T=cv2.estimateRigidTransform(pts1Good, nextPtsG, False)
    dx,dy=T[0,2],T[1,2]
    dxy=np.array([dx,dy])
    pCenter=np.array([frm2c.shape[1]/2, frm2c.shape[0]/2])
    dr=np.sqrt(dx**2+dy**2)
    cv2.line(frm2c, (pCenter[0],pCenter[1]), (int(pCenter[0]+dxy[0]),int(pCenter[1]+dxy[1])), (0,255,0))
    cv2.circle(frm2c, (pCenter[0],pCenter[1]), 5, (0,255,0))
    cv2.circle(frm2c, (pCenter[0],pCenter[1]), int(dr), (0,255,0))
    cv2.circle(frm2c, (int(pCenter[0]+dxy[0]),int(pCenter[1]+dxy[1])), 5, (0,0,255))

    print T
    print "dxy=(%s, %s)" % (T[0,2], T[1,2])
    cv2.imshow("win2", frm2c)
    while True:
        key = cv2.waitKey(0)
        if key==27:
            break
