#version 410

uniform sampler1D theTexture;

out vec4 outputColor;

void main()
{
    float texVal = texture(theTexture, 40).r;
    outputColor = vec4(0.0, texVal, 0.0, 1.0); 
}
