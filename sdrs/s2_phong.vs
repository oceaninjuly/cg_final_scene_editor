#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;
out vec4 FragPosInLightSpace;
out float visibility;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;

uniform mat4 lightView;
uniform mat4 lightProjection;

const float density = 0.001;
const float gradient = 1.5;

void main()
{
    gl_Position = vec4(position, 1.0f);
    TexCoords = texCoords;

    vec3 FragPos = vec3(gl_Position);
    FragPosInLightSpace = lightProjection * lightView * vec4(FragPos, 1.0f);

    vec4 CamRelativePos = view * gl_Position;
    float CamRelativeDistance = length(CamRelativePos.xyz);
    visibility = clamp(exp(-pow((CamRelativeDistance * density), gradient)), 0.0, 1.0);
}