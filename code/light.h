#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader/shader.h>
#include <vector>
#include <string>

#include"common.h"

struct DirLight {
    glm::vec3 direction;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct _PointLight {
    glm::vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class Light
{
private:
    DirLight parallel_light;
    std::vector<_PointLight> point_lights;

    public:
        Light(DirLight parallel_light):parallel_light(parallel_light){}

        void AddPointLight(_PointLight light){
            point_lights.push_back(light);
        }

        void set_light(Shader& deffered_shader) {
            deffered_shader.setInt("gPosition", 0);
            deffered_shader.setInt("gNormal", 1);
            deffered_shader.setInt("gAlbedoSpec", 2);
            deffered_shader.setInt("Specolor", 3);
            deffered_shader.setInt("objid", 4);
            deffered_shader.setInt("shadowMap ", 5);
            deffered_shader.setUint("target_obj_l", (GLuint)target_obj);
            deffered_shader.setUint("target_obj_h", (GLuint)((ull)target_obj >> 32));
            deffered_shader.setFloat("material.shininess", 32.0f);
            
            // directional light
            deffered_shader.setVec3("dirLight.direction", parallel_light.direction);
            deffered_shader.setVec3("dirLight.ambient", parallel_light.ambient);
            deffered_shader.setVec3("dirLight.diffuse", parallel_light.diffuse);
            deffered_shader.setVec3("dirLight.specular", parallel_light.specular);
            
            // point light
            deffered_shader.setInt("num_lights", point_lights.size());
            for (int i = 0; i < point_lights.size(); i++) {
                
                std::string tmp = "pointLights[" + std::to_string(i) + "]";
                deffered_shader.setVec3(tmp + ".position", point_lights[i].position);
                deffered_shader.setFloat(tmp + ".constant", point_lights[i].constant);
                deffered_shader.setFloat(tmp + ".linear", point_lights[i].linear);
                deffered_shader.setFloat(tmp + ".quadratic", point_lights[i].quadratic);
                deffered_shader.setVec3(tmp + ".ambient", point_lights[i].ambient);
                deffered_shader.setVec3(tmp + ".diffuse", point_lights[i].diffuse);
                deffered_shader.setVec3(tmp + ".specular", point_lights[i].specular);
            }

            // spotLight
            deffered_shader.setFloat("spotLight.open", torch_open);
            deffered_shader.setVec3("spotLight.position", camera.Position);
            deffered_shader.setVec3("spotLight.direction", camera.Front);
            deffered_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            deffered_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            deffered_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            deffered_shader.setFloat("spotLight.constant", 1.0f);
            deffered_shader.setFloat("spotLight.linear", 0.09f);
            deffered_shader.setFloat("spotLight.quadratic", 0.032f);
            deffered_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            deffered_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

            deffered_shader.setVec3("viewPos", camera.Position);
        }

        glm::vec3 GetDirLightDirection(){
            return parallel_light.direction;
        }

        unsigned int numofPointLight(){
            return point_lights.size();
        }

        _PointLight* getPointLight(unsigned int idx){
            return &(point_lights[idx]);
        }

        DirLight* getDirectionLight(){
            return &parallel_light;
        }

};



#endif LIGHT_H