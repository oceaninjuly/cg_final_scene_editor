#pragma once
#include"vertex.h"
#include"obj.h"
#include<Shader/shader.h>
#include<vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
struct Lightmdl;
BaseModelObj *lightshadermdl=nullptr;

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
        ):Object(offset,lightshadermdl),ambient(amb),diffuse(dif),
            specular(spe),gamma(gam)
    {
        scalemat = glm::scale(scalemat,glm::vec3(0.2f));
    };
};

struct Lightmdl:public BaseModelObj{
    Lightmdl():BaseModelObj("sdrs/1.light_cube.vs","sdrs/1.light_cube.fs"){
        VAO = create_modelmat1();
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
};
