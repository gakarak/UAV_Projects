#!/usr/bin/python
__author__ = 'ar'

import os
import sys
import numpy as np
import cv2
import PyQt4.QtCore as q4c
import PyQt4.QtGui  as q4
import random
import phcorr as ph

# fimg='/home/ar/img/doge2.jpg'
fimg='/home/ar/img/lena.png'
# fvideo='/home/ar/big.data/data.UAV/data_AlexKravchonok/video.avi'
# fvideo='/home/ar/video/data_AlexKravchonok/video.avi'
# fvideo='/home/ar/video/drone_project/frames.txt'
# fvideo='/home/ar/video/data_test_frames_ShiftX_0/idx.csv'
# fvideo='/home/ar/video/data_test_frames_ShiftY_0/idx.csv'
# fvideo='/home/ar/video/data_test_frames_rot_0/idx.csv'
# fvideo='/home/ar/video/data_test_frames_scl_0/idx.csv'
fvideo='/home/ar/video/data_test_frames_rot_scl_0/idx.csv'

# fvideo='/home/ar/video/UAV_PHONE/part_2/test_vid0_from_1to2.mp4'
# fvideo='/home/ar/video/UAV_PHONE/part_2/test_vid0_from_2to1.mp4'


parMaxFrameSize=256

def convertMat2Pixmap(mat):
    if len(mat.shape)<3:
        mat=cv2.cvtColor(mat, cv2.COLOR_GRAY2RGB)
    imgSiz=mat.shape
    ret=q4.QPixmap.fromImage(q4.QImage(mat.copy().data, imgSiz[1], imgSiz[0], imgSiz[1]*imgSiz[2], q4.QImage.Format_RGB888))
    return ret


