#!/usr/bin/python
__author__ = 'ar'

import cv2
import numpy as np
import numpy.matlib
import os

import libphcorr as ph


# fimg0='/home/ar/img/lena.png'
# fimg1='/home/ar/img/lena_b_rot_scale_crop.png'


# fimg0='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_proc2/image-021.jpeg'
# fimg1='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc2/image-293.jpeg'

# fimg0='/home/ar/video/UAV_PHONE/part_2/vid1_2to1.mp4_proc2/image-021.jpeg'
# fimg1='/home/ar/video/UAV_PHONE/part_2/vid1_1to2.mp4_proc2/image-293.jpeg'

fimg0='/home/ar/video/Video_Murashko/data_1fps_resize/frame-00005.png'
fimg1='/home/ar/video/Video_Murashko/data_1fps_resize/frame-00006.png'

# fimg0='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/image-101.jpeg'
# fimg1='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/frm_00465.png'

#fimg0='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/image-109.jpeg'
#fimg1='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/frm_00267.png'



FLANN_INDEX_KDTREE=1

####################################
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

def drawMatchesAndHomography(img1, img2, lkp1, lkp2, status, H, num=10):
    imgm1=cv2.drawKeypoints(img1, lkp1, color=(0,0,255))
    imgm2=cv2.drawKeypoints(img2, lkp2, color=(0,0,255))
    w1=imgm1.shape[1]
    #
    siz1=(img1.shape[1],img1.shape[0])
    arrROI=np.array([(0,0,1), (siz1[0],0,1), (siz1[0],siz1[1],1), (0,siz1[1],1), (0,0,1)])
    prjROI=np.transpose(np.dot(H,np.transpose(arrROI)))
    prjROI=prjROI/np.matlib.repmat(prjROI[:,2],3,1).transpose()
    # img2n=imgm2.copy()
    for ii in xrange(arrROI.shape[0]-1):
            p1=(int(prjROI[ii+0,0] + 0*siz1[0]), int(prjROI[ii+0,1]))
            p2=(int(prjROI[ii+1,0] + 0*siz1[0]), int(prjROI[ii+1,1]))
            cv2.line(imgm2, p1,p2, color=(0,255,0), thickness=3)
    #
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
    H,status = cv2.findHomography(listP2,listP1,cv2.RANSAC, 5.0)
    status=status.reshape(-1)
    arrDst=np.array([xx.distance for xx in listGoodMatches])
    arrDst=arrDst[status==1]
    parScore=(np.sum(status), 100.*np.sum(status)/len(status), np.mean(arrDst), np.std(arrDst))
    return (H, status.reshape(-1), retlP1, retlP2, listGoodMatches, parScore)

def filter_matches(kp1, kp2, matches, ratio = 0.75):
    mkp1, mkp2 = [], []
    gmatches=[]
    for m in matches:
        if len(m) == 2 and m[0].distance < m[1].distance * ratio:
            m = m[0]
            mkp1.append( kp1[m.queryIdx] )
            mkp2.append( kp2[m.trainIdx] )
            gmatches.append(m)
    p1 = np.float32([kp.pt for kp in mkp1])
    p2 = np.float32([kp.pt for kp in mkp2])
    kp_pairs = zip(mkp1, mkp2)
    return p1, p2, kp_pairs, mkp1, mkp2, gmatches

def getHomography2(pk1, pd1, pk2, pd2):
    flann_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
    matcher = cv2.FlannBasedMatcher(flann_params, {})
    matcher.add([pd2])
    matcher.train()
    # raw_matches = matcher.knnMatch(pd1, trainDescriptors = pd2, k = 2)
    raw_matches = matcher.knnMatch(pd1, k = 2)
    listP1, listP2, _, retlP1, retlP2, listGoodMatches = filter_matches(pk1, pk2, raw_matches)
    H,status = cv2.findHomography(listP2,listP1,cv2.RANSAC, 5.0)
    status=status.reshape(-1)
    arrDst=np.array([xx.distance for xx in listGoodMatches])
    arrDst=arrDst[status==1]
    parScore=(np.sum(status), 100.*np.sum(status)/len(status), np.mean(arrDst), np.std(arrDst))
    return (H, status.reshape(-1), retlP1, retlP2, listGoodMatches, parScore)

