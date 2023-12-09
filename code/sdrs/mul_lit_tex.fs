#version 330 core
layout (location = 0) out vec3 Position;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 FragColor;
layout (location = 3) out vec4 Specolor;
layout (location = 4) out uvec4 obj_ptr;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Material material;
uniform uint object_ptr_h;
uniform uint object_ptr_l;

void main()
{    
    FragColor = texture(material.diffuse, TexCoords);
    FragColor.a = 0.5f;
    Specolor = texture(material.specular, TexCoords);
    Specolor.a = 0.5f;
    Position = FragPos;
    gNormal.rgb = normalize(Normal);
    gNormal.a = material.shininess;
    obj_ptr = uvec4(object_ptr_h,object_ptr_l,0,0);
}