class MainWidget(q4.QWidget):
    class MyView(q4.QGraphicsView):
        def __init__(self):
            super(MainWidget.MyView,self).__init__()
            self.name="MyView"
        def wheelEvent(self, ev):
            scaleFactor=1.1
            if ev.delta()>0:
                self.scale(scaleFactor,scaleFactor)
            else:
                self.scale(1./scaleFactor,1./scaleFactor)
    def __init__(self):
        super(MainWidget,self).__init__()
        #
        self.pxmTest=None
        self.pxmCoords=None
        self.listPxm=None
        self.isShowImages=True
        self.videoNavigator=None
        self.frameGrabber=None
        self.numFrames=0
        self.posFrame=0
        self.skipFrames=0
        #
        self.mainVL=q4.QVBoxLayout()
        self.mainLH=q4.QHBoxLayout()
        self.btnQuit=q4.QPushButton('Quit')
        self.btnPush=q4.QPushButton('Push Me')
        self.btnExport=q4.QPushButton('Export Trj')
        self.chkShowHide=q4.QCheckBox('Show/Hide images')
        self.labelInfo=q4.QLabel('...')
        self.chkShowHide.setChecked(self.isShowImages)
        self.progress=q4.QProgressBar()
        self.progress.setMinimum(0)
        self.progress.setMaximum(1)
        self.progress.setValue(0)
        self.btnLV=q4.QVBoxLayout()
        self.btnLV.addWidget(self.btnPush)
        self.btnLV.addWidget(self.btnExport)
        self.btnLV.addWidget(self.btnQuit)
        self.btnLV.addWidget(self.chkShowHide)
        self.btnLV.addWidget(self.HLine())
        self.btnLV.addWidget(self.labelInfo)
        self.btnLV.addStretch(1)
        self.btnLV.addWidget(self.progress)
        #
        # self.view   = q4.QGraphicsView()
        self.view   = MainWidget.MyView()
        self.scene  = q4.QGraphicsScene()
        self.sceneSize=(14000,14000)
        self.sceneRect=(-self.sceneSize[0]/2.0, -self.sceneSize[1]/2.0, self.sceneSize[0],self.sceneSize[1])
        self.sceneBnd =(self.sceneRect[0],self.sceneRect[1],self.sceneRect[0]+self.sceneSize[0],self.sceneRect[1]+self.sceneSize[1])
        self.scene.setSceneRect(self.sceneRect[0],self.sceneRect[1],self.sceneRect[2],self.sceneRect[3])
        self.scene.addLine(self.sceneBnd[0],0,self.sceneBnd[2],0)
        self.scene.addLine(0,self.sceneBnd[1],0,self.sceneBnd[3])
        self.view.setScene(self.scene)
        #
        self.mainLH.addWidget(self.view)
        self.mainLH.addLayout(self.btnLV)
        #
        self.btnProcessPath=q4.QPushButton('Run processing')
        self.btnProcessNext=q4.QPushButton('Next -->')
        self.chkProcessAuto=q4.QCheckBox('Auto')
        self.chkProcessAuto.setChecked(False)
        self.editPath=q4.QLineEdit()
        self.ltPath=q4.QHBoxLayout()
        self.ltPath.addWidget(self.btnProcessPath)
        self.ltPath.addWidget(self.chkProcessAuto)
        self.ltPath.addWidget(self.editPath)
        self.ltPath.addWidget(self.btnProcessNext)
        self.mainVL.addLayout(self.ltPath)
        self.mainVL.addLayout(self.mainLH)
        # self.setLayout(self.mainLH)
        self.setLayout(self.mainVL)
        #
        self.btnQuit.clicked.connect(self.slot_on_quit)
        self.btnPush.clicked.connect(self.slot_on_append)
        self.btnExport.clicked.connect(self.slot_on_export_trj)
        self.btnProcessPath.clicked.connect(self.slot_on_process)
        self.btnProcessNext.clicked.connect(self.slot_on_process_next)
        self.chkShowHide.clicked.connect(self.slot_on_showhide)
    def HLine(self):
        tmp=q4.QFrame()
        tmp.setFrameShape(q4.QFrame.HLine)
        tmp.setFrameShadow(q4.QFrame.Sunken)
        return tmp
    def VLine(self):
        tmp=q4.QFrame()
        tmp.setFrameShape(q4.QFrame.VLine)
        tmp.setFrameShadow(q4.QFrame.Sunken)
        return tmp
    def slot_on_export_trj(self):
        msg=q4.QMessageBox()
        if (self.videoNavigator!=None) and self.videoNavigator.isInitialised():
            pathCSVTrj="%s_trj.csv" % self.editPath.text().toAscii()
            np.savetxt(pathCSVTrj, self.videoNavigator.getNavDataAsArray(), delimiter=',')
            msg.setText("Trajectory exported to [%s]" % pathCSVTrj)
        else:
            msg.setText("Error, Modelled Trajectory is Absent!")
        msg.exec_()
    def slot_on_process(self):
        pathData=self.editPath.text().toAscii()
        # print pathData
        pathData=fvideo
        # str=q4c.QString()
        # str.toAscii()
        self.frameGrabber=None
        self.numFrames=0
        if os.path.isfile(pathData):
            try:
                self.frameGrabber=ph.VideoCSVReader(pathData)
                self.numFrames=self.frameGrabber.getNumFrames()
                self.skipFrames=0
            except:
                self.frameGrabber=cv2.VideoCapture(pathData)
                self.numFrames=self.frameGrabber.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT)
                self.skipFrames=0
            self.progress.setMinimum(0)
            self.progress.setMaximum(self.numFrames)
            self.progress.setValue(1)
            self.videoNavigator=ph.VideoNavigator(maxFrameSize=parMaxFrameSize)
            while True:
                if not self.slot_on_process_next():
                    break
                if not self.chkProcessAuto.isChecked():
                    break
        else:
            msg=q4.QMessageBox()
            msg.setText("Can't find path [%s]" % pathData)
            msg.exec_()
        print '::slot_on_process()'
    def slot_on_process_next(self):
        return self.fun_process_next()
    def fun_process_next(self):
        ret=False
        frm=None
        for ii in xrange(0,self.skipFrames+1):
            ret,frm=self.frameGrabber.read()
            self.posFrame+=1
        if not ret:
            return False
        frm=cv2.cvtColor(frm, cv2.COLOR_BGR2GRAY)
        if not self.videoNavigator.isInitialised():
            self.videoNavigator.setStartState(frm)
        else:
            self.videoNavigator.processNewFrame(frm, isDebug=True, isUseGradMag=True)
        tmpState=self.videoNavigator.getLastState()
        if tmpState!=None:
            print tmpState
            self.appendImage(convertMat2Pixmap(self.videoNavigator.getCurrentFrame()), (tmpState[0],tmpState[1]), tmpState[2], tmpState[3])
        txtInfo="Position=[%0.f, %0.1f]\nAngle=%0.1f\nScale=%0.1f\nQuality=%0.1f" % (tmpState[0], tmpState[1], tmpState[2], tmpState[3], tmpState[4])
        self.labelInfo.setText(txtInfo)
        self.progress.setValue(self.posFrame)
        cv2.waitKey(2)
        q4.QApplication.processEvents()
        print '::slot_on_process_next()'
        return True
    def slot_on_append(self):
        if self.pxmTest:
            tmpAng=random.randint(0,360)
            tmpPosX=random.randint(self.sceneBnd[0],self.sceneBnd[2])
            tmpPosY=random.randint(self.sceneBnd[1],self.sceneBnd[3])
            self.appendImage(self.pxmTest,(tmpPosX,tmpPosY),tmpAng)
    def slot_on_showhide(self):
        self.isShowImages=self.chkShowHide.isChecked()
        for ii in self.listPxm:
            ii.setVisible(self.isShowImages)
    def slot_on_quit(self):
        q4.qApp.quit()
    def appendImage(self, pxm, pos, angle, scale=1.0):
        tmp=self.scene.addPixmap(pxm)
        tmp.setOffset(-pxm.width()/2,-pxm.height()/2)
        tmp.translate(pos[0],pos[1])
        # tmpScl=0.5+0.5*random.random()
        tmp.rotate(angle)
        tmp.scale(scale,scale)
        tmp.setVisible(self.isShowImages)
        sizE=10
        sizL=sizE*5.0
        # tmpLine=self.scene.addLine(pos[0],pos[1],pos[0]+sizE*5.0,pos[1])
        tmpLine=self.scene.addLine(0,0,sizL,0)
        tmpLine.translate(pos[0],pos[1])
        tmpLine.rotate(angle)
        tmpLine=self.scene.addLine(0,0,0,-sizL)
        tmpLine.translate(pos[0],pos[1])
        tmpLine.rotate(angle)
        self.scene.addEllipse(pos[0]-sizE,pos[1]-sizE,2*sizE,2*sizE)
        if self.listPxm:
            self.listPxm.append(tmp)
        else:
            self.listPxm=[tmp]
        if self.pxmCoords:
            prevPos=self.pxmCoords[-1]
            self.pxmCoords.append((pos[0],pos[1],angle))
            self.scene.addLine(prevPos[0],prevPos[1],pos[0],pos[1])
        else:
            self.pxmCoords=[(pos[0],pos[1],angle)]
        # tmp.setO(-pxm.width()/2,-pxm.height()/2)
    def setDataPath(self, path):
        self.editPath.setText(path)
        if not os.path.isfile(path):
            msg=q4.QMessageBox()
            msg.setText("Can't find path [%s]" % path)
            msg.exec_()


#################################
if __name__=='__main__':
    # frm1=cv2.imread('/home/ar/video/lena_crop.png',0)
    # frm2=cv2.imread('/home/ar/video/lena_crop_rot.png',0)

    # csvReader=ph.VideoCSVReader(fvideo)
    # csvReader.printInfo()
    # _,frm1=csvReader.read(9)
    # _,frm2=csvReader.read(10)
    # frm1=cv2.cvtColor(frm1, cv2.COLOR_BGR2GRAY)
    # frm2=cv2.cvtColor(frm2, cv2.COLOR_BGR2GRAY)
    #
    # print ph.imregcorr(frm1, frm2, isDebug=True, isSelectBestShiftPQ=True)
    # cv2.waitKey(0)
    # sys.exit(0)
    #
    app=q4.QApplication(sys.argv)
#    img=cv2.imread(fimg,0)
#    imgSiz=img.shape
#    pixm=q4.QPixmap(fimg)
    # pixm=convertMat2Pixmap(img)
    mainWidget=MainWidget()
#    mainWidget.pxmTest=pixm
    # mainWidget.appendImage(pixm,(0,200),-1)
    mainWidget.show()
    mainWidget.setDataPath(fvideo)
    sys.exit(app.exec_())
