import pygame as pg
from OpenGL.GL import *
from line import Line, Point
from circle import Circle
from OpenGL.GL.shaders import compileProgram, compileShader
import pickle

SCREEN=(640, 480)
class App:
    def __init__(self):
        pg.init()
        pg.display.set_mode(SCREEN, pg.OPENGL | pg.DOUBLEBUF)
        self.clock = pg.time.Clock()

        glClearColor(0.0, 0.0, 0.0, 1.0)
        glEnable(GL_PROGRAM_POINT_SIZE)

        self.shader = self.createShader("shaders/vecShader.fs", "shaders/fragShader.fs")
        glUseProgram(self.shader)
        self.mesh = Circle()

        self.mainloop()

    def createShader(self, vertexShaderPath, fragmentShaderPath):
        with open(vertexShaderPath) as vs:
            vertexShaderSource = vs.read()

        with open(fragmentShaderPath) as fs:
            fragmentShaderSource = fs.read()

        print("Compiling shaders...")

        shader = compileProgram(
            compileShader(vertexShaderSource, GL_VERTEX_SHADER),
            compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER)
        )

        print("Shaders compiled.")

        return shader
    def mainloop(self):
        running = True

        while running:
            for event in pg.event.get():
                if event.type == pg.QUIT :
                    running = False
            
            glClear(GL_COLOR_BUFFER_BIT)

            glUseProgram(self.shader)
            glBindVertexArray(self.mesh.vao)
            glDrawArrays(GL_POINTS, 0, self.mesh.vertex_count)

            pg.display.flip() # swap buffers ig?

            self.clock.tick(60)
        self.quit()

    def quit(self):

        self.mesh.destroy()
        glDeleteProgram(self.shader)
        pg.quit()


if __name__ == "__main__":
    xc = int(input("Enter circle center x: "))
    yc = int(input("Enter circle center y: "))
    r = int(input("Enter circle radius: "))
    with open('ptc.tmp', 'wb') as f:
        pickle.dump((xc, yc, r), f)
    myApp = App()
