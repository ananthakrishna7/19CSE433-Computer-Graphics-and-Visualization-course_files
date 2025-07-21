from OpenGL.GL import *
import numpy as np
import ctypes
from math import fabs
import pickle

SCREEN=(640, 480)

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Circle:
    def __init__(self,):

        with open('ptc.tmp', 'rb') as f:
            self.xc, self.yc, self.r = pickle.load(f)
        self.vertices = np.array(self.midpoint(), np.float32)
        self.vertex_count = len(self.vertices)//2
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)
        self.vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, self.vbo)
        glBufferData(GL_ARRAY_BUFFER, self.vertices.nbytes, self.vertices, GL_STATIC_DRAW)

        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, ctypes.c_void_p(0))
        # glEnableVertexAttribArray(1)
        # glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, ctypes.c_void_p(12))

    def midpoint(self):
        t1 = self.r / 16
        x = self.r
        y = 0
        verts = []
        while x >= y:
            verts.extend([x, y, y, x, -x, y, -y, x, x, -y, y, -x, -x, -y, -y, -x])
            y += 1
            t1 = t1 + y
            t2 = t1 - x
            if t2 >= 0:
                t1 = t2
                x -= 1
        verts = self.screenToNDC(verts)
        return verts
    
    def screenToNDC(self, verts):
        # try screen. then try min max
        for i in range(0, len(verts), 2):
            verts[i] = verts[i]/SCREEN[0]
            verts[i + 1] = verts[i + 1]/SCREEN[1]
        
        return verts
            

    def destroy(self):
        glDeleteVertexArrays(1, (self.vao,))
        glDeleteBuffers(1, (self.vbo,))