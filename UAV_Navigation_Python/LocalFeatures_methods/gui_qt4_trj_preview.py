#!/usr/bin/python
__author__ = 'ar'

import os
import sys
import numpy as np
import cv2
import PyQt4.QtCore as q4c
import PyQt4.QtGui  as q4
import random

import libphcorr as ph

def convertMat2Pixmap(mat):
    if len(mat.shape)<3:
        mat=cv2.cvtColor(mat, cv2.COLOR_GRAY2RGB)
    imgSiz=mat.shape
    ret=q4.QPixmap.fromImage(q4.QImage(mat.copy().data, imgSiz[1], imgSiz[0], imgSiz[1]*imgSiz[2], q4.QImage.Format_RGB888))
    return ret

#######################################
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
        self.listPts=None
        self.isShowImages=True
        self.videoNavigator=None
        self.frameGrabber=None
        self.numFrames=0
        self.posFrame=0
        self.skipFrames=0
        self.oldPath=os.path.expanduser('~')
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
        self.sceneSize=(22000,22000)
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
        self.btnProcessNext=q4.QPushButton('Load Trajectory -->')
        self.chkProcessAuto=q4.QCheckBox('Auto')
        self.btnCleanScene=q4.QPushButton('Clean')
        self.btnSelectDialog=q4.QPushButton('...')
        self.chkProcessAuto.setChecked(False)
        self.editPath=q4.QLineEdit()
        self.ltPath=q4.QHBoxLayout()
        self.ltPath.addWidget(self.btnProcessPath)
        self.ltPath.addWidget(self.chkProcessAuto)
        self.ltPath.addWidget(self.editPath)
        self.ltPath.addWidget(self.btnCleanScene)
        self.ltPath.addWidget(self.btnSelectDialog)
        self.ltPath.addWidget(self.btnProcessNext)
        self.mainVL.addLayout(self.ltPath)
        self.mainVL.addLayout(self.mainLH)
        # self.setLayout(self.mainLH)
        self.setLayout(self.mainVL)
        #
        self.btnProcessPath.setEnabled(False)
        self.chkProcessAuto.setEnabled(False)
        self.btnExport.setEnabled(False)
        self.btnPush.setEnabled(False)
        #
        self.btnQuit.clicked.connect(self.slot_on_quit)
        self.btnPush.clicked.connect(self.slot_on_append)
        self.chkShowHide.clicked.connect(self.slot_on_showhide)
        self.btnProcessNext.clicked.connect(self.slot_on_loadTrajectory)
        self.btnSelectDialog.clicked.connect(self.slot_on_select_dialog)
        self.btnCleanScene.clicked.connect(self.slot_on_clean_scene)
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
    def slot_on_select_dialog(self):
        filename = q4.QFileDialog.getOpenFileName(self, 'Open file', self.oldPath, "Image-index (idx.csv)")
        filename = str(filename)
        isError=False
        if not filename:
            isError=True
        else:
            if os.path.isfile(filename):
                self.oldPath=os.path.dirname(filename)
                self.editPath.setText(filename)
            else:
                msg=q4.QMessageBox()
                msg.setText("Cant find idx-file [%s]" % filename )
                msg.exec_()
    def slot_on_clean_scene(self):
        if self.listPxm!=None:
            for ii in self.listPxm:
                self.scene.removeItem(ii)
                ii=None
            for ii in self.listPts:
                self.scene.removeItem(ii)
                ii=None
            self.listPxm=None
            self.listPts=None
            self.pxmCoords=None
    def slot_on_append(self):
        if self.pxmTest:
            tmpAng=random.randint(0,360)
            tmpPosX=random.randint(self.sceneBnd[0],self.sceneBnd[2])
            tmpPosY=random.randint(self.sceneBnd[1],self.sceneBnd[3])
            self.appendImage(self.pxmTest,(tmpPosX,tmpPosY),tmpAng)
    def slot_on_loadTrajectory(self):
        fidxPath=self.editPath.text()
        ftrjPath="%s_trjfp.csv" % fidxPath
        if (not os.path.isfile(fidxPath)) or (not os.path.isfile(ftrjPath)):
            msg=q4.QMessageBox()
            msg.setText("Cant find idx-file [%s] or [%s]" % (fidxPath, ftrjPath) )
            msg.exec_()
        else:
            cap=ph.VideoCSVReader(str(fidxPath))
            numFrames=cap.getNumFrames()
            arrTrj=np.genfromtxt(ftrjPath, delimiter=',')
            # assert arrTrj.shape[0]==numFrames
            numFramesGood=min(arrTrj.shape[0], numFrames)
            # numFramesGood=100
            self.progress.setMinimum(0)
            self.progress.setMaximum(numFramesGood-1)
            for ii in xrange(numFramesGood):
                tret,timg=cap.read(ii)
                if tret:
                    pxm=convertMat2Pixmap(timg)
                    self.appendImage(pxm, (arrTrj[ii,0],arrTrj[ii,1]), arrTrj[ii,2], arrTrj[ii,3])
                self.progress.setValue(ii)
                q4.QApplication.processEvents()
    def slot_on_showhide(self):
        self.isShowImages=self.chkShowHide.isChecked()
        if self.listPxm==None:
            msg=q4.QMessageBox()
            msg.setText("Trajectory data not loaded... skip")
            msg.exec_()
        else:
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
        tmpLine1=self.scene.addLine(0,0,sizL,0)
        tmpLine1.translate(pos[0],pos[1])
        tmpLine1.rotate(angle)
        tmpLine2=self.scene.addLine(0,0,0,-sizL)
        tmpLine2.translate(pos[0],pos[1])
        tmpLine2.rotate(angle)
        tmpEllipse=self.scene.addEllipse(pos[0]-sizE,pos[1]-sizE,2*sizE,2*sizE)
        if self.listPxm:
            self.listPxm.append(tmp)
            self.listPts.append(tmpLine1)
            self.listPts.append(tmpLine2)
            self.listPts.append(tmpEllipse)
        else:
            self.listPxm=[tmp]
            self.listPts=[tmpLine1, tmpLine2, tmpEllipse]
        if self.pxmCoords:
            prevPos=self.pxmCoords[-1]
            self.pxmCoords.append((pos[0],pos[1],angle))
            tmpLine3=self.scene.addLine(prevPos[0],prevPos[1],pos[0],pos[1])
            self.listPts.append(tmpLine3)
        else:
            self.pxmCoords=[(pos[0],pos[1],angle)]
        # tmp.setO(-pxm.width()/2,-pxm.height()/2)
    def setDataPath(self, path):
        self.editPath.setText(path)
        if not os.path.isfile(path):
            msg=q4.QMessageBox()
            msg.setText("Can't find path [%s]" % path)
            msg.exec_()
        else:
            self.oldPath=os.path.dirname(path)

def usage(sysArgv):
    print "Usage: %s {/path/to/dir-with-frames-and-trj/idx_file.csv}"

#######################################
if __name__=='__main__':
    # fvideo='/home/ar/video/data_test_frames_rot_scl_0/idx.csv'
    fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'
    # fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_2to1.mp4_ffmpeg/idx.csv'
    # fvideo='/home/ar/video/UAV_PHONE/part_2/vid0_1to2.mp4_proc/idx.csv'
    # fvideo='/home/ar/video/UAV_PHONE/DJI/DJI_0005_MOV_CROP_StonePark/frames-crop1/idx.csv'
    #
    app=q4.QApplication(sys.argv)
    if len(sys.argv)>1:
        fvideo=sys.argv[1]
    else:
        usage(sys.argv)
    mainWidget=MainWidget()
    mainWidget.show()
    mainWidget.setDataPath(fvideo)
    sys.exit(app.exec_())
