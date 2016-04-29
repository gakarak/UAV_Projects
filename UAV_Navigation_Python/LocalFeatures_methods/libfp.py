#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import os
import sys
import pickle as pik
import multiprocessing as mp

###############################
def getDetByName(detName):
    if detName=='SURF':
        return cv2.SURF()
    if detName=='SIFT':
        return cv2.SIFT()
    if detName=='BRISK':
        return cv2.BRISK()
    if detName=='ORB':
        return cv2.ORB(nfeatures=1400)
    return None

###############################
def task_proc_CalcKPF(data):
    fimg = data[0]
    detName = data[1]
    if os.path.isfile(fimg):
        det=getDetByName(detName)
        if det!=None:
            img=cv2.imread(fimg,0)
            kp,kd=det.detectAndCompute(img, None)
            imgc=cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
            plotFPoints(imgc, kp, rad=3)
            fkpOut='%s_%s.kp' % (fimg, detName)
            fimgOut='%s_%s_kp.png' % (fimg, detName)
            cv2.imwrite(fimgOut, imgc)
            kp2file(fkpOut, kp,kd)
            print "process --> %s : %s" % (detName, fkpOut)

class TaskManager():
    def __init__(self, nproc=4):
        self.nProc=nproc
        self.pool=mp.Pool(processes=self.nProc)
    def appendTaskCalcKPF(self, fimg, detName):
        vdata=[fimg, detName]
        self.pool.apply_async(task_proc_CalcKPF, [vdata])

###############################
def plotFPoints(img,fps, color=(0,255,0), rad=-1):
    for pp in fps:
        trad=int(pp.size)
        if rad>0:
            trad=rad
        cv2.circle(img, (int(pp.pt[0]), int(pp.pt[1])), trad, color)

###############################
class SimpleTimer():
    def __init__(self, isPrintToc=True):
        self.tlast=0.0
    def tic(self):
        self.tlast=cv2.getTickCount()/cv2.getTickFrequency()
    def toc(self):
        tnew=cv2.getTickCount()/cv2.getTickFrequency()
        dt=tnew-self.tlast
        print "dt=%0.5f" % dt
        return dt

###############################
def pik_keypoints(kp, kd):
    assert len(kp)==len(kd)
    tmp=[]
    for ii in xrange(len(kp)):
        tp=kp[ii]
        td=kd[ii]
        tvar = (tp.pt, tp.size, tp.angle, tp.response, tp.octave, tp.class_id, td)
        tmp.append(tvar)
    return tmp

def unpik_keypoints(arrpik):
    kp = []
    kd = []
    for ii in arrpik:
        tp=cv2.KeyPoint(ii[0][0], ii[0][1], ii[1], ii[2], ii[3], ii[4], ii[5])
        td=ii[6]
        kp.append(tp)
        kd.append(td)
    return (kp,np.array(kd))

def kp2file(fout, kp, kd):
    with open(fout,'wb') as f:
        dat=pik_keypoints(kp,kd)
        pik.dump(dat, f)

def file2kp(fout):
    with open(fout,'rb') as f:
        dat=pik.load(f)
        return unpik_keypoints(dat)


###############################
if __name__=='__main__':
    pass
