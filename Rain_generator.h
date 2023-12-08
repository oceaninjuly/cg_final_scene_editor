#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader/shader.h>

struct RainModel {
    Shader shader;
    GLuint VAO;
    std::vector<float> vertices;
    RainModel() :shader("./sdrs/rain/rain.vs", "./sdrs/rain/rain.fs", "./sdrs/rain/rain.gs"), VAO(0) {
        // ���ö�������
        srand(static_cast<unsigned>(time(nullptr)));

        // ��X��Y�᷶Χ����������ĳ�ʼλ�ú�ƫ��ֵ
        for (int i = 0; i < 100; ++i)
        {
            float x = static_cast<float>(rand()) / RAND_MAX * 2.0 - 1.0; // X�᷶ΧΪ[-1, 1]
            float y = static_cast<float>(rand()) / RAND_MAX * 2.0 - 1.0; // Y�᷶ΧΪ[-1, 1]
            float offset = static_cast<float>(rand()) / RAND_MAX * 2.0 - 1.0; // Z��ƫ�Ʒ�ΧΪ[-1, 1]
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(offset);
        }

        GLuint _VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    void Draw(glm::mat4 projection, glm::mat4 view) {
        shader.use();
        float currentTime = static_cast<float>(glfwGetTime());
        shader.setFloat("time", currentTime);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, vertices.size() / 3);
    }
};
