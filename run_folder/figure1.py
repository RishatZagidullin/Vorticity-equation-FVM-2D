from PyQt5 import QtGui  # (the example applies equally well to PySide)
import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui
from PyQt5.QtCore import Qt
import pyqtgraph.ptime as ptime
import pyqtgraph.opengl as gl
from pyqtgraph.pgcollections import OrderedDict

## Always start by initializing Qt (only once per application)
app = QtGui.QApplication([])
## Define a top-level widget to hold everything
w = QtGui.QWidget()
w.setWindowTitle("advection-coagulation")
## Create some widgets to be placed inside
## Create a grid layout to manage the widgets size and position
layout = QtGui.QGridLayout()
w.setLayout(layout)
w.resize(875, 800)
w.show()
## Add widgets to the layout in their proper positions
grid = gl.GLViewWidget()
#grid.setBackgroundColor("b")
grid.setCameraPosition(elevation=47, azimuth=155 , distance=4)

verts = np.loadtxt("vertices.txt")
faces = np.loadtxt("faces.txt", dtype=int)
colors = np.genfromtxt("colours.txt", max_rows=len(faces), usecols = (1, 1, 1, 3))
## Mesh item will automatically compute face normals.
meshes = []
linex = []
liney = []
for i in range(29):
    meshes.append(gl.GLMeshItem(vertexes=verts, faces=faces, faceColors=colors, drawEdges=False, edgeColor=[1,1,1,1]))
    meshes[i].translate(-0.75, -0.25, float(i/60))
    meshes[i].setGLOptions('additive')
    meshes[i].rotate(90, 0, 0, 1)
    grid.addItem(meshes[i])

    linex.append(gl.GLLinePlotItem(pos=np.array([[0,1.5, float(i/60)],  [-0.5,1.5,float(i/60)]]), color=(255,255,255,255), antialias=True))
    linex[i].translate(0.25, -0.75, 0.0)
    grid.addItem(linex[i])

    liney.append(gl.GLLinePlotItem(pos=np.array([[-0.5,0,float(i/60)],  [-0.5,1.5,float(i/60)]]), color=(255,255,255,255), antialias=True))
    liney[i].translate(0.25, -0.75, 0.0)
    grid.addItem(liney[i])
'''
m1 = gl.GLMeshItem(vertexes=verts, faces=faces, faceColors=colors, smooth=True, drawEdges=False, edgeColor=[1,1,1,1])
m1.translate(-0.75, -0.25, 0.0)
m1.setGLOptions('additive')
m1.rotate(90, 0,0,1)
grid.addItem(m1)

m2 = gl.GLMeshItem(vertexes=verts, faces=faces, faceColors=colors, smooth=True, drawEdges=False, edgeColor=(1,1,1,1))
m2.translate(-0.75, -0.25, 0.3)
m2.setGLOptions('additive')
m2.rotate(90, 0,0,1)
grid.addItem(m2)
'''
m3 = gl.GLMeshItem(vertexes=verts, faces=faces, faceColors=colors, smooth=True, drawEdges=True, edgeColor=(1,1,1,1))
m3.translate(-0.75, -0.25, 0.485)
m3.setGLOptions('additive')
m3.rotate(90, 0,0,1)
grid.addItem(m3)


linex = gl.GLLinePlotItem(pos=np.array([[0,0,0],  [-1,0,0]]), color=(0,255, 0, 255), antialias=True)
linex.translate(0.25, -0.75, 0.0)
grid.addItem(linex)

liney = gl.GLLinePlotItem(pos=np.array([[0,0,0],  [0,2,0]]), color=(0,255, 0, 255), antialias=True)
liney.translate(0.25, -0.75, 0.0)
grid.addItem(liney)

linez = gl.GLLinePlotItem(pos=np.array([[0,0,0],  [0,0,1]]), color=(0,255, 0, 255), antialias=True)
linez.translate(0.25, -0.75, 0.0)
grid.addItem(linez)


borderlinesx = []
borderlinesy = []

for i in range(9):
    borderlinesx.append(gl.GLLinePlotItem(pos=np.array([[float(-(i)*0.0625),1.5,0], [float(-(i)*0.0625),1.5,0.485]])))
    borderlinesx[i].translate(0.25, -0.75, 0.0)
    grid.addItem(borderlinesx[i])

for i in range(33):
    borderlinesy.append(gl.GLLinePlotItem(pos=np.array([[-0.5, float((i)*0.0469),0], [-0.5, float((i)*0.0469),0.485]])))
    borderlinesy[i].translate(0.25, -0.75, 0.0)
    grid.addItem(borderlinesy[i])

layout.addWidget(grid, 0, 0)



## Start Qt event loop unless running in interactive mode.

if __name__ == '__main__':
    import sys
    total_frames = int(sys.argv[1])
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()
