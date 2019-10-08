#include <algorithm>
#include <vector>
#include <string>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint LoadShader(GLenum eShaderType, const std::string &strShaderFilename);
GLuint CreateProgram(const std::vector<GLuint> &shaderList);

void init();
void display();
void window_resize_callback(GLFWwindow* window, int width, int height);

