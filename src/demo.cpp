#include "graphics.h" 

GLuint theProgram;
GLuint vbo;
GLuint vao;

const float vertexData[] = {
     0.0f,    0.5f, 0.0f, 1.0f,
     0.5f, -0.366f, 0.0f, 1.0f,
    -0.5f, -0.366f, 0.0f, 1.0f,
     1.0f,    0.0f, 0.0f, 1.0f,
     0.0f,    1.0f, 0.0f, 1.0f,
     0.0f,    0.0f, 1.0f, 1.0f,
};

void init()
{
	// Initialize gpu program
    {
	std::vector<GLuint> shaderList;
	shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "color.vert"));
	shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "color.frag"));
	theProgram = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
    }

	// Initialize Vertex Buffer
    {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // bind vertex array object to context
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)48);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);
}

