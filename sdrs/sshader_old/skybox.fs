#version 330 core
out vec4 FragColor;

in vec3 outUV;

uniform samplerCube  ourTexture;

void main()
{
    FragColor = texture(ourTexture , outUV);
};