#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import pickle as pik


fimg='/home/ar/img/doge.jpg'
fdat='%s.kpdat' % fimg


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

def pickle_keypoints(kp, kd):
    assert len(kp)==len(kd)
    tmp=[]
    for ii in xrange(len(kp)):
        tp=kp[ii]
        td=kd[ii]
        tvar = (tp.pt, tp.size, tp.angle, tp.response, tp.octave, tp.class_id, td)
        tmp.append(tvar)
    return tmp

def unpickle_keypoints(arrpik):
    kp = []
    kd = []
    for ii in arrpik:
        tp=cv2.KeyPoint(ii[0][0], ii[0][1], ii[1], ii[2], ii[3], ii[4], ii[5])
        td=ii[6]
        kp.append(tp)
        kd.append(td)
    return (kp,kd)


def kp2file(fout, kp, kd):
    with open(fout,'wb') as f:
        dat=pickle_keypoints(kp,kd)
        pik.dump(dat, f)

def file2kp(fout):
    with open(fout,'rb') as f:
        dat=pik.load(f)
        return unpickle_keypoints(dat)

if __name__=='__main__':
    st=SimpleTimer()
    st.tic()
    img=cv2.imread(fimg, 0)
    det=cv2.SURF()
    kp,kd=det.detectAndCompute(img, None)
    st.toc()
    print "#pk = %d" % len(kp)
    # for ik,id in zip(kp,kd):
    #     print ik, id

    st.tic()
    kp2file(fdat, kp, kd)
    st.toc()
    st.tic()
    kpn,kdn=file2kp(fdat)
    st.toc()
    print "#pk = %d" % len(kpn)

