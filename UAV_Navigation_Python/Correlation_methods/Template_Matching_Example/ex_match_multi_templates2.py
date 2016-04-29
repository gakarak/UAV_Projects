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
lstColor=((0,0,255), (0,255,0), (0,255,255))
lstLbl=('ex#1', 'ex#2', 'ex#3')


def image2RGB(img):
    if len(img.shape)<3:
        return cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    else:
        return img.copy()

def makeLabelImage(img, imgSize, lblTxt, lblColor):
    lblSiz=40
    img=image2RGB(img)
    newSize=(imgSize, (imgSize*img.shape[1])/img.shape[0])
    img=cv2.resize(img, newSize)
    cv2.rectangle(img, (2,2), (img.shape[1]-2, img.shape[0]-2), lblColor, 4)
    imgp=np.zeros((img.shape[0]+lblSiz,img.shape[1], 3) , np.uint8)
    imgp[:,:,0]=lblColor[0]
    imgp[:,:,1]=lblColor[1]
    imgp[:,:,2]=lblColor[2]
    imgp[lblSiz:,:,:]=img
    cv2.putText(imgp, lblTxt, (10,lblSiz-5), cv2.FONT_HERSHEY_PLAIN, 1.8, (0,0,0) ,2)
    return imgp.copy()

def nrmMat(mat):
    return cv2.normalize(mat, None, 0,255, cv2.NORM_MINMAX, cv2.CV_8U)

def makeCenteredImage(img, newSize):
    imgn=np.zeros(newSize, np.uint8)
    p0=((newSize[0]-img.shape[0])/2, (newSize[1]-img.shape[1])/2)
    imgn[p0[0]:p0[0]+img.shape[0], p0[1]:p0[1]+img.shape[1]]=img
    return imgn

def drawLabels(img, lstPeaks, idx, bsiz, bColor):
    # print lstPeaks
    for ii in lstPeaks:
        pt=(int(ii.pt[0]),int(ii.pt[1]))
        bnd=(bsiz[1]/2, bsiz[0]/2)
        pt1=(pt[0]-bnd[0], pt[1]-bnd[1])
        pt2=(pt[0]+bnd[0], pt[1]+bnd[1])
        ptt=(pt[0]+bnd[0]+4, pt[1]-bnd[1]-1)
        cv2.rectangle(img, pt1, pt2, bColor, 2)
        cv2.putText(img, "#%d" % idx,  ptt, cv2.FONT_HERSHEY_PLAIN, 1.2, bColor, 2)

def drawCoorel(img, lstPeaks, lstQ, bsiz, bColor):
    # print lstPeaks
    cnt=0
    for ii in lstPeaks:
        pt=(int(ii.pt[0]),int(ii.pt[1]))
        bnd=(bsiz[1]/2, bsiz[0]/2)
        pt1=(pt[0]-bnd[0], pt[1]-bnd[1])
        pt2=(pt[0]+bnd[0], pt[1]+bnd[1])
        ptt=(pt[0]+bnd[0]+4, pt[1]-bnd[1]-1)
        cv2.circle(img, pt, max(bnd),  bColor, 2)
        cv2.putText(img, "#%0.2f" % lstQ[cnt],  ptt, cv2.FONT_HERSHEY_PLAIN, 1.2, bColor, 2)
        cnt+=1

imageTmpSize=200
###############################
if __name__=='__main__':
    tmpAll=None
    for ii in xrange(len(lstTmp)):
        ftmp=lstTmp[ii]
        tmp=cv2.imread(ftmp, 0)
        tmp=makeLabelImage(tmp, imageTmpSize, lstLbl[ii], lstColor[ii])
        if tmpAll==None:
            tmpAll=tmp.copy()
        else:
            tmpAll=np.concatenate((tmpAll, tmp.copy()))
    cv2.imshow("Request", tmpAll)
    img=cv2.imread(fimg, 0)
    #
    CCtot=None
    lstBlobs=[]
    lstQual=[]
    lstShapes=[]
    for ftmp in lstTmp:
        tmp=cv2.imread(ftmp, 0)
        lstShapes.append(tmp.shape)
        CC=cv2.matchTemplate(img, tmp, cv2.TM_CCOEFF_NORMED)
        res=np.abs(CC)**3
        val, result = cv2.threshold(res, 0.01, 0, cv2.THRESH_TOZERO)
        CCnrm=nrmMat(result)
        CCnrm=makeCenteredImage(CCnrm, img.shape)
        #
        if CCtot==None:
            CCtot=CCnrm.copy()
        else:
            CCtot+=CCnrm.copy()
        #
        CCnrm[CCnrm<80]=0
        CCnrm[CCnrm>80]=255
        kernel = np.ones((5,5),np.uint8)
        CCnrm=cv2.dilate(CCnrm, kernel=kernel, iterations=1)
        blbDet=cv2.SimpleBlobDetector()
        retBLB=blbDet.detect(255-CCnrm)
        lstQualTmp=[]
        for bb in retBLB:
            tp=(int(bb.pt[1]), (bb.pt[0]))
            tprt=CCtot[tp[0]-3:tp[0]+3, tp[1]-3:tp[1]+3]
            valCC=np.float(np.max(tprt)/255.)
            lstQualTmp.append(valCC)
        lstQual.append(lstQualTmp)
        lstBlobs.append(retBLB)
        print lstQual
    # cv2.imshow("CC", CCtot)
    imgc=image2RGB(img)
    CCtotC=image2RGB(CCtot)
    cnt=0
    for bb in lstBlobs:
        drawLabels(imgc, bb, cnt+1, lstShapes[cnt], lstColor[cnt])
        drawCoorel(CCtotC, bb, lstQual[cnt], lstShapes[cnt], lstColor[cnt])
        cnt+=1
    cv2.imshow("Detection-MAP", imgc)
    cv2.imshow("Correlation-MAP", CCtotC)
    while True:
        key=cv2.waitKey(0)
        if key==27:
            break