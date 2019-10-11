#include "graphics.h" 

GLuint shader_program;

GLuint uniform_elapsed_time;

GLuint uniform_model_to_camera_matrix;
GLuint uniform_camera_to_clip_matrix;

GLuint vbo;
GLuint vao;

GLuint element_array_object;

glm::mat4 camera_to_clip_matrix(0.0f);

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(array[0]))

const int vertex_count = 24;

#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f

#define YELLOW_COLOR 1.0f, 1.0f, 0.0f, 1.0f
#define CYAN_COLOR 0.0f, 1.0f, 1.0f, 1.0f
#define MAGENTA_COLOR 	1.0f, 0.0f, 1.0f, 1.0f

const float vertex_data[] =
{
	//Front
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,

	//Top
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,

	//Left
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,

	//Back
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	//Bottom
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,

	//Right
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,


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
	RED_COLOR,

	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,

	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,

	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
};

const GLshort vertex_indices[] =
{
	0, 1, 2,
	2, 3, 0,

	4, 5, 6,
	6, 7, 4,

	8, 9, 10,
	10, 11, 8,

	12, 13, 14,
	14, 15, 12,

	16, 17, 18,
	18, 19, 16,

	20, 21, 22,
	22, 23, 20,
};

float calculate_frustum_scale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}

float frustum_scale = calculate_frustum_scale(45.0f);

inline float Deg_To_Rad(float angle_deg)
{
    return angle_deg * 3.14159f / 180.0f;
}

inline float Clamp(float value, float min, float max)
{
	if(value < min) return min;
	if(value > max) return max;
	return value;
}

glm::mat3 Rotate_X(float angle_deg)
{
	float angle_rad = Deg_To_Rad(angle_deg);
	float cosine = cosf(angle_rad);
	float sine = sinf(angle_rad);

	glm::mat3 result(1.0f);
	result[1].y = cosine; result[2].y = -sine;
	result[1].z = sine; result[2].z = cosine;
	return result;
}

glm::mat3 Rotate_Y(float angle_deg)
{
	float angle_rad = Deg_To_Rad(angle_deg);
	float cosine = cosf(angle_rad);
	float sine = sinf(angle_rad);

	glm::mat3 result(1.0f);
	result[0].x = cosine; result[2].x = sine;
	result[0].z = -sine; result[2].z = cosine;
	return result;
}

glm::mat3 Rotate_Z(float angle_deg)
{
	float angle_rad = Deg_To_Rad(angle_deg);
	float cosine = cosf(angle_rad);
	float sine = sinf(angle_rad);

	glm::mat3 result(1.0f);
	result[0].x = cosine; result[1].x = -sine;
	result[0].y = sine; result[1].y = cosine;
	return result;
}

class Matrix_Stack
{
public:
	Matrix_Stack()
		: m_currMat(1.0f)
	{
	}

	const glm::mat4 &Top()
	{
		return m_currMat;
	}

	void Rotate_X(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::Rotate_X(fAngDeg));
	}

	void Rotate_Y(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::Rotate_Y(fAngDeg));
	}

	void Rotate_Z(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::Rotate_Z(fAngDeg));
	}

	void Scale(const glm::vec3 &scaleVec)
	{
		glm::mat4 scaleMat(1.0f);
		scaleMat[0].x = scaleVec.x;
		scaleMat[1].y = scaleVec.y;
		scaleMat[2].z = scaleVec.z;

		m_currMat = m_currMat * scaleMat;
	}

	void Translate(const glm::vec3 &offsetVec)
	{
		glm::mat4 translateMat(1.0f);
		translateMat[3] = glm::vec4(offsetVec, 1.0f);

		m_currMat = m_currMat * translateMat;
	}

	void Push()
	{
		m_matrices.push(m_currMat);
	}

	void Pop()
	{
		m_currMat = m_matrices.top();
		m_matrices.pop();
	}

private:
	glm::mat4 m_currMat;
	std::stack<glm::mat4> m_matrices;
};

