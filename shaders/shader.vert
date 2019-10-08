#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

uniform vec2 offset;
uniform sampler1D theTexture;

smooth out vec4 theColor;

void main()
{
	gl_Position = position;
    float texVal = texture(theTexture, 40).r;
    theColor = vec4(0.0, texVal, 0.0, 1.0); 
	//theColor = color;
}