def calcCorrCoeff(img1, img2):
    tmp1=img1.reshape(-1).astype(np.float)
    tmp2=img2.reshape(-1).astype(np.float)
    tmp1=(tmp1-np.mean(tmp1))/np.std(tmp1)
    tmp2=(tmp2-np.mean(tmp2))/np.std(tmp2)
    ret=np.sum(tmp1*tmp2)/len(tmp1)
    return ret

def calcCCHomography(imgBig, imgSmall, H):
    siz=imgSmall.shape
    borderSiz=(0.2*np.array(siz)).astype(np.int)
    imgBigWarpCrop=cv2.warpPerspective(imgBig, np.linalg.inv(H), (siz[1], siz[0]))
    imgBigWarpCrop=imgBigWarpCrop[borderSiz[0]:-borderSiz[0], borderSiz[1]:-borderSiz[1]]
    imgSmallCrop=imgSmall[borderSiz[0]:-borderSiz[0], borderSiz[1]:-borderSiz[1]]
    return calcCorrCoeff(imgBigWarpCrop, imgSmallCrop)

####################################
if __name__=='__main__':
    img0=cv2.imread(fimg0, 0)
    img1=cv2.imread(fimg1, 0)
    img0=ph.resizeToMaxSize(img0, 768)
    img1=ph.resizeToMaxSize(img1, 768)
    det=cv2.SURF()
    kp0,kd0=det.detectAndCompute(img0, None)
    kp1,kd1=det.detectAndCompute(img1, None)
    img0kp=cv2.drawKeypoints(img0, kp0, 4)
    img1kp=cv2.drawKeypoints(img1, kp1, 4)


    H, status, mKP0, mKP1, listGoodMatches, scores= getHomography2(kp0, kd0, kp1, kd1)

    print "%d/%d : status=%0.2f%%" % (np.sum(status), len(status), 100.*(np.sum(status)/len(status)))
    print scores

    imgMatch=drawMatchesAndHomography(img1, img0, mKP1, mKP0, status, H, num=100)

    imgWarp10=cv2.warpPerspective(img1, H, (img0.shape[1], img0.shape[0]))
    imgWarp01=cv2.warpPerspective(img0, np.linalg.inv(H), (img1.shape[1], img1.shape[0]))

    borderSiz=(0.2*np.array(img1.shape)).astype(np.int)
    imgWarp01Crop=imgWarp01[borderSiz[0]:imgWarp01.shape[0]-borderSiz[0], borderSiz[1]:imgWarp01.shape[1]-borderSiz[1]].copy()
    img1Crop=img1[borderSiz[0]:img1.shape[0]-borderSiz[0], borderSiz[1]:img1.shape[1]-borderSiz[1]].copy()


    cv2.imshow("win-imgWarp01Crop", imgWarp01Crop)
    cv2.imshow("win-img1Crop", img1Crop)
    # print "Corr=%0.2f" % calcCorrCoeff(imgWarp01Crop, img1Crop)
    print "Corr=%0.2f" % calcCCHomography(img0, img1, H)

    cv2.imshow("stack 0->1", np.dstack( (img1Crop, imgWarp01Crop, img1Crop) ))
    # cv2.imshow("win0", img0kp)
    # cv2.imshow("win1", img1kp)
    cv2.imshow("winMatch",  imgMatch)
    # cv2.imshow("winWarp10", imgWarp10)
    # cv2.imshow("winWarp01", imgWarp01)
    cv2.waitKey(0)

    print 'test'
