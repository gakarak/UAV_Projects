#!/usr/bin/python
__author__ = 'ar'

import libfp as fp
import libphcorr as ph

import cv2
import numpy as np
import os
import sys



fidx='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'

listDet=('BRISK', 'SURF', 'SIFT', 'ORB')
# listDet=('SIFT', 'ORB')

############################
def kp2arr(kp):
    ret=[]
    for tp in kp:
        tmp=(tp.pt[0], tp.pt[1], tp.size, tp.angle, tp.response, tp.octave, tp.class_id)
        ret.append(tmp)
    return np.array(ret)

def getBestKp(kp, numBest=100):
    if len(kp)<numBest:
        numBest=len(kp)
    arrkp=kp2arr(kp)
    sortIdx=np.argsort(-arrkp[:,4])
    ret=[]
    for ii in range(numBest):
        ret.append(kp[sortIdx[ii]])
    return ret

############################
if __name__=='__main__':
    #
    # fimg='/home/ar/img/lena.png'
    # img=cv2.imread(fimg, 0)
    # det1=cv2.SIFT()
    # det2=cv2.SURF()
    # kp1,kd1=det1.detectAndCompute(img,None)
    # kp2,kd2=det2.detectAndCompute(img,None)
    # arrkp1=kp2arr(kp1)
    # arrkp2=kp2arr(kp2)
    # print 'test'
    lstfn=np.recfromtxt(fidx)
    cnt=0
    for ff in lstfn:
        for tt in listDet:
            fkp='%s_%s.kp' % (ff, tt)
            fimgpOut='%s_%s_kp_best.png' % (ff,tt)
            imgc=cv2.imread(ff, 1)
            kp,kd=fp.file2kp(fkp)
            # arrkp=kp2arr(kp)
            kpBest=getBestKp(kp)
            # fp.plotFPoints(imgc, kpBest, color=(0,0,255))
            imgc=cv2.drawKeypoints(imgc, kpBest, flags=cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
            cv2.imshow("win-%s" % tt, imgc)
            # cv2.imwrite(fimgpOut, imgc)
            cv2.waitKey(0)
            print ff
        cnt+=1
        # if cnt>3:
        #     break
