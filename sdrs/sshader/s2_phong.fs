#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

// struct Material {
//     sampler2D diffuse;
//     sampler2D specular;
//     float shininess;
// }; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    float open;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 30

vec3 Diffuse;
vec3 Specular;
float Diffuse_strengh=1.0f;
float Specular_strengh=1.0f;
float shininess = 16.0f;

uniform uint render_style;
uniform float time;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D Specolor;
uniform usampler2D objid;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform int num_lights;
uniform uint target_obj_h;
uniform uint target_obj_l;
// uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


// 添加一个函数用于生成伪随机数，输入vec2，是为了对应纹理坐标，方便进行处理
// 参数值是经验选择的，称为噪声函数
float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}


void change_style( )
{
    if(render_style == 0u) return;
    else if(render_style == 1u)
    {
        //1.黑白风格
        // 将彩色转为灰度，可以使用标准的灰度公式：gray = 0.2126 * r + 0.7152 * g + 0.0722 * b
        float gray = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
        // 将灰度值分配给输出颜色的所有通道，得到黑白效果
        FragColor = vec4(vec3(gray), 1.0);
    }
    else if(render_style == 2u)
    {
        //2.像素风格
        // 将彩色转为离散颜色，通过乘color_num取整得到从0到color_num这么多的类型，例如取5
        // 再除以color_num（取5），将颜色分割到1，4/5，……，0，一共6个颜色区间
        // 这样实现颜色的有限化，可以降低画面复杂度，接近于油画、像素等风格
        float color_num = 5.0;
        vec3 quantizedColor = round(FragColor.rgb * color_num) / color_num;
        FragColor = vec4(quantizedColor, 1.0);
        // 添加扰动，增强像素效果
        float jitter = rand(TexCoords + 0.5) * 0.05; // 调整扰动强度
        FragColor.rgb += vec3(jitter);
    }
    else if(render_style == 3u)
    {
        //3.电视花屏
        // 这个是在2的后半段基础上做的，其实就是把扰动强度提的很高
        //添加扰动，增强像素效果
        float jitter = rand(TexCoords + time) * 0.5; // 调整扰动强度
        FragColor.rgb += vec3(jitter);
    }
    else if(render_style == 4u)
    {
        //4.下雨
        // 模拟雨滴效果
        float raindrop = rand(TexCoords*100 + time); // 调整雨滴可能性，在0-1之间分布
        // 输出雨滴效果
        if(raindrop>=0.98)
        {
           FragColor = vec4(vec3(0.0, 0.0, 1.0) * raindrop, 1.0);
        }
    }
}

void main()
{    
    // properties
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    Diffuse = texture(gAlbedoSpec, TexCoords).rgb;

    Specular = texture(Specolor, TexCoords).rgb;
    Specular_strengh = texture(Specolor, TexCoords).a;
    uvec3 obj_ptr = texture(objid,TexCoords).rgb;
    float shininess = texture(gNormal, TexCoords).a;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < num_lights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: spot light
    result += spotLight.open * CalcSpotLight(spotLight, norm, FragPos, viewDir); 

    FragColor = vec4(result, 1.0);

    //切换风格
    change_style( );

    if((obj_ptr.x != 0u || obj_ptr.y != 0u)&&(obj_ptr.x == target_obj_h && obj_ptr.y == target_obj_l)) FragColor += vec4(0.2,0.2,0.2,1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * Diffuse;
    vec3 diffuse = light.diffuse * diff * Diffuse;
    vec3 specular = light.specular * spec * Specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * Diffuse;
    vec3 diffuse = light.diffuse * diff * Diffuse * Diffuse_strengh;
    vec3 specular = light.specular * spec * Specular * Specular_strengh;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * Diffuse;
    vec3 diffuse = light.diffuse * diff * Diffuse * Diffuse_strengh;
    vec3 specular = light.specular * spec * Specular * Specular_strengh;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}