#include "graphics.h" 

GLuint shader_program;

GLuint uniform_elapsed_time;

GLuint uniform_perspective_matrix;
float perspective_matrix[16];

GLuint uniform_model_to_camera_matrix;
GLuint uniform_camera_to_clip_matrix;

GLuint vbo;
GLuint vao;

GLuint element_array_object;

const int vertex_count = 8;

glm::mat4 camera_to_clip_matrix(0.0f);

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(array[0]))

#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f
#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR 0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const float vertex_data[] =
{
	 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,
};

const GLshort vertex_indices[] =
{
	0, 1, 2,
	1, 0, 3,
	2, 3, 0,
	3, 2, 1,

	5, 4, 6,
	4, 5, 7,
	7, 6, 4,
	6, 7, 5,
};

float calculate_frustum_scale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}

float frustum_scale = calculate_frustum_scale(45.0f);

glm::vec3 offset_stationary(float elapsed_time)
{
	return glm::vec3(0.0f, 0.0f, -20.0f);
}

glm::vec3 offset_oval(float elapsed_time)
{
	const float loop_duration = 3.0f;
	const float scale = 3.14159f * 2.0f / loop_duration;

	float time_through_loop = fmodf(elapsed_time, loop_duration);

	return glm::vec3(cosf(time_through_loop * scale) * 4.f,
		sinf(time_through_loop * scale) * 6.f,
		-20.0f);
}

glm::vec3 offset_bottom_circle(float elapsed_time)
{
	const float loop_duration = 12.0f;
	const float scale = 3.14159f * 2.0f / loop_duration;

	float time_through_loop = fmodf(elapsed_time, loop_duration);

	return glm::vec3(cosf(time_through_loop * scale) * 5.f,
		-3.5f,
		sinf(time_through_loop * scale) * 5.f - 20.0f);
}

struct Instance
{
	typedef glm::vec3(*Offset_Func)(float);

	Offset_Func calculate_offset;

	glm::mat4 construct_matrix(float elapsed_time)
	{
		glm::mat4 result(1.0f);
		result[3] = glm::vec4(calculate_offset(elapsed_time), 1.0f);
		return result;
	}
};


Instance instances[] =
{
	{ offset_stationary },
	{ offset_oval },
	{ offset_bottom_circle },
};

unsigned int num_instances = ARRAY_COUNT(instances);

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
    uniform_model_to_camera_matrix = glGetUniformLocation(shader_program, "model_to_camera_matrix");
    uniform_camera_to_clip_matrix = glGetUniformLocation(shader_program, "camera_to_clip_matrix");

    float z_near = 1.0f, z_far = 45.0f;

	camera_to_clip_matrix[0].x = frustum_scale;
	camera_to_clip_matrix[1].y = frustum_scale;
	camera_to_clip_matrix[2].z = (z_far + z_near) / (z_near - z_far);
	camera_to_clip_matrix[2].w = -1.0f;
	camera_to_clip_matrix[3].z = (2 * z_far * z_near) / (z_near - z_far);

    glUniformMatrix4fv(uniform_camera_to_clip_matrix, 1, GL_FALSE, glm::value_ptr(camera_to_clip_matrix));

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
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    size_t offset_color = sizeof(float) * 3 * vertex_count;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)offset_color);
    // this call also updates vao state
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
    glBindVertexArray(vao);

    float elapsed_time = glfwGetTime();

    // pass updated uniform data
    glUniform1f(uniform_elapsed_time, elapsed_time); 

    int i;
    for(i = 0; i < num_instances; i++)
	{
		Instance &cur_inst = instances[i];
		const glm::mat4 &transform_matrix = cur_inst.construct_matrix(elapsed_time);

		glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(transform_matrix));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
	}

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
