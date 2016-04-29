#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np

if __name__=='__main__':
    lp1=np.random.rand(20,2)
    lp2=lp1.copy()*2.0
    H,status = cv2.findHomography(lp1,lp2,cv2.RANSAC, 5.0)
    # Ha=cv2.getAffineTransform(lp1,lp2)
    print H
    # print Ha
    print "good=%0.2f%%" % (100*np.sum(status)/len(status))
