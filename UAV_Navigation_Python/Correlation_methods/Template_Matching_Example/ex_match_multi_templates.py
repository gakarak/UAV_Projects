#!/usr/bin/python
__author__ = 'ar'

import numpy as np
import cv2
import os
import sys

fimg='/home/ar/img/shoots/USA_AIRBASE/sample_aircrafts_0.png'
lstTmp=(
    '/home/ar/img/shoots/USA_AIRBASE/sample_aircrafts_template_1.png',
    '/home/ar/img/shoots/USA_AIRBASE/sample_aircrafts_template_2.png',
    '/home/ar/img/shoots/USA_AIRBASE/sample_aircrafts_template_3.png')
lstColors=((0,255,0), (0,0,255), (0,255,255))
lstLbl=('type#1', 'type#2', 'type#3')
tmpSize=256

def img2Color(img):
    if len(img.shape)<3:
        return cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    else:
        return img.copy()

def prepareTemplate(img, siz, lblTxt, lblColor):
    img=img2Color(img)
    lblSiz=40
    sizNew=(siz, (img.shape[0]*siz)/img.shape[1])
    img=cv2.resize(img, sizNew)
    imgp=np.zeros((img.shape[0]+lblSiz, img.shape[1], img.shape[2]), np.uint8)
    imgp[:,:,0]=lblColor[0]
    imgp[:,:,1]=lblColor[1]
    imgp[:,:,2]=lblColor[2]
    cv2.rectangle(img, (2,2), (img.shape[1]-2, img.shape[0]-2), lblColor, 4)
    imgp[lblSiz:, :, :]=img
    cv2.putText(imgp, lblTxt, (10,lblSiz-2), cv2.FONT_HERSHEY_PLAIN, 1.4, (0,0,0), 2)
    return imgp


def nrmMat(mat):
    return cv2.normalize(mat, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)

def makeCenteredImg(img, newSiz):
    newImg=np.zeros(newSiz, np.uint8)
    p0=( (newSiz[0]-img.shape[0])/2, (newSiz[1]-img.shape[1])/2 )
    newImg[p0[0]:p0[0]+img.shape[0], p0[1]:p0[1]+img.shape[1]]=img.copy()
    return newImg

####################################
if __name__=='__main__':
    # img=cv2.imread(lstTmp[0], 0)
    # cv2.imshow("win", prepareTemplate(img, tmpSize, lstLbl[0], lstColors[0]))
    # cv2.waitKey(0)
    # sys.exit(0)

    img=cv2.imread(fimg, 0)
    CCtot=None
    lstBlbs=[]
    for ftmp in lstTmp:
        tmp=cv2.imread(ftmp, 0)
        CC=cv2.matchTemplate(img, tmp, cv2.TM_CCOEFF_NORMED)
        res=np.abs(CC)**3
        val, result = cv2.threshold(res, 0.01, 0, cv2.THRESH_TOZERO)
        CCnrm=makeCenteredImg(nrmMat(result), img.shape)
        if CCtot==None:
            CCtot=img2Color(CCnrm.copy())
        else:
            CCtot+=img2Color(CCnrm.copy())
        CCnrm[CCnrm<70]=0
        CCnrm[CCnrm>70]=255
        kernel = np.ones((5,5),np.uint8)
        CCnrm=cv2.dilate(CCnrm, kernel=kernel, iterations=1)
        params=cv2.SimpleBlobDetector_Params()
        params.minThreshold=100
        params.maxThreshold=255
        params.filterByCircularity = False
        params.filterByConvexity = False
        params.filterByInertia = False
        blbDet=cv2.SimpleBlobDetector(params)
        blbs=blbDet.detect(255-CCnrm)
        lstBlbs.append(blbs)
        # print blbs
    cnt=0
    tmpPreview=None
    timg=img2Color(img.copy())
    for ftmp in lstTmp:
        tmp=cv2.imread(ftmp, 0)
        for pp in lstBlbs[cnt]:
            tpc=(int(pp.pt[0]), int(pp.pt[1]))
            brd=(tmp.shape[1]/2, tmp.shape[0]/2)
            tp1=(tpc[0]-brd[0], tpc[1]-brd[1])
            tp2=(tpc[0]+brd[0], tpc[1]+brd[1])
            cv2.rectangle(timg, tp1, tp2, lstColors[cnt], 2 )
            cv2.putText(timg, "#%d" %(cnt+1), (tp2[0]+3, tp1[1]), cv2.FONT_HERSHEY_PLAIN, 1.0, lstColors[cnt])
            ccVal=np.float(np.max(CCtot[tpc[1]-5:tpc[1]+5, tpc[0]-5:tpc[0]+5, :]))/255.
            cv2.circle(CCtot, tpc, np.max(tmp.shape)/2, lstColors[cnt], 2)
            cv2.putText(CCtot, "%0.2f" % ccVal, (tpc[0]+10, tpc[1]-brd[1]-5), cv2.FONT_HERSHEY_PLAIN, 1.0, lstColors[cnt])
        tmpPrvt=prepareTemplate(tmp, tmpSize, lstLbl[cnt], lstColors[cnt])
        if tmpPreview==None:
            tmpPreview=tmpPrvt.copy()
        else:
            tmpPreview=np.concatenate((tmpPreview, tmpPrvt.copy()))
        cnt+=1
    cv2.imshow("Requests", tmpPreview)
    cv2.imshow("Detection Map", timg)
    cv2.imshow("Correlation Map", CCtot)
    cv2.waitKey(0)
