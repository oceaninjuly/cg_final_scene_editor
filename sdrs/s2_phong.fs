#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 FragPosInLightSpace;
in float visibility;

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

float Specular_strengh=0.1f;
float shininess = 16.0f;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D Specolor;
uniform usampler2D objid;
uniform sampler2D shadowMap;//-5

uniform float near_plane;
uniform float far_plane;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform int num_lights;

uniform uint target_obj_h;
uniform uint target_obj_l;
// uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCaculation(vec4 fragPosLightSpace);
float LinearizeDepth(float depth);

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
    float depthValue = texture(shadowMap, TexCoords).r;
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    float shadow = ShadowCaculation(FragPosInLightSpace);
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir, shadow) * 1.2f;
    
    // phase 2: point lights
    for(int i = 0; i < num_lights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    
    // phase 3: spot light
    result += spotLight.open * CalcSpotLight(spotLight, norm, FragPos, viewDir);    

    // result = vec3(LinearizeDepth(depthValue) / far_plane);
    // result = vec3(depthValue, 1.0);

    // vec3 color = Diffuse;
    // vec3 lightColor = vec3(0.4);
    // // Ambient
    // vec3 ambient = 0.2 * color;
    // // Diffuse
    // vec3 lightDir = normalize(lightPos - FragPos);
    // float diff = max(dot(lightDir, norm), 0.0);
    // vec3 diffuse = diff * lightColor;
    // // Specular
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    // vec3 specular = spec * lightColor;    
    // shadow = min(shadow, 0.75);
    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;   

    FragColor = vec4(result, 1.0);
    if((obj_ptr.x != 0u || obj_ptr.y != 0u)&&(obj_ptr.x == target_obj_h && obj_ptr.y == target_obj_l)) FragColor += vec4(0.2,0.2,0.2,1.0);
}

float LinearizeDepth(float depth){
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow)
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
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * Diffuse;  
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
    vec3 diffuse = 0.5 * light.diffuse * diff * Diffuse;
    vec3 specular = 0.5 * light.specular * spec * Specular;
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
    vec3 diffuse = light.diffuse * diff * Diffuse;
    vec3 specular = light.specular * spec * Specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

float ShadowCaculation(vec4 fragPosLightSpace)
{
    // 对顶点坐标执行透视除法，将其从齐次坐标变换为投影坐标
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0, 1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    float bias = 0.0001;
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x){
        for(int y = -1; y <= 1; ++y){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;      
        }
    }

    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}   