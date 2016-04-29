#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import matplotlib.pyplot as plt
import sys

fimg1='/home/ar/video/drone_project/2/frame_364_640x480.3.png'
fimg2='/home/ar/video/drone_project/2/frame_420_640x480.3.png'
fimg3='/home/ar/video/drone_project/2/frame_533_640x480.3.png'

def nrmMat(mat):
    return cv2.normalize(mat, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)

def phaseCorr(img1, img2, isUseHann=True, isDebug=False):
    assert(img1.shape==img2.shape)
    if isUseHann:
        hw=cv2.createHanningWindow(img1.shape[::-1], cv2.CV_64F)
        img1=img1*hw
        img2=img2*hw
    if isDebug:
        cv2.imshow("debug-img12", np.concatenate((nrmMat(img1), nrmMat(img2))))
    fft1=np.fft.fft2(img1)
    fft2=np.fft.fft2(img2)
    CC=np.fft.fftshift(np.fft.ifft2( (fft1*fft2.conj())/(fft2*fft2.conj()) ).real)
    minVal, maxVal, minLoc, maxLoc=cv2.minMaxLoc(CC)
    dxy=(maxLoc - np.array(img1.shape[::-1])/2)
    return (CC, dxy, maxVal)

"""
axis '0' -> y
axis '1' -> x
"""
def getCylHanning(size, axis=0, ptype=cv2.CV_64F):
    tmp=cv2.createHanningWindow(size[::-1], ptype)
    if axis==0:
        tmp=tmp[size[1]/2,:]
        print tmp.shape
        ret=np.tile(tmp,(size[0],1))
    else:
        tmp=tmp[:, size[0]/2]
        print tmp.shape
        ret=np.transpose(np.tile(tmp,(size[1],1)))
        # print ret.shape
    return ret

def calcGradMag(img):
    gx=cv2.Sobel(img, cv2.CV_64F, 1,0)
    gy=cv2.Sobel(img, cv2.CV_64F, 1,0)
    gm=np.sqrt(gx*gx+gy*gy)
    # cv2.imshow("img-gm", nrmMat(gm))
    return gm

def getLogPolar(img):
    imgLP=cv2.cv.fromarray(img.copy())
    siz=np.array(img.shape, dtype=np.int32)
    p0=siz/2
    sizMin=np.min(p0[0:2])
    MM=2*sizMin/np.log(sizMin)
    cv2.cv.LogPolar(cv2.cv.fromarray(img),imgLP,(img.shape[1]/2, img.shape[0]/2), MM, cv2.cv.CV_WARP_FILL_OUTLIERS)
    cv2.line(img, (0, p0[0]), (siz[1],p0[0]), (255,255,255), 2)
    cv2.line(img, (p0[1], 0), (p0[1],siz[0]), (255,255,255), 2)
    imgLPM=np.asarray(imgLP)
    # sizLPM=np.array(imgLPM.shape, dtype=np.int32)
    # p0LPM=siz/2
    cropPTS1=int(MM*np.log(1.*sizMin/4.))
    cropPTS2=int(MM*np.log(3.*sizMin/4.))
    imgLPCrop=imgLPM[:,cropPTS1:cropPTS2].copy()
    return (imgLPCrop,MM)

def getSquaredImg(img):
    if img.shape[0]==img.shape[1]:
        return img.copy()
    else:
        siz=img.shape[0:2]
        if siz[0]>siz[1]:
            dy=(siz[0]-siz[1])/2
            return img[dy:dy+siz[1],:].copy()
        else:
            dx=(siz[1]-siz[0])/2
            return img[:,dx:dx+siz[0]].copy()



fimg1='/home/ar/img/lena.png'
##################################################
if __name__=='__main__':
    img1=cv2.imread(fimg1, 0).astype(np.float)
    # img2=cv2.imread(fimg2, 0).astype(np.float)
    rotm=cv2.getRotationMatrix2D( (img1.shape[1]/2, img1.shape[0]/2), 0, 1.4)
    img2=cv2.warpAffine(img1, rotm, img1.shape[::-1])
    # brd=np.array(img1.shape)/8
    # print brd
    # img2=cv2.resize(img1[brd[0]+5:-brd[0]+5, brd[1]+10:-brd[1]+10].copy(), img1.shape[::-1])

    img1=getSquaredImg(img1)
    img2=getSquaredImg(img2)

    # img1 = calcGradMag(img1)
    # img2 = calcGradMag(img2)

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
    img1Fs=nrmMat(np.abs(np.log(img1F+1.0)))
    img2Fs=nrmMat(np.abs(np.log(img2F+1.0)))
    cv2.imshow("win-img-12", np.dstack((img1,img2,img1)).astype(np.uint8) )
    # cv2.imshow("win-fft-12", np.dstack((img1Fs,img2Fs,img1Fs)) )
    CC,dxy,ccval=phaseCorr(img1, img2, isDebug=False)
    print dxy, ccval
    # cv2.imshow("win-CC", nrmMat(CC) )
    img2Shift=np.roll(np.roll(img2,dxy[0],1), dxy[1], 0)
    # cv2.imshow("win-shift-12", np.dstack((nrmMat(img1),nrmMat(img2Shift),nrmMat(img1))) )
    #
    # Rotate-Scale
    img1FLP,MM=getLogPolar(img1F.copy())
    img2FLP,MM=getLogPolar(img2F.copy())
    hwLP=getCylHanning(img1FLP.shape, axis=0)
    # hwLP=cv2.createHanningWindow(img1FLP.shape[::-1], cv2.CV_64F)
    img1FLP=img1FLP*hwLP
    img2FLP=img2FLP*hwLP
    #
    img1FLPN=nrmMat( np.abs(np.log(img1FLP+1.0)) )
    img2FLPN=nrmMat( np.abs(np.log(img2FLP+1.0)) )
    # cv2.imshow("win-Hann-LP", nrmMat(hwLP) )
    cv2.imshow("win-fft-LP-img1", np.dstack((img1FLPN,img2FLPN,img1FLPN)) )
    CCRS,dxyRS,ccvalRS=phaseCorr(img1FLP,img2FLP)
    #
    dAng=(360./CCRS.shape[0])*dxyRS[1]
    dScl=np.exp(float(dxyRS[0])/MM)
    print dxyRS, ccvalRS, ", dAng = ", dAng, ", dScale=", dScl
    #
    rotm2=cv2.getRotationMatrix2D((img2.shape[1]/2, img2.shape[0]/2), -dAng, 1./dScl)
    print rotm2
    img2RS=cv2.warpAffine(img2.copy(), rotm2, img2.shape)
    CCS,dxyS,ccvalS=phaseCorr(img1, img2RS)
    print dxyS, ccvalS
    #
    img2SRS=np.roll(np.roll(img2RS,dxyS[0],1), dxyS[1], 0)
    cv2.imshow("win-CC-ScaleRot",   nrmMat(CCRS) )
    cv2.imshow("win-CC-Shift",      nrmMat(CCS) )
    cv2.imshow("win-img2-ScaleRot", nrmMat(img2RS) )
    cv2.imshow("win-img-12-SRS", np.dstack((img1,img2SRS,img1)).astype(np.uint8) )
    # cv2.imshow("win1", nrmMat(img1Fs))
    # cv2.imshow("win2", nrmMat(img2Fs))
    while True:
        key=cv2.waitKey(0)
        if key==27:
            break