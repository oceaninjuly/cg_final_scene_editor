#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;                                             
layout (location = 2) in vec3 Normal;                                               
                                                                                    
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;                                                              
                                                                                    
out vec2 TexCoord0;                                                                 
out vec3 Normal0;                                                                   
out vec3 WorldPos0;                                                              
                                                                                    
void main()                                                                         
{                                                                                   
    TexCoord0 = TexCoord;    
    WorldPos0 = vec3(model * vec4(Position, 1.0));
    Normal0 = mat3(transpose(inverse(model))) * Normal;  
    gl_Position = projection * view * model * vec4(Position, 1.0);                              
}