#!/usr/bin/python
__author__ = 'ar'

import sys
import numpy as np
import cv2
import PyQt4.QtCore as q4c
import PyQt4.QtGui  as q4
import random

fimg='/home/ar/img/doge2.jpg'

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
        #
        self.mainLH=q4.QHBoxLayout()
        self.btnQuit=q4.QPushButton('Quit')
        self.btnPush=q4.QPushButton('Append Image')
        self.chkShowHide=q4.QCheckBox('Show/Hide images')
        self.chkShowHide.setChecked(self.isShowImages)
        self.btnLV=q4.QVBoxLayout()
        self.btnLV.addWidget(self.btnPush)
        self.btnLV.addWidget(self.btnQuit)
        self.btnLV.addWidget(self.chkShowHide)
        self.btnLV.addStretch(1)
        #
        # self.view   = q4.QGraphicsView()
        self.view   = MainWidget.MyView()
        self.scene  = q4.QGraphicsScene()
        self.sceneSize=(1000,1000)
        self.sceneRect=(-500,-500,self.sceneSize[0],self.sceneSize[1])
        self.sceneBnd =(self.sceneRect[0],self.sceneRect[1],self.sceneRect[0]+self.sceneSize[0],self.sceneRect[1]+self.sceneSize[1])
        self.scene.setSceneRect(self.sceneRect[0],self.sceneRect[1],self.sceneRect[2],self.sceneRect[3])
        self.scene.addLine(self.sceneBnd[0],0,self.sceneBnd[2],0)
        self.scene.addLine(0,self.sceneBnd[1],0,self.sceneBnd[3])
        self.view.setScene(self.scene)
        #
        self.mainLH.addWidget(self.view)
        self.mainLH.addLayout(self.btnLV)
        self.setLayout(self.mainLH)
        #
        self.btnQuit.clicked.connect(self.slot_on_quit)
        self.btnPush.clicked.connect(self.slot_on_append)
        self.chkShowHide.clicked.connect(self.slot_on_showhide)
    def slot_on_append(self):
        if self.pxmTest :
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
    def appendImage(self, pxm, pos, angle):
        tmp=self.scene.addPixmap(pxm)
        tmp.setOffset(-pxm.width()/2,-pxm.height()/2)
        tmp.translate(pos[0],pos[1])
        tmp.rotate(angle)
        tmp.setVisible(self.isShowImages)
        sizE=10
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


if __name__=='__main__':
    app=q4.QApplication(sys.argv)
    img=cv2.imread(fimg,1)
    imgSiz=img.shape
    # pixm=q4.QPixmap.fromImage(q4.QImage(img[:,:,::-1].copy().data, imgSiz[1], imgSiz[0], imgSiz[1]*imgSiz[2], q4.QImage.Format_RGB888))
    # pixm=q4.QPixmap.fromImage(q4.QImage(img.copy().data, imgSiz[1], imgSiz[0], imgSiz[1]*imgSiz[2], q4.QImage.Format_RGB888))
    pixm=q4.QPixmap(fimg)
    # view=q4.QGraphicsView()
    # scene=q4.QGraphicsScene()
    # scene.setSceneRect(-500,-500,1000,1000)
    # scene.addLine(-500,0,500,0)
    # scene.addLine(0,-500,0,500)
    # tmp1=scene.addPixmap(pixm)
    # tmp1.rotate(30)
    # tmp1.translate(-pixm.width()/2, -pixm.width()/2)
    # view.setScene(scene)
    # mainWidget=q4.QWidget()
    # hbl=q4.QHBoxLayout()
    # hbl.addWidget(view)
    # vbl=q4.QVBoxLayout()
    # vbl.addWidget(q4.QPushButton('push1'))
    # vbl.addWidget(q4.QPushButton('push2'))
    # vbl.addStretch(1)
    # hbl.addLayout(vbl)
    # mainWidget.setLayout(hbl)
    # mainWidget.show()
    mainWidget=MainWidget()
    mainWidget.pxmTest=pixm
    mainWidget.appendImage(pixm,(0,200),45)
    mainWidget.show()
    sys.exit(app.exec_())
