#version 330 core

layout (location = 0) in vec3 aPos;

uniform float time;
uniform mat4 projection;
uniform mat4 view;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    float offset1 = rand(vec2(aPos.y + time, time)) * 0.5;
    float new_y = aPos.y + offset1 > 1.3 ? -1.0 : aPos.y + offset1;

    float offset2 = rand(vec2(aPos.x + time, time)) * 0.5;
    float new_x = aPos.x + offset2 > 1.3 ? -1.0 : aPos.x + offset2;

    gl_Position = vec4(new_x, new_y, aPos.z, 1.0);
}