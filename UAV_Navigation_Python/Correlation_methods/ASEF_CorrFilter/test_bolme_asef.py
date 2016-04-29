#!/usr/bin/python
__author__ = 'ar'

import cv
import numpy as np
import struct
import array
import matplotlib.pyplot as plt

def loadFilterEyeLocator2(filename):
    f = open(filename,'rb')
    line = f.readline().strip()
    assert line == "CFEL"
    print f.readline(), f.readline()
    # get the width and the height
    r,c = f.readline().split()
    r,c = int(r),int(c)
    print "ROWxCOL = %dx%d" % (r,c)
    x,y,w,h = f.readline().split()
    left_rect = (int(x),int(y),int(w),int(h))
    # read in the right bounding rectangle
    x,y,w,h = f.readline().split()
    right_rect = (int(x),int(y),int(w),int(h))
    print "left-bb = %s, right-bb = %s" % (left_rect, right_rect)
    # read the magic number
    magic_number = f.readline().strip()
    assert len(magic_number) == 4
    magic_number = struct.unpack('i',magic_number)[0]
    lf = array.array('f')
    rf = array.array('f')
    lf.fromfile(f,r*c)
    rf.fromfile(f,r*c)
    if magic_number == 0x41424344:
        pass
    elif magic_number == 0x44434241:
        lf.byteswap()
        rf.byteswap()
    else:
        raise ValueError("Bad Magic Number: Unknown byte ordering in file")
    lFlt=np.array(lf)
    rFlt=np.array(rf)
    print "lFlt = %s, rFlt = %s" % (lFlt.shape, rFlt.shape)
    f.close()
    lFltR=np.reshape(lFlt, (r,c))
    rFltR=np.reshape(rFlt, (r,c))
    plt.figure(1)
    plt.imshow(np.fft.fftshift(np.log(lFltR+1)))
    plt.figure(2)
    plt.imshow(np.fft.fftshift(np.log(rFltR+1)))
    plt.show()


def loadFilterEyeLocator(filename,ilog=None):
    '''
    Loads the eye locator from a file.'
    '''

    # open the file
    f = open(filename,'rb')

    # Check the first line
    line = f.readline().strip()
    assert line == "CFEL"

    # read past the comment and copyright.
    f.readline()
    f.readline()

    # get the width and the height
    r,c = f.readline().split()
    r,c = int(r),int(c)

    # read in the left bounding rectangle
    x,y,w,h = f.readline().split()
    left_rect = (int(x),int(y),int(w),int(h))

    # read in the right bounding rectangle
    x,y,w,h = f.readline().split()
    right_rect = (int(x),int(y),int(w),int(h))

    # read the magic number
    magic_number = f.readline().strip()
    assert len(magic_number) == 4
    magic_number = struct.unpack('i',magic_number)[0]

    # Read in the filter data
    lf = array.array('f')
    rf = array.array('f')

    lf.fromfile(f,r*c)
    rf.fromfile(f,r*c)

    # Test the magic number and byteswap if necessary.
    if magic_number == 0x41424344:
        pass
    elif magic_number == 0x44434241:
        lf.byteswap()
        rf.byteswap()
    else:
        raise ValueError("Bad Magic Number: Unknown byte ordering in file")

    # Create the left and right filters
    left_filter  = cv.CreateMat(r,c,cv.CV_32F)
    right_filter = cv.CreateMat(r,c,cv.CV_32F)

    # Copy data into the left and right filters
    cv.SetData(left_filter, lf.tostring())
    cv.SetData(right_filter, rf.tostring())

    # tmp = pv.OpenCVToNumpy(left_filter)
    # t1 = tmp.mean()
    # t2 = tmp.std()
    # cv.Scale(left_filter,left_filter,1.0/t2,-t1*1.0/t2)
    #
    # tmp = pv.OpenCVToNumpy(right_filter)
    # t1 = tmp.mean()
    # t2 = tmp.std()
    # cv.Scale(right_filter,right_filter,1.0/t2,-t1*1.0/t2)
    #
    # #tmp = pv.OpenCVToNumpy(left_filter)
    # #print tmp.mean(),tmp.std()
    #
    # if ilog != None:
    #     #lf = cv.cvCreateMat(r,c,cv.CV_8U)
    #     #rf = cv.cvCreateMat(r,c,cv.CV_8U)
    #
    #     lf = pv.OpenCVToNumpy(left_filter)
    #     rf = pv.OpenCVToNumpy(right_filter)
    #
    #     lf = np.fft.fftshift(lf).transpose()
    #     rf = np.fft.fftshift(rf).transpose()
    #
    #     ilog.log(pv.Image(lf),label="LeftEyeFilter")
    #     ilog.log(pv.Image(rf),label="RightEyeFilter")
    #
    # # Return the eye locator
    # return OpenCVFilterEyeLocator(left_filter,right_filter,left_rect,right_rect)
    return 1

if __name__=='__main__':
    print 'test'
    loadFilterEyeLocator2('./EyeLocatorASEF128x128.fel')