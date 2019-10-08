#include "graphics.h" 

GLuint theProgram;
GLuint uniElapsedTime;
GLuint vbo;
GLuint vao;

GLuint sampler;
GLubyte *textureData;
const int texUnit = 0;

// fullscreen
const float vertexData[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,

    -1.0f, -1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f
};

void init()
{
	// Initialize gpu program
	std::vector<GLuint> shaderList;
	shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "shader.vert"));
	shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "shader.frag"));
	theProgram = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	// Initialize Vertex Buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Initialize uniforms
    GLuint uniLoopDuration = glGetUniformLocation(theProgram, "loopDuration");
    glUseProgram(theProgram);
    glUniform1f(uniLoopDuration, 5.0f);
    uniElapsedTime = glGetUniformLocation(theProgram, "elapsedTime");

    // Initialize texture
    GLuint texture;
    textureData = new GLubyte[100];    
    memset(textureData, 255, 100);
    printf("middle check: %d\n", textureData[50]);

    GLuint uniTexture = glGetUniformLocation(theProgram, "theTexture");
	glUseProgram(theProgram);
	glUniform1i(uniTexture, texUnit);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_1D, texture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R8, 100, 0,
		GL_RED, GL_UNSIGNED_BYTE, textureData);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_1D, texture); 

    glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindSampler(texUnit, sampler);

    // bind vertex array object to context
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    // set culling options
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(theProgram);

    // pass uniform data
    glUniform1f(uniElapsedTime, glfwGetTime()); 

    // pass vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);   
}
