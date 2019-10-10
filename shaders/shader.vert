#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

uniform float elapsed_time;
uniform vec3 v_offset;
uniform mat4 perspective_matrix;

smooth out vec4 frag_color;

void main()
{
    vec4 camera_pos = position + vec4(v_offset.x, v_offset.y, v_offset.z, 0.0);

	gl_Position = perspective_matrix * camera_pos;
	frag_color = color;
}
