#!/usr/bin/python
__author__ = 'ar'

import sys

import numpy as np

import PyQt4.QtCore     as qtc
import PyQt4.QtGui      as qt
import PyQt4.QtOpenGL   as qtgl

from OpenGL import GL
from OpenGL import GLU

# try:
#     from OpenGL import GL
# except ImportError:
#     app = q4.QApplication(sys.argv)
#     q4.QMessageBox.critical(None, "OpenGL samplebuffers",
#             "PyOpenGL must be installed to run this example.")
#     sys.exit(1)

class GLWidget(qtgl.QGLWidget):
    def __init__(self, parent):
        super(GLWidget, self).__init__(qtgl.QGLFormat(qtgl.QGL.SampleBuffers), parent)
        self.listObj=[]
        self.colorRed   =qt.QColor.fromCmykF(1,0,0,0)
        self.colorGreen =qt.QColor.fromCmykF(0,1,0,0)
        self.colorBlue  =qt.QColor.fromCmykF(0,0,1,0)
        self.colorYellow=qt.QColor.fromCmykF(1,1,0,0)
        self.colorGrey  =qt.QColor.fromCmykF(0.5,0.5,0.5,0)
        self.colorBlack =qt.QColor.fromCmykF(0,0,0,0)
        self.colorWhite =qt.QColor.fromCmykF(1.0,1.0,1.0,0.0)
        #
    def initializeGL(self):
        self.qglClearColor(qtc.Qt.black)
        GL.glShadeModel(GL.GL_FLAT)
        GL.glEnable(GL.GL_DEPTH_TEST)
        GL.glEnable(GL.GL_CULL_FACE)
        GL.glShadeModel(GL.GL_SMOOTH)
        GL.glHint(GL.GL_PERSPECTIVE_CORRECTION_HINT, GL.GL_NICEST)
        #
        GL.glEnable(GL.GL_TEXTURE_2D)
        # GL.glTexGeni(GL.GL_S, GL.GL_TEXTURE_GEN_MODE, GL.GL_SPHERE_MAP)
        # GL.glTexGeni(GL.GL_T, GL.GL_TEXTURE_GEN_MODE, GL.GL_SPHERE_MAP)
        #
        self.textures=[]
        self.fimg='/home/ar/img/lena.png'
        # self.fimg='/home/ar/img/doge.jpg'
        self.timg=qt.QImage(self.fimg)
        self.textures.append(self.bindTexture(self.timg))
        self.hmap=30.0*np.random.rand(self.timg.height(), self.timg.width())
        self.sizW=self.timg.width()
        self.sizH=self.timg.height()
        self.resg=8.0
        #
        self.listObj=self.makeObject()
    def resizeGL(self, w, h):
        side = min(w, h)
        if side < 0:
            return
        GL.glViewport(0,0,w,h)
        # GL.glViewport((w - side) // 2, (h - side) // 2, side, side)

        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glLoadIdentity()
        # GL.glOrtho(-self.sizW, +self.sizW, -self.sizH, +self.sizH, -100.0, +5000.0)
        GLU.gluPerspective(60.0, float(w)/float(h), 0.01, 5000.0)
        GL.glMatrixMode(GL.GL_MODELVIEW)
    def paintGL(self):
        GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT)
        # GL.glMatrixMode(GL.GL_MODELVIEW)
        GL.glLoadIdentity()
        GL.glTranslatef(-self.sizW/2.0, -self.sizH/2.0, -900.0)
        # GL.glTranslatef(0, 0, -500.0)
        GL.glRotatef(15.0, 1,0,0)
        # GL.glRotatef(15.0, 0,1,0)
        # GL.glClearColor(0,0,0,0)
        # GL.glPolygonMode( GL.GL_FRONT_AND_BACK, GL.GL_LINE )
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.textures[0])
        GL.glCallList(self.listObj)
        # GL.glPolygonMode( GL.GL_FRONT_AND_BACK, GL.GL_FILL )
        # GL.glPushMatrix()
    def makeObject(self):
        genList = GL.glGenLists(1)
        GL.glNewList(genList, GL.GL_COMPILE)
        self.qglColor(qtc.Qt.white)
        # tsiz=10
        # th=0.0
        # GL.glBegin(GL.GL_QUADS)
        # GL.glVertex3f(-tsiz, +tsiz, th)
        # GL.glVertex3f(-tsiz, -tsiz, th)
        # GL.glVertex3f(+tsiz, -tsiz, th)
        # GL.glVertex3f(+tsiz, +tsiz, th)
        # GL.glEnd()
        # GL.glBegin(GL.GL_TRIANGLES)

        nx=int(np.floor(self.sizW/self.resg))
        ny=int(np.floor(self.sizH/self.resg))
        dx=float(self.sizW)/(nx-1)
        dy=float(self.sizH)/(ny-1)
        dtx=1.0/(nx-1)
        dty=1.0/(ny-1)
        xc=0.0*float(self.sizW)/2.0
        yc=0.0*float(self.sizH)/2.0
        for xi in xrange(nx-2):
            for yi in xrange(ny-2):
                x1=dx*xi-xc
                x2=dx*(xi+1)-xc
                y1=self.sizH-dy*yi-yc
                y2=self.sizH-dy*(yi+1)-yc
                tx1=dtx*xi
                tx2=dtx*(xi+1)
                ty1=1.0-dty*yi
                ty2=1.0-dty*(yi+1)
                tz11=self.hmap[self.sizH-1-self.resg*yi, self.resg*xi]
                tz21=self.hmap[self.sizH-1-self.resg*yi, self.resg*(xi+1)]
                tz22=self.hmap[self.sizH-1-self.resg*(yi+1), self.resg*(xi+1)]
                tz12=self.hmap[self.sizH-1-self.resg*(yi+1), self.resg*xi]
                #
                GL.glBegin(GL.GL_QUADS)
                GL.glTexCoord2f(tx1, ty2)
                GL.glVertex3f(x1, y2, tz12)

                GL.glTexCoord2f(tx2, ty2)
                GL.glVertex3f(x2, y2, tz22)

                GL.glTexCoord2f(tx2, ty1)
                GL.glVertex3f(x2, y1, tz21)

                GL.glTexCoord2f(tx1, ty1)
                GL.glVertex3f(x1, y1, tz11)
                GL.glEnd()
        GL.glEndList()
        return genList


if __name__=='__main__':
    app=qt.QApplication(sys.argv)
    window = GLWidget(None)
    window.show()
    sys.exit(app.exec_())
