#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 6) out;

out vec3 fColor;

uniform vec2 axis_X;
uniform vec2 axis_Y;
uniform vec2 axis_Z;

void main() {
    fColor = vec3(1.0,0.0,0.0);
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.1*axis_X,0.0,0.0);
    EmitVertex();
    EndPrimitive();

    fColor = vec3(0.0,1.0,0.0);
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.1*axis_Y,0.0,0.0);
    EmitVertex();
    EndPrimitive();

    fColor = vec3(0.0,0.0,1.0);
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.1*axis_Z,0.0,0.0);
    EmitVertex();
    EndPrimitive();
}