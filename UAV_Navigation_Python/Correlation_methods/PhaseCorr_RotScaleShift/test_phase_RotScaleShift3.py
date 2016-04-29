#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import matplotlib.pyplot as plt
import sys
import phcorr as ph

fimg1='/home/ar/video/drone_project/2/frame_364_640x480.3.png'
fimg2='/home/ar/video/drone_project/2/frame_420_640x480.3.png'
fimg3='/home/ar/video/drone_project/2/frame_533_640x480.3.png'


##################################################
if __name__=='__main__':
    img1=cv2.imread(fimg1, 0).astype(np.float)
    img2=cv2.imread(fimg2, 0).astype(np.float)
    # rotm=cv2.getRotationMatrix2D( (img1.shape[1]/2, img1.shape[0]/2), 15, 1.2)
    # img2=cv2.warpAffine(img1, rotm, img1.shape[::-1])
    # brd=np.array(img1.shape)/8
    # print brd
    # img2=cv2.resize(img1[brd[0]+5:-brd[0]+5, brd[1]+10:-brd[1]+10].copy(), img1.shape[::-1])

    # img1t=img1[50:50+200,50:50+200].copy()
    # img2t=img1[50+20:50+20+200,50+20:50+20+200].copy()
    # cmsk=ph.getCirlceMask(1.0, img1t.shape)
    # img1t=img1t#*cmsk
    # img2t=img2t*cmsk
    # CC,dxy,ccval=ph.phaseCorr(img1t, img2t, isDebug=False)
    # print dxy, ccval
    # cv2.imshow("win-imgs", ph.nrmMat(np.concatenate((img1t,img2t))))
    # cv2.imshow("win-CC", ph.nrmMat(CC))
    #
    #
    # cv2.imshow("win-msk", ph.nrmMat(cmsk))
    #
    #
    # cv2.waitKey(0)
    # sys.exit(0)


    img1=ph.getSquaredImg(img1)
    img2=ph.getSquaredImg(img2)

    img1 = ph.calcGradMag(img1)
    img2 = ph.calcGradMag(img2)

    # cv2.imshow("win-LogPolar", nrmMat(getLogPolar(img1)))

    # hw=getCylHanning(img1.shape, 1)
    # cv2.imshow("win0", nrmMat(img1))
    # cv2.imshow("win1", nrmMat(hw))
    # cv2.imshow("win2", nrmMat(img1*hw))
    # cv2.waitKey(0)
    # sys.exit(0)

    # img2=np.roll(np.roll(img1.copy(), 20,0), 30,1)
    siz1=img1.shape
    siz2=img2.shape
    assert (siz1==siz2)
    #
    # Shift-phase correlation
    img1F=np.abs(np.fft.fftshift(np.fft.fft2(img1)))
    img2F=np.abs(np.fft.fftshift(np.fft.fft2(img2)))
    img1Fs=ph.nrmMat(np.abs(np.log(img1F+1.0)))
    img2Fs=ph.nrmMat(np.abs(np.log(img2F+1.0)))
    cv2.imshow("win-img-12", np.dstack((img1,img2,img1)).astype(np.uint8) )
    # cv2.imshow("win-fft-12", np.dstack((img1Fs,img2Fs,img1Fs)) )
    CC,dxy,ccval=ph.phaseCorr(img1, img2, isDebug=False)
    print dxy, ccval
    # cv2.imshow("win-CC", nrmMat(CC) )
    img2Shift=np.roll(np.roll(img2,dxy[0],1), dxy[1], 0)
    # cv2.imshow("win-shift-12", np.dstack((nrmMat(img1),nrmMat(img2Shift),nrmMat(img1))) )
    #
    # Rotate-Scale
    img1FLP,MM=ph.getLogPolar(img1F.copy())
    img2FLP,MM=ph.getLogPolar(img2F.copy())
    # hwLP=getCylHanning(img1FLP.shape, axis=0)
    hwLP=cv2.createHanningWindow(img1FLP.shape[::-1], cv2.CV_64F)
    img1FLP=img1FLP*hwLP
    img2FLP=img2FLP*hwLP
    #
    img1FLPN=ph.nrmMat( np.abs(np.log(img1FLP+1.0)) )
    img2FLPN=ph.nrmMat( np.abs(np.log(img2FLP+1.0)) )
    # cv2.imshow("win-Hann-LP", nrmMat(hwLP) )
    cv2.imshow("win-fft-LP-img1", np.dstack((img1FLPN,img2FLPN,img1FLPN)) )
    CCRS,dxyRS,ccvalRS=ph.phaseCorr(img1FLP,img2FLP)
    #
    dAng=(360./CCRS.shape[0])*dxyRS[1]
    dScl=np.exp(float(dxyRS[0])/MM)
    print dxyRS, ccvalRS, ", dAng = ", dAng, ", dScale=", dScl
    #
    rotm2=cv2.getRotationMatrix2D((img2.shape[1]/2, img2.shape[0]/2), -dAng, 1./dScl)
    print rotm2
    img2RS=cv2.warpAffine(img2.copy(), rotm2, img2.shape)
    CCS,dxyS,ccvalS=ph.phaseCorr(img1, img2RS)
    print dxyS, ccvalS, CCS.shape
    #
    img2SRS=np.roll(np.roll(img2RS,dxyS[0],1), dxyS[1], 0)
    cv2.imshow("win-CC-ScaleRot",   ph.nrmMat(CCRS) )
    cv2.imshow("win-CC-Shift",      ph.nrmMat(CCS) )
    cv2.imshow("win-img2-ScaleRot", ph.nrmMat(img2RS) )
    cv2.imshow("win-img-12-SRS", np.dstack((img1,img2SRS,img1)).astype(np.uint8) )
    # cv2.imshow("win1", nrmMat(img1Fs))
    # cv2.imshow("win2", nrmMat(img2Fs))
    while True:
        key=cv2.waitKey(0)
        if key==27:
            break