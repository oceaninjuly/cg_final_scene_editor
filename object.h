#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<model.h>
#include <Shader/shader.h>
#include <camera/camera.h>

#include <iostream>
#include<vector>
#include"vertex.h"
#include"obj.h"
#include"common.h"
#include"pointlit.h"
typedef unsigned long long ull;

struct ModelObj3: public BaseModelObj{ // 无纹理的箱子
    ModelObj3():BaseModelObj("sdrs/3.mul_lit.vs","sdrs/3.mul_lit.fs"){
        VAO = create_cube();
        //VAO = create_texmodel(vertix_cube2);

        category = 3;
    }
    void render(glm::vec3 &lightPos,glm::mat4 &projection,glm::mat4 &view,glm::vec3 &viewPos){
        shader.use();
        shader.setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
        shader.setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);
        
        // camera
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // world transformation
        shader.setVec3("viewPos", viewPos);
        // std::printf("%f,%f,%f\n",viewPos.x,viewPos.y,viewPos.z);
        //render all object
        glBindVertexArray(VAO);
        for(auto ele: objlist){
            shader.setUint("object_ptr_l", (GLuint)ele);
            shader.setUint("object_ptr_h", (GLuint)((ull)ele >> 32));
            shader.setMat4("model",ele->getmodel());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
};

struct ModelObj4 : public BaseModelObj { // 有纹理的箱子
    unsigned int diffuseMap, specularMap;
    ModelObj4() :BaseModelObj("sdrs/mul_lit_tex.vs", "sdrs/mul_lit_tex.fs") {
        VAO = create_texmodel(vertix_cube2,sizeof(vertix_cube2));
        diffuseMap = gentexture("./texture/container2.png");
        specularMap = gentexture("./texture/container2_specular.png");
        shader.use();
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);

        category = 4;
    }
    void render(glm::vec3& lightPos, glm::mat4& projection, glm::mat4& view, glm::vec3& viewPos) {
        shader.use();
        shader.setFloat("material.shininess", 32.0f);
        
        // camera
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // world transformation
        shader.setVec3("viewPos", viewPos);
        // std::printf("%f,%f,%f\n",viewPos.x,viewPos.y,viewPos.z);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        //render all object
        glBindVertexArray(VAO);
        for (auto ele : objlist) {
            shader.setUint("object_ptr_l", (GLuint)ele);
            shader.setUint("object_ptr_h", (GLuint)((ull)ele >> 32));
            shader.setMat4("model", ele->getmodel());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
};

struct ModelObj1 : public BaseModelObj {
    Model ourModel;
    ModelObj1():
        BaseModelObj("./sdrs/model_loading.vs", "./sdrs/model_loading.fs"), 
        ourModel("./model/nanosuit/nanosuit.obj"){
        VAO = -1;

        category = 1;
    }
    void render(glm::vec3& lightPos, glm::mat4& projection, glm::mat4& view, glm::vec3& viewPos) {
        shader.use();
        // camera
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        //render all object
        glBindVertexArray(VAO);
        ull tmp1, tmp2;
        for (auto ele : objlist) {
            shader.setUint("object_ptr_l", (GLuint)ele);
            shader.setUint("object_ptr_h", (GLuint)((ull)ele >> 32));
            shader.setMat4("model", ele->getmodel());
            ourModel.Draw(shader);
        }
    }

    
};

