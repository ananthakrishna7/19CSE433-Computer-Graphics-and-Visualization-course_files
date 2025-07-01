#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Shader sources
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main() {\n"
    "   gl_PointSize = 10.0;\n"
    "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\n";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(0.1, 1.0, 0.2, 1.0);\n"
    "}\n";

// Convert screen coordinates to NDC
void screenToNDC(int x, int y, float* fx, float* fy, int width, int height) {
    *fx = (2.0f * x) / width - 1.0f;
    *fy = 1.0f - (2.0f * y) / height;
}

// Generate Bresenham stepping line in NDC space
int generateLineNDC(float* points, int x0, int y0, int x1, int y1, int width, int height, int stepInterval) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int count = 0;
    int step = 0;

    while (1) {
        if (step % stepInterval == 0) {
            float fx, fy;
            screenToNDC(x0, y0, &fx, &fy, width, height);
            points[2 * count] = fx;
            points[2 * count + 1] = fy;
            count++;
        }

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }

        step++;
    }

    return count;
}

// Apply mirroring to points in-place
void mirrorPoints(float* dst, const float* src, int count, int mirrorX, int mirrorY) {
    for (int i = 0; i < count; ++i) {
        dst[2 * i] = mirrorX ? -src[2 * i]     : src[2 * i];
        dst[2 * i + 1] = mirrorY ? -src[2 * i + 1] : src[2 * i + 1];
    }
}

// Shader compilation
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
        exit(EXIT_FAILURE);
    }
    return shader;
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int width = 800, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "Mirrored Bresenham Lines", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load GLAD\n");
        return -1;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    // Compile shaders
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs); glDeleteShader(fs);
    glUseProgram(shaderProgram);

    // Buffers for original + 3 mirrored lines
    float originalPoints[10000];
    float mirroredX[10000];
    float mirroredY[10000];
    float mirroredXY[10000];

    // Line: from (100, 100) to (700, 300)
    int stepInterval = 5;
    int pointCount = generateLineNDC(originalPoints, 100, 100, 700, 300, width, height, stepInterval);

    mirrorPoints(mirroredX, originalPoints, pointCount, 1, 0);  // across Y-axis
    mirrorPoints(mirroredY, originalPoints, pointCount, 0, 1);  // across X-axis
    mirrorPoints(mirroredXY, originalPoints, pointCount, 1, 1); // across origin

    // Create VAO + 4 VBOs
    GLuint VAO, VBOs[4];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(4, VBOs);
    glBindVertexArray(VAO);

    float* allLines[4] = { originalPoints, mirroredX, mirroredY, mirroredXY };
    for (int i = 0; i < 4; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, pointCount * 2 * sizeof(float), allLines[i], GL_STATIC_DRAW);
    }

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        for (int i = 0; i < 4; ++i) {
            glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_POINTS, 0, pointCount);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(4, VBOs);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
