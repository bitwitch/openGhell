#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
uniform float loopDuration;
uniform float elapsedTime;
smooth out vec4 theColor;

void main()
{
    float timeThroughLoop = mod(elapsedTime, loopDuration);
    float theta = 3.14159f * 2.0f / loopDuration * timeThroughLoop;
    vec4 offset = vec4(
        0.5f * cos(theta),
        0.5f * sin(theta),
        0.0f,
        0.0f
    );

	gl_Position = position + offset;
	theColor = color;
}
