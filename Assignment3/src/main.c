#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Shader source strings
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main() {\n"
"    gl_PointSize = 3.0;\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"}\n";

// Convert screen to NDC
void screenToNDC(int x, int y, float* fx, float* fy, int width, int height) {
    *fx = (2.0f * x) / width - 1.0f;
    *fy = 1.0f - (2.0f * y) / height;
}

// Add a point to a float buffer
void addPoint(float* buffer, int* index, int x, int y, int width, int height) {
    float fx, fy;
    screenToNDC(x, y, &fx, &fy, width, height);
    buffer[(*index)++] = fx;
    buffer[(*index)++] = fy;
}

// Midpoint Circle Algorithm
void midpointCircle(float* buffer, int* index, int xc, int yc, int r, int width, int height) {
    int x = 0, y = r;
    int p = 1 - r;

    while (x <= y) {
        addPoint(buffer, index, xc + x, yc + y, width, height);
        addPoint(buffer, index, xc - x, yc + y, width, height);
        addPoint(buffer, index, xc + x, yc - y, width, height);
        addPoint(buffer, index, xc - x, yc - y, width, height);
        addPoint(buffer, index, xc + y, yc + x, width, height);
        addPoint(buffer, index, xc - y, yc + x, width, height);
        addPoint(buffer, index, xc + y, yc - x, width, height);
        addPoint(buffer, index, xc - y, yc - x, width, height);

        if (p < 0)
            p += 2 * x + 3;
        else {
            p += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

// Bresenham’s Line Algorithm
void bresenhamLine(float* buffer, int* index, int x0, int y0, int x1, int y1, int width, int height) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        addPoint(buffer, index, x0, y0, width, height);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

// Shader compiler
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        fprintf(stderr, "Shader error: %s\n", log);
        exit(1);
    }
    return shader;
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "Midpoint Circle + Bresenham Line", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_PROGRAM_POINT_SIZE);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    float points[20000]; // enough for all pixels
    int index = 0;

    // Structure: circle + cross lines
    midpointCircle(points, &index, 400, 300, 100, width, height);
    bresenhamLine(points, &index, 600, 600, 700, 300, width, height); // horizontal
    bresenhamLine(points, &index, 800, 800, 700, 300, width, height); // vertical

    // OpenGL buffer setup
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, index * sizeof(float), points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, index / 2);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
