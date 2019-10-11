#include "graphics.h"
#include <fstream>
#include <stdio.h>

const std::string SHADER_DIR = "shaders/";

// TODO(shaw): get rid of streams and stdlib shit, cause its fucking everything up
// just use the c you learned in K&R

std::string FindFileOrThrow( const std::string &strBasename )
{
    // try 'shaders' dir
    std::string strFilename = SHADER_DIR + strBasename;
    std::ifstream testFile(strFilename.c_str());
    if(testFile.is_open())
        return strFilename;
    
    // try current dir 
    strFilename = "./" + strBasename;
    testFile.open(strFilename.c_str());
    if(testFile.is_open())
        return strFilename;

    throw std::runtime_error("Could not find the file " + strBasename);
}

GLuint LoadShader(GLenum eShaderType, const std::string &strShaderFilename)
{
    std::string strFilename = FindFileOrThrow(strShaderFilename);
    
    // read shader file into a buffer
    char *buffer = NULL;
    long length;
    FILE *fp = fopen(strFilename.c_str(), "r");

    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buffer = (char *)malloc(length + 1); // for null character
        if (buffer)
        {
            fread(buffer, 1, length, fp);
            buffer[length] = '\0';
        }
        fclose(fp);
    }

    if (!buffer)
    {
        printf("Error reading shader %s\n", strFilename.c_str());
        throw;
    }

    const GLchar *source = (const GLchar *)buffer;

    GLuint shader = glCreateShader(eShaderType);
    
    glShaderSource(shader, 1, &source, 0);

    glCompileShader(shader);

    free(buffer);

    // Handle compilation error
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

 		GLchar *strInfoLog = new GLchar[maxLength + 1];
		glGetShaderInfoLog(shader, maxLength, NULL, strInfoLog);
        
        glDeleteShader(shader);

        char *strShaderType = NULL;
        switch(eShaderType)
        {
        case GL_VERTEX_SHADER:   strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", 
                strShaderType, strInfoLog);
		delete[] strInfoLog;
        throw;
    }

    return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

    // Handle link error
	GLint linkStatus;
	glGetProgramiv (program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);

        glDeleteProgram(program);
        std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

void logSecondsPerFrame(double &lastTime, int &nbFrames) 
{
    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
        // printf and reset timer
        printf("%f ms/frame\n", 1000.0/double(nbFrames));
        nbFrames = 0;
        lastTime += 1.0;
    }
}

static void 
error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE ); 

    window = glfwCreateWindow(640, 640, "Fucking Finally", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwSetFramebufferSizeCallback(window, window_resize_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    // NOTE: OpenGL error checks have been omitted for brevity

    // Application initialization
    init();

    // For logging seconds per frame
    //double lastTime = glfwGetTime();
    //int nbFrames = 0;

    while (!glfwWindowShouldClose(window))
    {
        //logSecondsPerFrame(lastTime, nbFrames);

        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

