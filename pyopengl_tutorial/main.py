import pygame as pg
from OpenGL.GL import *
import numpy as np
import ctypes
from OpenGL.GL.shaders import compileProgram, compileShader

class App:
    def __init__(self):
        pg.init()
        pg.display.set_mode((640, 480), pg.OPENGL | pg.DOUBLEBUF)
        self.clock = pg.time.Clock()

        glClearColor(1.0, 0.0, 0.0, 1.0)

        self.mainloop()

    def createShader(self, vertexShaderPath, fragmentShaderPath):
        with open(vertexShaderPath) as vs:
            vertexShaderSource = vs.read()

    def mainloop(self):
        running = True

        while running:
            for event in pg.event.get():
                if event.type == pg.QUIT :
                    running = False
            
            glClear(GL_COLOR_BUFFER_BIT)
            pg.display.flip() # swap buffers ig?

            self.clock.tick(60)
        self.quit()

    def quit(self):
        pg.quit()

class Triangle:
    def __init__(self):

        # x y x r g b
        self.vertices = np.array((
            -.5, .5, 0,0,1,0,
            .5,-.5,0,0,1,0,
            0,.5,0,0,1,0,
        ), dtype=np.float32)

        self.vetrex_count = 3

        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)
        self.vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, self.vbo)
        glBufferData(GL_ARRAY_BUFFER, self.vertices.nbytes, self.vertices, GL_STATIC_DRAW)

        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, ctypes.c_void_p(0))
        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, ctypes.c_void_p(12))

    def destroy(self):
        glDeleteVertexArrays(1, (self.vao,))
        glDeleteBuffers(1, (self.vbo,))
if __name__ == "__main__":
    myApp = App()
