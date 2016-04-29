#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import numpy.matlib
import sys


sys.path.append('..')

import libphcorr as ph


# fimg0='/home/ar/video/data_vgg_affine/viewpoint1_graf/img1.ppm'
# fimg1='/home/ar/video/data_vgg_affine/viewpoint1_graf/img2.ppm'
# fimg2='/home/ar/video/data_vgg_affine/viewpoint1_graf/img3.ppm'

fimg0='/home/ar/video/data_vgg_affine/zoomrot2_boat/img1.pgm'
fimg1='/home/ar/video/data_vgg_affine/zoomrot2_boat/img2.pgm'
fimg2='/home/ar/video/data_vgg_affine/zoomrot2_boat/img3.pgm'
fimg3='/home/ar/video/data_vgg_affine/zoomrot2_boat/img4.pgm'
fimg4='/home/ar/video/data_vgg_affine/zoomrot2_boat/img5.pgm'
fimg5='/home/ar/video/data_vgg_affine/zoomrot2_boat/img6.pgm'


# fimg0='/home/ar/video/data_vgg_affine/zoomrot1_bark/img1.ppm'
# fimg1='/home/ar/video/data_vgg_affine/zoomrot1_bark/img2.ppm'
# fimg2='/home/ar/video/data_vgg_affine/zoomrot1_bark/img3.ppm'
# fimg3='/home/ar/video/data_vgg_affine/zoomrot1_bark/img4.ppm'
# fimg4='/home/ar/video/data_vgg_affine/zoomrot1_bark/img5.ppm'
# fimg5='/home/ar/video/data_vgg_affine/zoomrot1_bark/img6.ppm'


def drawMatches(img1, img2, lkp1, lkp2, status, num=10):
    imgm1=cv2.drawKeypoints(img1, lkp1, color=(0,0,255))
    imgm2=cv2.drawKeypoints(img2, lkp2, color=(0,0,255))
    w1=imgm1.shape[1]
    totimg=np.concatenate((imgm1.copy(),imgm2.copy()), axis=1).copy()
    cnt=0
    cntb=0
    for pp1,pp2 in zip(lkp1,lkp2):
        if status[cnt]==1:
            p1=(int(pp1.pt[0]),    int(pp1.pt[1]))
            p2=(int(pp2.pt[0])+w1, int(pp2.pt[1]))
            cv2.line(totimg,p1,p2,(0,255,0))
            if cntb>num:
                break
            cntb+=1
        cnt+=1
    return totimg

def getDrawProjection(img1, img2, listH, listColors=[(0,255,0),(0,0,255),(0,255,255),(255,0,0), (255,255,255)]):
    if len(img1.shape)<3:
        img1=cv2.cvtColor(img1, cv2.COLOR_GRAY2BGR)
        img2=cv2.cvtColor(img2, cv2.COLOR_GRAY2BGR)
    imgTot=np.concatenate((img1,img2), axis=1).copy()
    im1w=img1.shape[1]
    im1h=img1.shape[0]
    pc=(im1w/2, im1h/2)
    dw=im1w/4
    dh=im1h/4
    arrROI=np.zeros((5,3))
    arrROI[0,:]=(pc[0]-dw, pc[1]-dh, 1)
    arrROI[1,:]=(pc[0]+dw, pc[1]-dh, 1)
    arrROI[2,:]=(pc[0]+dw, pc[1]+dh, 1)
    arrROI[3,:]=(pc[0]-dw, pc[1]+dh, 1)
    arrROI[4,:]=(pc[0]-dw, pc[1]-dh, 1)
    cnt=0
    for hh in listH:
        prjROI=np.transpose(np.dot(hh,np.transpose(arrROI)))
        prjROI=prjROI/np.matlib.repmat(prjROI[:,2],3,1).transpose()
        # print prjROI
        for ii in xrange(arrROI.shape[0]-1):
            p1=(int(arrROI[ii+0,0]), int(arrROI[ii+0,1]))
            p2=(int(arrROI[ii+1,0]), int(arrROI[ii+1,1]))
            cv2.line(imgTot, p1,p2, color=(0,255,0))
            p1=(int(prjROI[ii+0,0] + im1w), int(prjROI[ii+0,1]))
            p2=(int(prjROI[ii+1,0] + im1w), int(prjROI[ii+1,1]))
            cv2.line(imgTot, p1,p2, color=listColors[cnt])
        cnt+=1
    return imgTot

def getHomography(pk1, pd1, pk2, pd2):
    matcher=cv2.BFMatcher(cv2.NORM_L2)
    matches0=matcher.match(pd1, pd2)
    matches1=matcher.match(pd2, pd1)
    listGoodMatches=[]
    listP1=[]
    listP2=[]
    retlP1=[]
    retlP2=[]
    listDistances=[]
    for mm0 in matches0:
        if mm0.queryIdx==matches1[mm0.trainIdx].trainIdx:
            listGoodMatches.append(mm0)
            listDistances.append(mm0.distance)
            listP1.append(pk1[mm0.queryIdx].pt)
            listP2.append(pk2[mm0.trainIdx].pt)
            retlP1.append(pk1[mm0.queryIdx])
            retlP2.append(pk2[mm0.trainIdx])
    listP1=np.array(listP1, dtype=np.float32)
    listP2=np.array(listP2, dtype=np.float32)
    H,status = cv2.findHomography(listP1,listP2,cv2.RANSAC, 5.0)
    return (H, status, retlP1, retlP2, listGoodMatches)

###############################
if __name__=='__main__':
    img0=ph.resizeToMaxSize(cv2.imread(fimg0, 0), 512)
    img1=ph.resizeToMaxSize(cv2.imread(fimg1, 0), 512)
    img2=ph.resizeToMaxSize(cv2.imread(fimg2, 0), 512)
    img3=ph.resizeToMaxSize(cv2.imread(fimg3, 0), 512)
    #
    det=cv2.SIFT()
    kp0,kd0=det.detectAndCompute(img0, None)
    kp1,kd1=det.detectAndCompute(img1, None)
    kp2,kd2=det.detectAndCompute(img2, None)
    kp3,kd3=det.detectAndCompute(img3, None)
    #
    H01,s01,lpg0,lpg1,lmatch=getHomography(kp0,kd0, kp1,kd1)
    H12,s12,   _,   _,lmatch=getHomography(kp1,kd1, kp2,kd2)
    H23,s23,   _,   _,lmatch=getHomography(kp2,kd2, kp3,kd3)

    H03,s03,   _,   _,lmatch=getHomography(kp0,kd0, kp3,kd3)

    print H12
    print '----'
    H02p=np.dot(H01, H12)
    H03p=np.dot(H02p,H23)
    # H12p/=H12p[2,2]
    print H03p

    H03p2=np.dot(H23,np.dot(H12, H01))


    # cv2.imshow("win-matches",  drawMatches(img0, img1, lpg0,lpg1, s01))
    cv2.imshow("win-projection", getDrawProjection(img0,img3, [H03, H03p]))
    # cv2.imshow("win-projection", getDrawProjection(img1,img2, [H12]))

    cv2.waitKey(0)