class Hierarchy
{
public:
	Hierarchy()
		: posBase(glm::vec3(3.0f, -5.0f, -40.0f))
		, angBase(-45.0f)
		, posBaseLeft(glm::vec3(2.0f, 0.0f, 0.0f))
		, posBaseRight(glm::vec3(-2.0f, 0.0f, 0.0f))
		, scaleBaseZ(3.0f)
		, angUpperArm(-33.75f)
		, sizeUpperArm(9.0f)
		, posLowerArm(glm::vec3(0.0f, 0.0f, 8.0f))
		, angLowerArm(146.25f)
		, lenLowerArm(5.0f)
		, widthLowerArm(1.5f)
		, posWrist(glm::vec3(0.0f, 0.0f, 5.0f))
		, angWristRoll(0.0f)
		, angWristPitch(67.5f)
		, lenWrist(2.0f)
		, widthWrist(2.0f)
		, posLeftFinger(glm::vec3(1.0f, 0.0f, 1.0f))
		, posRightFinger(glm::vec3(-1.0f, 0.0f, 1.0f))
		, angFingerOpen(180.0f)
		, lenFinger(2.0f)
		, widthFinger(0.5f)
		, angLowerFinger(45.0f)
	{}

	void Draw()
	{
		Matrix_Stack modelToCameraStack;

		glUseProgram(shader_program);
		glBindVertexArray(vao);

		modelToCameraStack.Translate(posBase);
		modelToCameraStack.Rotate_Y(angBase);

		//Draw left base.
		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(posBaseLeft);
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
			glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}

		//Draw right base.
		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(posBaseRight);
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
			glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}

		//Draw main arm.
		DrawUpperArm(modelToCameraStack);

		glBindVertexArray(0);
		glUseProgram(0);
	}

#define STANDARD_ANGLE_INCREMENT 11.25f
#define SMALL_ANGLE_INCREMENT 9.0f

	void AdjBase(bool bIncrement)
	{
		angBase += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angBase = fmodf(angBase, 360.0f);
	}

	void AdjUpperArm(bool bIncrement)
	{
		angUpperArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angUpperArm = Clamp(angUpperArm, -90.0f, 0.0f);
	}

	void AdjLowerArm(bool bIncrement)
	{
		angLowerArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angLowerArm = Clamp(angLowerArm, 0.0f, 146.25f);
	}

	void AdjWristPitch(bool bIncrement)
	{
		angWristPitch += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angWristPitch = Clamp(angWristPitch, 0.0f, 90.0f);
	}

	void AdjWristRoll(bool bIncrement)
	{
		angWristRoll += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angWristRoll = fmodf(angWristRoll, 360.0f);
	}

	void AdjFingerOpen(bool bIncrement)
	{
		angFingerOpen += bIncrement ? SMALL_ANGLE_INCREMENT : -SMALL_ANGLE_INCREMENT;
		angFingerOpen = Clamp(angFingerOpen, 9.0f, 180.0f);
	}

	void WritePose()
	{
		printf("angBase:\t%f\n", angBase);
		printf("angUpperArm:\t%f\n", angUpperArm);
		printf("angLowerArm:\t%f\n", angLowerArm);
		printf("angWristPitch:\t%f\n", angWristPitch);
		printf("angWristRoll:\t%f\n", angWristRoll);
		printf("angFingerOpen:\t%f\n", angFingerOpen);
		printf("\n");
	}

