#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import libfp as fp
import libphcorr as ph


fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'

listDet=('BRISK', 'SURF', 'SIFT', 'ORB')

if __name__=='__main__':
    csvReader=ph.VideoCSVReader(fvideo)
    csvReader.printInfo()
    numf=csvReader.getNumFrames()
    algName='SURF'
    for ii in xrange(30,numf-1):

        print "%d/%d" % (ii, numf)
