#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<Shader/shader.h>

struct Axismodel{
	Shader shader;
    float points[2] = {
    //0.8f, -0.8f  // сроб
    0.0f,0.0f
    };
    GLuint VAO;
    Axismodel():shader(s_path + "Axis/a.vs", s_path + "Axis/a.fs", s_path + "Axis/a.gs"),VAO(0){
        GLuint _VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glBindVertexArray(0);
    }
    void Draw(glm::mat4 projection, glm::mat4 view) {
        shader.use();
        glm::mat4 tmpmat = projection * view;
        glm::vec3 Axis_X = glm::vec3(tmpmat * glm::vec4(1, 0, 0, 0)),
            Axis_Y = glm::vec3(tmpmat * glm::vec4(0, 1, 0, 0)),
            Axis_Z = glm::vec3(tmpmat * glm::vec4(0, 0, 1, 0));
        shader.setVec2("axis_X", glm::vec2(glm::normalize(Axis_X)));
        shader.setVec2("axis_Y", glm::vec2(glm::normalize(Axis_Y)));
        shader.setVec2("axis_Z", glm::vec2(glm::normalize(Axis_Z)));
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS,0,1);
    }
};