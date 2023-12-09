#ifndef RENDER_H
#define RENDER_H
#define GLM_SWIZZLE
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera/camera.h>
#include<Shader/shader.h>

#include<vector>
#include"shadow.h"
#include"light.h"
#include"common.h"

class Render
{
private:
    Light& main_light;
    Shadow_Frame_Buffer& shadowfb;
public:
    Render(Light& main_light, Shadow_Frame_Buffer& shadowfb): main_light(main_light), shadowfb(shadowfb){}
    
    void DrawShadowMap(Shader& shadowShader){
        glm::vec3 LightDir = main_light.GetDirLightDirection() * 10.0f;
        glm::vec3 DivPos = camera.Position;
        DivPos.z -= 20.f;
        GLfloat near_plane = 1.0f, far_plane = 1000.0f;
        shadowfb.lightProjection = glm::ortho(-50.0f, 50.0f, -80.0f, 20.0f, near_plane, far_plane);
        shadowfb.lightView = glm::lookAt(DivPos - LightDir, DivPos, glm::vec3(0.0f, 1.0f, 0.0f));
        glViewport(0, 0, shadowfb.SHADOW_WIDTH, shadowfb.SHADOW_HEIGHT);
        shadowfb.bindFrameBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);
        
        for (int i = 0; i < Model_List.size(); i++) {
            Model_List[i].Draw(shadowShader, camera.Position, shadowfb.lightProjection, shadowfb.lightView);
        }

        shadowfb.unbindFrameBuffer();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }



};




#endif