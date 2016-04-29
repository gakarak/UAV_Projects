#!/usr/bin/python
__author__ = 'ar'


import cv2
import numpy as np
import math
import pylab as pl

fimg='/home/ar/img/lena.png'
# fimg='/home/ar/img/Falling_Down_DVD.jpg'
# fimg='/home/ar/video/drone_project/2/frame_364_640x480.3.png'


if __name__=='__main__':
    img=cv2.imread(fimg, 1)
    # imgLP=cv2.cv.LogPolar(img,(img.shape[1]/2, img.shape[0]/2), 40, cv2.WARP_FILL_OUTLIERS)
    imgLP=cv2.cv.fromarray(img.copy())
    siz=np.array(img.shape, dtype=np.int32)
    p0=siz/2
    sizMin=np.min(p0[0:2])
    MM=2*sizMin/np.log(sizMin)
    cv2.cv.LogPolar(cv2.cv.fromarray(img),imgLP,(img.shape[1]/2, img.shape[0]/2), MM, cv2.cv.CV_WARP_FILL_OUTLIERS)
    cv2.line(img, (0, p0[0]), (siz[1],p0[0]), (255,255,255), 2)
    cv2.line(img, (p0[1], 0), (p0[1],siz[0]), (255,255,255), 2)
    imgLPM=np.asarray(imgLP)
    sizLPM=np.array(imgLPM.shape, dtype=np.int32)
    #
    p0LPM=siz/2
    cropPTS=int(MM*np.log(sizMin/5.))
    print cropPTS
    cv2.line(imgLPM, (0, p0LPM[0]), (sizLPM[1],p0LPM[0]), (255,255,255), 2)
    cv2.line(imgLPM, (p0LPM[1], 0), (p0LPM[1],sizLPM[0]), (255,255,255), 2)
    imgLPCrop=imgLPM[:,cropPTS:]
    cv2.imshow("win-Original", img)
    cv2.imshow("win-LogPolar", imgLPM)
    cv2.imshow("win-LogPolar-Crop", imgLPCrop)
    cv2.waitKey(0)