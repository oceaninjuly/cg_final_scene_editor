#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"vertex.h"
#include"obj.h"
#include"common.h"

#include<Shader/shader.h>
#include<vector>


struct Pointlight:public Object{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 gamma;
    Pointlight(
        glm::vec3 offset,
        glm::vec3 amb=glm::vec3(0.05f, 0.05f, 0.05f),
        glm::vec3 dif=glm::vec3(0.8f, 0.8f, 0.8f),
        glm::vec3 spe=glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3 gam=glm::vec3(1.0f,0.09f,0.032f)
        ):Object(offset, lightCube_model),ambient(amb),diffuse(dif),
            specular(spe),gamma(gam)
    {
        scalemat = glm::scale(scalemat,glm::vec3(0.2f));
    };
};

struct LightCube:public BaseModelObj{
    LightCube():BaseModelObj(s_path+"light_cube.vs", s_path + "light_cube.fs"){
        VAO = create_light_cube();
    }
    void render(glm::vec3 &lightPos,glm::mat4 &projection,glm::mat4 &view,glm::vec3 &viewPos){
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glBindVertexArray(VAO);
        for(auto ele: objlist){
            shader.setMat4("model",ele->getmodel());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    unsigned int create_light_cube(){
        unsigned int _VBO,VAO;
        // first, configure the cube's VAO (and VBO)
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(light_cube), light_cube, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //normal attr
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        return VAO;
    }
};
