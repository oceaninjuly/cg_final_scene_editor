#version 330
layout (location = 0) out vec3 Position;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 FragColor;
layout (location = 3) out vec4 Specolor;
layout (location = 4) out uvec4 obj_ptr;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};      

uniform Material material;
uniform uint object_ptr_h;
uniform uint object_ptr_l;


void main()                                                                                 
{                                                                                           
    FragColor = texture(material.diffuse, TexCoord0);
    FragColor.a = 0.5f;
    Specolor = texture(material.specular, TexCoord0);
    Specolor.a = 0.5f;
    Position = WorldPos0;
    gNormal.rgb = normalize(Normal0);
    gNormal.a = 32.0f;
    obj_ptr = uvec4(object_ptr_h,object_ptr_l,0,0);
    
}