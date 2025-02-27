#version 330 core
layout (location = 0) out vec3 Position;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 FragColor;
layout (location = 3) out vec4 Specolor;
layout (location = 4) out uvec4 obj_ptr;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform uint object_ptr_h;
uniform uint object_ptr_l;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
    FragColor.a = 0.5f;
    Specolor = texture(texture_specular1, TexCoords);
    Specolor.a = 0.5f;
    Position = FragPos;
    gNormal.rgb = normalize(Normal);
    gNormal.a = 32.0f;
    obj_ptr = uvec4(object_ptr_h,object_ptr_l,0,0);
}