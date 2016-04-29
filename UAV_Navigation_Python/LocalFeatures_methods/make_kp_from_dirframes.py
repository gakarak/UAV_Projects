#!/usr/bin/python
__author__ = 'ar'

import libfp as fp
import libphcorr as ph

import cv2
import numpy as np
import os
import sys

import multiprocessing as mp
try:
    import affinity
except:
    pass


# fidx='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'
# fidx='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_proc/idx.csv'
# fidx='/home/ar/video/data_test_frames_rot_scl_0/idx.csv'

# fidx='/home/ar/video/UAV_PHONE/part_2/vid1_1to2.mp4_proc/idx.csv'
# fidx='/home/ar/video/UAV_PHONE/part_2/vid1_2to1.mp4_proc/idx.csv'

# fidx='/home/ar/video/UAV_PHONE/part_2/vid1_1to2.mp4_proc2/idx.csv'
# fidx='/home/ar/video/UAV_PHONE/part_2/vid1_2to1.mp4_proc2/idx.csv'
# fidx='/media/big4/big.data/shared.vmware/git-bitbucket.org/build-FlightSimulator-Qt_5_4_1_x64-Debug/datalog_22.05.2015_18-28/screenfg_good/idx.csv'

# fidx='/home/ar/video/cam_video/VID_20150402_104326_crop1.mp4_proc/idx.csv'
# fidx='/home/ar/video/cam_video/VID_20150402_104326_crop2.mp4_proc/idx.csv'

# fidx='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/idx.csv'

# fidx='/home/ar/video/Video_Murashko/data_1fps_resize/idx.csv'
# fidx='/home/ar/video/UAV_PHONE/DJI/DJI_0005_MOV_CROP_StonePark/frames-crop1/idx.csv'
# fidx='/home/ar/video/UAV_PHONE/DJI/DJI_0005_MOV_CROP_StonePark/frames-crop2/idx.csv'
fidx='/home/ar/video/UAV_PHONE/DJI/DJI_0005_MOV_CROP_StonePark/frames-crop3/idx.csv'
# fidx='/home/ar/video/UAV_PHONE/DJI/DJI_0005_MOV_CROP_StonePark/frames-crop4/idx.csv'


# listDet=('BRISK', 'SURF', 'SIFT', 'ORB')
# listDet=('SURF', 'ORB')
listDet=['SURF']


def proc_seq():
    cap=ph.VideoCSVReader(fidx)
    cap.printInfo()
    for tt in listDet:
        print tt
        det=fp.getDetByName(tt)
        if det==None:
            print 'ERROR: incorrect detector [%s], exit...' % tt
            sys.exit(1)
        ret=True
        cap.resetPos()
        while ret:
            ret, frm, fname=cap.readWithName()
            if not ret:
                continue
            kp,kd=det.detectAndCompute(frm, None)
            frmc=cv2.cvtColor(frm.copy(), cv2.COLOR_GRAY2BGR)
            fp.plotFPoints(frmc, kp, rad=3)
            fkp='%s_%s.kp' % (fname, tt)
            fimgOut='%s_%s_kp.png' % (fname, tt)
            cv2.imwrite(fimgOut, frmc)
            fp.kp2file(fkp, kp,kd)
            print "--> %s : %s" % (tt, fkp)


#############################
if __name__=='__main__':
    #
    try:
        print affinity.get_process_affinity_mask(0)
        affinity.set_process_affinity_mask(0,2**mp.cpu_count()-1)
    except:
        pass
    #
    cap=ph.VideoCSVReader(fidx)
    cap.printInfo()
    taskManager=fp.TaskManager()
    for tt in listDet:
        print tt
        det=fp.getDetByName(tt)
        if det==None:
            print 'ERROR: incorrect detector [%s], exit...' % tt
            sys.exit(1)
        ret=True
        numFrames=cap.getNumFrames()
        for ii in xrange(numFrames):
            fname=cap.listFImg[ii]
            print 'Append task {%s}-[%s]' % (tt, fname)
            taskManager.appendTaskCalcKPF(fname, tt)
    taskManager.pool.close()
    taskManager.pool.join()
