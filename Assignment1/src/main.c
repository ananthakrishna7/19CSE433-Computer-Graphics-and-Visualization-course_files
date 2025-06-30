#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
// char * readShaderFile(char* filename);

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main()\n"
    "{\n"
    "   "
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

float* vertices;
int x1 = 5;
int Y1 = 6;
int x2 = 8;
int y2 = 12;

// dda algorithm
int populateVertices()
{
  // someone call free() on this monstrosity please.
  int dx = x2 - x1;
  int dy = y2 - Y1;
  int x = x1;
  int y = Y1;
  int m = dy/dx;
  int n = abs(dy)>abs(dx)?abs(dy):abs(dx);
  vertices = malloc(sizeof(int)*2*(n + 1));
  // to colour that pixel, store in array
  vertices[0] = x1;
  vertices[1] = Y1;
  int xinc = dx/n;
  int yinc = dy/n; 
  for (int i = 1; i <= n; i++)
  {
    x += xinc;
    y += yinc;
    vertices[2*i] = x;
    vertices[2*i + 1] = y;
  }
  return n;
}

int main(void) {
  GLFWwindow* window;

  // init glfw
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window
  window = glfwCreateWindow(800, 600, "Hello ma'am", NULL, NULL); // arg 4 is monitor number for fullscreen mode, NULL for windowed mode
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  // make window context current context
  glfwMakeContextCurrent(window);

  // check glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    fprintf(stderr, "Failed to initialize GLAD\n");
    return -1;
  }

  // set viewport for drawing
  glViewport(0, 0, 800, 600);

  // set framebuffer resize callback
  // this will be called whenever the window is resized
  // it sets the viewport to the new width and height of the window
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // triangle coordinates
  // float vertices[] = {
  //   -0.5f, -0.5f, 0.0f,
  //    0.5f, -0.5f, 0.0f,
  //    0.0f,  0.5f, 0.0f
  // };
  
  int n = populateVertices();
 
  // vertex shader
  // const char* vertexShaderSource = readShaderFile("vertexShader.vert");
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // check compile success
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::VERTEX SHADER NOT COMPILED. INFO LOG:\n%s", infoLog);
  }

  // fragment shader
  // const char* fragmentShaderSource = readShaderFile("fragmentShader.frag");
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // check compile success
  // int success;
  // char infoLog[512];
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::FRAGMENT SHADER NOT COMPILED. INFO LOG:\n%s", infoLog);
  }

  // shader program
  GLuint shaderProgram;
  shaderProgram = glCreateProgram();
  // attach and link
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("ERROR::SHADER PROGRAM NOT LINKED. INFO LOG:\n%s", infoLog);
  }
  
  // clean up
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  // free(vertexShaderSource);
  // free(fragmentShaderSource);

  GLuint VBO, VAO; // vertex buffer object, vertex array object
  glGenBuffers(1, &VBO); // holds vertex data
  glGenVertexArrays(1, &VAO); // holds vertex config

  glBindVertexArray(VAO); // bind the VAO, all subsequent calls will affect this VAO

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // tell opengl what is there in the vertex data, GL_FALSE -> GL_TRUE for int data
  glVertexAttribPointer(0, 2, GL_INT, GL_TRUE, 2 * sizeof(int), (void*) 0); // arg 1 corresponds to the layout(location = 0) in the vertex shader -> position attribute
  glEnableVertexAttribArray(0);
  glBindVertexArray(0); // unbind. unneccesary here, but it seems like a good practice
  
  // main loop
  while (!glfwWindowShouldClose(window))
  {
    // process input
    processInput(window);

    // rendering code

    // FIRST clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // bluish green color
    glClear(GL_COLOR_BUFFER_BIT);

    // draw stuff here
    glUseProgram(shaderProgram); // we will use this shader program hereafter
    glBindVertexArray(VAO); // bind vertex array that holds the configurations
    glDrawArrays(GL_LINE_STRIP, 0, n + 1);


    // swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents(); // poll for events
  }

  // cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
  free(vertices);

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// char * readShaderFile(char* filename)
// {
//   char* buffer; // caller must free()
//   long length;
//   FILE* file = fopen(filename, "rb");

//   if(buffer){
//     fseek(file, 0, SEEK_END);
//     length = ftell(file);
//     fseek(file, 0, SEEK_SET);
//     buffer = malloc(length);
//     if (buffer){
//       fread(buffer, 1, length, file);
//     }
//     fclose(file);
//   }

//   return buffer;
// }