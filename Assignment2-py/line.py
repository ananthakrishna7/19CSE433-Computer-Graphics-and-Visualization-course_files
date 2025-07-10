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

class Line:
    def __init__(self,):

        with open('pt.tmp', 'rb') as f:
            self.p1, self.p2 = pickle.load(f)
        self.vertices = np.array(self.bressenham(), np.float32)
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

    def dda(self):
        dx = self.p2.x - self.p1.x
        dy = self.p2.y - self.p1.y
        n = int(max(fabs(self.p2.x - self.p1.x), fabs(self.p2.y - self.p1.y)))
        x = self.p1.x
        y = self.p1.y
        xinc = dx/n
        yinc = dy/n

        verts = []
        for i in range(n):
            x += xinc
            y += yinc
            verts.extend([int(x), int(y)])

        verts = self.screenToNDC(verts)
        return verts

    def reflectx(self):
        for i in range(len(self.vertices), 2):
            self.vertices[i] = -self.vertices[i]
    def bressenham(self):
        x1 = self.p1.x
        y1 = self.p1.y
        x2 = self.p2.x
        y2 = self.p2.y
        if x1 > x2:
            x1,x2 = x2,x1
            y1,y2 = y2,y1
        verts = [int(x1), int(y1)]
        dx = x2 - x1
        dy = y2 - y1 # neg
        neg = False
        if dy < 0:
            neg = True
            dy = -dy
        p = 2*dy - dx # neg
        x = x1
        y = y1
        while x <= x2:
            if p < 0:
                x += 1
                p += 2*dy # neg
            else:
                x += 1
                if not neg:
                    y += 1
                else:
                    y -= 1
                p += 2*dy - 2*dx
            verts.extend([x, y])

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