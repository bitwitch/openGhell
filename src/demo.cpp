#include "graphics.h" 

GLuint shader_program;

GLuint uniform_elapsed_time;
GLuint uniform_v_offset;

GLuint uniform_perspective_matrix;
float perspective_matrix[16];
const float frustum_scale = 1.0f;

GLuint vbo;
GLuint vao;
GLuint vao_wedge1, vao_wedge2;

GLuint element_array_object;

const int vertex_count = 36;

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(array[0]))

#define RIGHT_EXTENT 0.8f
#define LEFT_EXTENT -RIGHT_EXTENT
#define TOP_EXTENT 0.20f
#define MIDDLE_EXTENT 0.0f
#define BOTTOM_EXTENT -TOP_EXTENT
#define FRONT_EXTENT -1.25f
#define REAR_EXTENT -1.75f

#define GREEN_COLOR 0.75f, 0.75f, 1.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.5f, 0.0f, 1.0f
#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR 0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const float vertex_data[] = {
	//Object 1 positions
	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,

	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	//Object 2 positions
	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,

	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,

	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
					
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,
					
	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,

	//Object 1 colors
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,

	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,

	RED_COLOR,
	RED_COLOR,
	RED_COLOR,

	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,

	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,

	//Object 2 colors
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,

	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,

	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,

	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,

	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,
};

const GLshort vertex_indices[] =
{
	0, 2, 1,
	3, 2, 0,

	4, 5, 6,
	6, 7, 4,

	8, 9, 10,
	11, 13, 12,

	14, 16, 15,
	17, 16, 14,
};


void init()
{
// Initialize gpu program
	std::vector<GLuint> shaderList;
	shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "shader.vert"));
	shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "shader.frag"));
	shader_program = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

// Initialize uniforms
    glUseProgram(shader_program);
    uniform_elapsed_time = glGetUniformLocation(shader_program, "elapsed_time");
    uniform_v_offset = glGetUniformLocation(shader_program, "v_offset");
    uniform_perspective_matrix = glGetUniformLocation(shader_program, "perspective_matrix");

    float z_near = 0.5f, z_far = 3.0f;

	memset(perspective_matrix, 0, sizeof(float) * 16);

	perspective_matrix[0] = frustum_scale;
	perspective_matrix[5] = frustum_scale;
	perspective_matrix[10] = (z_far + z_near) / (z_near - z_far);
	perspective_matrix[14] = (2 * z_far * z_near) / (z_near - z_far);
	perspective_matrix[11] = -1.0f;

    glUniformMatrix4fv(uniform_perspective_matrix, 1, GL_FALSE, perspective_matrix);

// Initialize Vertex Buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

// Initialize index array buffer
    glGenBuffers(1, &element_array_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), 
        vertex_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

// Initialize Vertex Array Objects
	glGenVertexArrays(1, &vao_wedge1);
	glBindVertexArray(vao_wedge1);

    size_t offset_color = sizeof(float) * 3 * vertex_count;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)offset_color);
    // this call also updates vao state
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_object); 

	glGenVertexArrays(1, &vao_wedge2);
	glBindVertexArray(vao_wedge2);

    size_t offset_position = sizeof(float) * 3 * vertex_count / 2;
    offset_color += sizeof(float) * 4 * vertex_count / 2;

    // use same vbo 
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset_position);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)offset_color);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_object); 


// Set opengl parameters
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader_program);

    // pass updated uniform data
    glUniform1f(uniform_elapsed_time, glfwGetTime()); 

    // draw wedge1
    glBindVertexArray(vao_wedge1);
    glUniform3f(uniform_v_offset, 0.0f, 0.0f, 0.0f);
    glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);

    // draw wedge2
    glBindVertexArray(vao_wedge2);
    glUniform3f(uniform_v_offset, 0.0f, 0.0f, -1.0f);
    glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
}

void window_resize_callback(GLFWwindow* window, int width, int height)
{
    perspective_matrix[0] = frustum_scale / (width / (float)height);
	perspective_matrix[5] = frustum_scale;

	glUseProgram(shader_program);
	glUniformMatrix4fv(uniform_perspective_matrix, 1, GL_FALSE, perspective_matrix);
	glUseProgram(0);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}
