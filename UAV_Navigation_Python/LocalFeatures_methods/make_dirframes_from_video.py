#!/usr/bin/python
_author__ = 'ar'

import libfp as fp
import libphcorr as ph

import cv2
import numpy as np
import os
import sys
import shutil

# fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4'
# fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4'
# fvideo='/home/ar/video/UAV_PHONE/part_2/vid1_2to1.mp4'
# fvideo='/home/ar/video/UAV_PHONE/part_2/vid1_1to2.mp4'

# fvideo='/home/ar/video/cam_video/VID_20150402_104326_crop1.mp4'
fvideo='/home/ar/video/cam_video/VID_20150402_104326_crop2.mp4'

parStep=3
parSkip=30
parMaxSize=768

#########################
if __name__=='__main__':
    cap=cv2.VideoCapture(fvideo)
    numFrames=int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT))
    if numFrames<5:
        print '**ERROR** incorrect file [%s]' % fvideo
        sys.exit(1)
    wdirout='%s_proc' % fvideo
    if os.path.isdir(wdirout):
        print "***STOP*** directory exist [%s]" % wdirout
        shutil.rmtree(wdirout)
        # sys.exit(0)
    os.mkdir(wdirout)
    #
    foutIdx='%s/idx.csv' % wdirout
    for ii in xrange(parSkip):
        ret,frm=cap.read(ii)
    #
    f=open(foutIdx, 'w')
    for ii in range(parSkip,numFrames,parStep):
        ret,frm=cap.read(ii)
        if ret:
            if len(frm.shape)>2:
                frm=cv2.cvtColor(frm, cv2.COLOR_BGR2GRAY)
            frm=ph.resizeToMaxSize(frm, parMaxSize)
            frmName='frm_%05d.png' % ii
            tpath='%s/%s' % (wdirout, frmName)
            cv2.imwrite(tpath, frm)
            f.write('%s\n' % tpath)
        else:
            print 'ERROR, cant read frame #' % ii
            sys.exit(1)
        print "%d/%d" % (ii,numFrames)
    f.close()

