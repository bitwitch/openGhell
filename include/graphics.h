#include <algorithm>
#include <vector>
#include <stack>
#include <string>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint LoadShader(GLenum eShaderType, const std::string &strShaderFilename);
GLuint CreateProgram(const std::vector<GLuint> &shaderList);

void init();
void display();
void window_resize_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
