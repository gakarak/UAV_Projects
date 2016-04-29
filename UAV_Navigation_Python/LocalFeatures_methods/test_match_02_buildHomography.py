#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import os
import sys

fvideo0='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/idx.csv'
fvideo1='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'

if __name__=='__main__':
    wdir=os.path.dirname(os.path.dirname(fvideo0))
    dir0=os.path.basename(os.path.dirname(fvideo0))
    dir1=os.path.basename(os.path.dirname(fvideo1))
    foutDstCorr="%s/%s_match_to_%s.csv" % (wdir,dir0,dir1)
    foutPts2Pts="%s_pts2pts.csv" % foutDstCorr
    dataP2P=np.genfromtxt(foutPts2Pts, delimiter=',')
    H,Scores=cv2.findHomography(dataP2P[:,0:2].copy(), dataP2P[:,2:].copy())
    #
    fcsvTrj0='%s_trjfp.csv' % fvideo0
    fcsvTrj1='%s_trjfp.csv' % fvideo1
    fcsvTrj1to0='%s_trjfp_1to0.csv' % fvideo1
    trj0=np.genfromtxt(fcsvTrj0, delimiter=',')
    trj1=np.genfromtxt(fcsvTrj1, delimiter=',')
    #
    trj1to0=trj1.copy()
    numPts1=trj1.shape[0]
    for ii in xrange(numPts1):
        # XY:
        tmpXY=np.array((trj1[ii,0], trj1[ii,1], 1))
        tmpXY2=np.dot(H,tmpXY)
        tmpXY2=tmpXY2/tmpXY2[2]
        trj1to0[ii,:2]=tmpXY2[:2]
        # Angle:
        tang=np.deg2rad(trj1[ii,2])
        nxy=np.array( (np.cos(tang),np.sin(tang), 1) )
        pxy0=np.array( (trj1[ii,0], trj1[ii,1], 1) )
        pxy1=pxy0+nxy
        pxy02=np.dot(H,pxy0)
        pxy12=np.dot(H,pxy1)
        pxy02=pxy02/pxy02[2]
        pxy12=pxy12/pxy12[2]
        nxy2=pxy12-pxy02
        nxy2=nxy2/np.linalg.norm(nxy2)
        # nxy2=np.dot(H,nxy)
        # nxy2=nxy2/nxy2[2]
        tang2=np.rad2deg(np.arctan2(nxy2[1],nxy2[0]))
        trj1to0[ii,2]=tang2
    np.savetxt(fcsvTrj1to0, trj1to0, delimiter=',')
