#version 410

out vec4 outputColor;

uniform float loopDuration;
uniform float elapsedTime;

const vec4 firstColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 secondColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);

void main()
{
    float currTime = mod(elapsedTime, loopDuration);
    float currLerp = currTime / loopDuration;
    
    outputColor = mix(firstColor, secondColor, currLerp);
}