private:
	void DrawFingers(Matrix_Stack &modelToCameraStack)
	{
		//Draw left finger
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posLeftFinger);
		modelToCameraStack.Rotate_Y(angFingerOpen);

		modelToCameraStack.Push();
		modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
		modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
		glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		{
			//Draw left lower finger
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
			modelToCameraStack.Rotate_Y(-angLowerFinger);

			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
			modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
			glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();

			modelToCameraStack.Pop();
		}

		modelToCameraStack.Pop();

		//Draw right finger
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posRightFinger);
		modelToCameraStack.Rotate_Y(-angFingerOpen);

		modelToCameraStack.Push();
		modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
		modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
		glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		{
			//Draw right lower finger
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
			modelToCameraStack.Rotate_Y(angLowerFinger);

			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
			modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
			glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();

			modelToCameraStack.Pop();
		}

		modelToCameraStack.Pop();
	}

	void DrawWrist(Matrix_Stack &modelToCameraStack)
	{
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posWrist);
		modelToCameraStack.Rotate_Z(angWristRoll);
		modelToCameraStack.Rotate_X(angWristPitch);

		modelToCameraStack.Push();
		modelToCameraStack.Scale(glm::vec3(widthWrist / 2.0f, widthWrist/ 2.0f, lenWrist / 2.0f));
		glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		DrawFingers(modelToCameraStack);

		modelToCameraStack.Pop();
	}

	void DrawLowerArm(Matrix_Stack &modelToCameraStack)
	{
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posLowerArm);
		modelToCameraStack.Rotate_X(angLowerArm);

		modelToCameraStack.Push();
		modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenLowerArm / 2.0f));
		modelToCameraStack.Scale(glm::vec3(widthLowerArm / 2.0f, widthLowerArm / 2.0f, lenLowerArm / 2.0f));
		glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		DrawWrist(modelToCameraStack);

		modelToCameraStack.Pop();
	}

	void DrawUpperArm(Matrix_Stack &modelToCameraStack)
	{
		modelToCameraStack.Push();
		modelToCameraStack.Rotate_X(angUpperArm);

		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, (sizeUpperArm / 2.0f) - 1.0f));
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, sizeUpperArm / 2.0f));
			glUniformMatrix4fv(uniform_model_to_camera_matrix, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(vertex_indices), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}

		DrawLowerArm(modelToCameraStack);

		modelToCameraStack.Pop();
	}

	glm::vec3		posBase;
	float			angBase;

	glm::vec3		posBaseLeft, posBaseRight;
	float			scaleBaseZ;

	float			angUpperArm;
	float			sizeUpperArm;

	glm::vec3		posLowerArm;
	float			angLowerArm;
	float			lenLowerArm;
	float			widthLowerArm;

	glm::vec3		posWrist;
	float			angWristRoll;
	float			angWristPitch;
	float			lenWrist;
	float			widthWrist;

	glm::vec3		posLeftFinger, posRightFinger;
	float			angFingerOpen;
	float			lenFinger;
	float			widthFinger;
	float			angLowerFinger;
};


Hierarchy g_armature;

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

    g_armature.Draw();
}

void window_resize_callback(GLFWwindow* window, int width, int height)
{
    camera_to_clip_matrix[0].x = frustum_scale / (width / (float)height);
	camera_to_clip_matrix[1].y = frustum_scale;

	glUseProgram(shader_program);
	glUniformMatrix4fv(uniform_camera_to_clip_matrix, 1, GL_FALSE, glm::value_ptr(camera_to_clip_matrix));
	glUseProgram(0);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

	switch (key)
	{
	case GLFW_KEY_A: g_armature.AdjBase(true); break;
	case GLFW_KEY_D: g_armature.AdjBase(false); break;
	case GLFW_KEY_W: g_armature.AdjUpperArm(false); break;
	case GLFW_KEY_S: g_armature.AdjUpperArm(true); break;
	case GLFW_KEY_R: g_armature.AdjLowerArm(false); break;
	case GLFW_KEY_F: g_armature.AdjLowerArm(true); break;
	case GLFW_KEY_T: g_armature.AdjWristPitch(false); break;
	case GLFW_KEY_G: g_armature.AdjWristPitch(true); break;
	case GLFW_KEY_Z: g_armature.AdjWristRoll(true); break;
	case GLFW_KEY_C: g_armature.AdjWristRoll(false); break;
	case GLFW_KEY_Q: g_armature.AdjFingerOpen(true); break;
	case GLFW_KEY_E: g_armature.AdjFingerOpen(false); break;
	case GLFW_KEY_SPACE: g_armature.WritePose(); break;
	}
}



