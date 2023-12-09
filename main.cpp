#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"object.h"
#include<IMG/ffimage.cpp>
#include <iostream>
#include<algorithm>
#include"main.h"
#include"pointlit.h"
#include"ground.h"
#include"Axis_generator.h"
#include"_mesh.h"
#include"Rain_generator.h"
//shader_model
std::vector<BaseModelObj*> shadermodel_list;
//point_light
std::vector<Pointlight*> point_lights;
//deffered_rendering
unsigned int framebuffer;
unsigned int textureColorbuffer, posbuffer, normalbuffer,specolorbuffer,objidbuffer;
Shader deffered_shader;
//Axis
Axismodel* axismodel;
//Rain
RainModel* rainmodel;
//new Mesh
_Mesh* m = new _Mesh();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
uint creatSkyBoxVAO();
uint createSkyBoxTex(std::string,std::string,std::string);
void rend();
void Deffer_setting();
void RenderQuad();
Object* get_Target_object(int winX, int winY);
glm::vec3 get_Target_world(int winX, int winY);

GLuint quadVAO = 0;
GLuint quadVBO;
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
Object* target_obj=nullptr;
Object* last_obj = nullptr;

// 全局变量，渲染风格标记和总数
uint render_style = 0;
uint render_style_number = 5;

void framebufinit() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    glGenTextures(1, &posbuffer);
    glBindTexture(GL_TEXTURE_2D, posbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posbuffer, 0);

    glGenTextures(1, &normalbuffer);
    glBindTexture(GL_TEXTURE_2D, normalbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalbuffer, 0);

    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textureColorbuffer, 0);

    glGenTextures(1, &specolorbuffer);
    glBindTexture(GL_TEXTURE_2D, specolorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, specolorbuffer, 0);

    glGenTextures(1, &objidbuffer);
    glBindTexture(GL_TEXTURE_2D, objidbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, objidbuffer, 0);

    GLenum attachments[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4};
    glDrawBuffers(5, attachments);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint creatSkyBoxVAO(){
    uint _VAO = 0;
    uint _VBO = 0;

    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return _VAO;
}

uint createSkyBoxTex(std::string type="1",std::string suffix=".png",std::string prefix="texture/skybox/")
{
    uint _tid = 0;
    glGenTextures(1, &_tid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _tid);
        //需要将合适的纹理路径按照立方体贴图枚举指定的顺序加载到一个vector中;
    std::vector<std::string> _facePath =
    {
            prefix+type+"/PosX"+suffix, //right
            prefix+type+"/NegX"+suffix, //left
            prefix+type+"/PosY"+suffix,  //top
            prefix+type+"/NegY"+suffix, //bottom
            prefix+type+"/PosZ"+suffix,  //front
            prefix+type+"/NegZ"+suffix,  //back
    };
    //为6个面做初始;
    for (int i = 0; i < 6; i++)
    {
        ffImage* _pImage = ffImage::readFromFile(_facePath[i].c_str());

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, _pImage->getWidth(), _pImage->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _pImage->getData());
        delete _pImage;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return _tid;
}

void rend(){
    // glEnable(GL_DEPTH_TEST);
    //更新观察者信息
    static float winZ;
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
    glm::mat4 view = camera.GetViewMatrix();
    //第一阶段渲染(每个像素的漫反射颜色，法向量，位置)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i=0;i < shadermodel_list.size();i++){
        // 调用shader-model 内部实现好的的render函数，传递必要参数
        shadermodel_list[i]->render(lightPos,projection,view,camera.Position);
    }
    groundshadermdl->render(lightPos, projection, view, camera.Position);
    
    //第二阶段渲染(光照)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    deffered_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, posbuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalbuffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, specolorbuffer);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, objidbuffer);
    Deffer_setting();
    RenderQuad();
    //第三阶段渲染：不属于延迟渲染管线的对象
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲
    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    
    //点光源模型
    lightshadermdl->render(lightPos,projection,view,camera.Position);
    //坐标指示器绘制
    if(ground_grid) axismodel->Draw(projection, view);
    //下雨状态绘制
    if (render_style == 4) rainmodel->Draw(projection, view);
    //渲染天空盒
    glDepthFunc(GL_LEQUAL);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureSky);



    _shader_sky.use();
    _shader_sky.setUint("render_style", render_style);
    float currentTime = static_cast<float>(glfwGetTime());
    _shader_sky.setFloat("time", currentTime);
    _shader_sky.setMat4("_viewMatrix", camera.GetSkyviewMatrix());    //设置观察矩阵；
    _shader_sky.setMat4("_projMatrix", projection);    //设置投影矩阵；
    glBindVertexArray(VAO_sky);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    
}

void Deffer_setting() {
    float currentTime = static_cast<float>(glfwGetTime());
    deffered_shader.setUint("render_style", (GLuint)render_style);
    deffered_shader.setFloat("time", currentTime);
    deffered_shader.setInt("gPosition", 0);
    deffered_shader.setInt("gNormal", 1);
    deffered_shader.setInt("gAlbedoSpec", 2);
    deffered_shader.setInt("Specolor", 3);
    deffered_shader.setInt("objid", 4);
    deffered_shader.setUint("target_obj_l", (GLuint)target_obj);
    deffered_shader.setUint("target_obj_h", (GLuint)((ull)target_obj >> 32));
    deffered_shader.setFloat("material.shininess", 32.0f);
    // directional light
    deffered_shader.setVec3("dirLight.direction", glm::normalize(glm::vec3(-1.0f, -0.3f, -0.3f)));
    deffered_shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    deffered_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    deffered_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light
    std::vector<Object*>& lit = lightshadermdl->objlist;
    deffered_shader.setInt("num_lights", lit.size());
    for (int i = 0; i < lit.size(); i++) {
        Pointlight* iter = (Pointlight*)lit[i];
        std::string tmp = "pointLights[" + std::to_string(i) + "]";
        deffered_shader.setVec3(tmp + ".position", iter->getPos());
        deffered_shader.setVec3(tmp + ".ambient", iter->ambient);
        deffered_shader.setVec3(tmp + ".specular", iter->specular);
        deffered_shader.setFloat(tmp + ".constant", iter->gamma.x);
        deffered_shader.setFloat(tmp + ".linear", iter->gamma.y);
        deffered_shader.setFloat(tmp + ".quadratic", iter->gamma.z);
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

int main(){
    /*Create_gray();
    return 0;*/
    //初始化地面模型
    Create_ground_model("./ground/t1.png", 0.1f);
    GLFWwindow* window;
    //初始化
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif
        // glfw window creation
        // --------------------
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }
        glEnable(GL_DEPTH_TEST);
    }
    //初始化framebuffer
    framebufinit();
    Shader deffered_shader_("./sdrs/sshader/s2.vs", "./sdrs/sshader/s2_phong.fs");
    deffered_shader = deffered_shader_;
    //生成天空盒(顶点对象，纹理)
    VAO_sky = creatSkyBoxVAO();
    _textureSky = createSkyBoxTex();
    _shader_sky.initialize("sdrs/sshader/skybox.vs","sdrs/sshader/skybox.fs");
    //初始化Axis
    axismodel = new Axismodel();
    //初始化RainModel
    rainmodel = new RainModel();
    //生成模型和对应着色器
    lightshadermdl = new Lightmdl();
    groundshadermdl = new Groundmdl("texture/dry_dirt.jpg");
    shadermodel_list.push_back(new ModelObj3());
    shadermodel_list.push_back(new ModelObj4());
    shadermodel_list.push_back(new ModelObj1());
    //shadermodel_list.push_back(new ModelObj6());
    //Object
    /*std::vector<Object*> Objectlist;
    std::vector<Object*> Objectlist2;
    std::vector<Object*> Objectlist3;*/
    
    groundobj = new Object(glm::vec3(27, -1, 25), groundshadermdl);//groundshadermdl
    //groundobj->scalemat = glm::scale(groundobj->scalemat, glm::vec3(0.1f));
    
    //生成物体
    /*auto ground = decode("./ground/1.jpg");
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            for(int k=0;k< ground[i][j]%15;k++)
                Objectlist2.push_back(new Object(glm::vec3(i, k, j), shadermodel_list[2]));
        }
    }*/
    /*std::vector<Object*>& ae = shadermodel_list[2]->objlist;
    for (int i = 1; i < ae.size(); i++) {
        if (ae[i - 1] >= ae[i]) {
            std::cout << i << '\n';
        }
    }*/
    //删除指定点，测试一下功能
    /*int i = 0;
    while(i<Objectlist2.size()) {
        auto pos = Objectlist2[i]->getPos();
        if (pos.x > 2 && pos.x < 17 && pos.z > 2 && pos.z < 17) {
            delete Objectlist2[i];
            Objectlist2.erase(Objectlist2.begin() + i);
        }
        else {
            i++;
        }
    }*/
    Models.push_back(new Object(glm::vec3(0, 0, 1), shadermodel_list[1]));
    Models.push_back(new Object(glm::vec3(0, -1, -4), shadermodel_list[2]));
    //Models.push_back(new Object(glm::vec3(22, 1,0 ), shadermodel_list[3]));
    Models[1]->scalemat = glm::scale(Models[1]->scalemat, glm::vec3(0.15f));
    //Models[2]->scalemat = glm::scale(Models[2]->scalemat, glm::vec3(0.05f));
    
    for(int i=0;i<5;i++){
        Models.push_back(new Object(glm::vec3(i-5,0,0),shadermodel_list[0]));
    }


    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  -0.9f,  2.0f),
        glm::vec3( 2.3f, -0.9f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -6.0f),
        glm::vec3( 0.0f,  -0.9f, -3.0f)
    };
    for(int i=0;i<4;i++){
        point_lights.push_back(new Pointlight(pointLightPositions[i]));
    }
    //std::cout<<shadermodel_list[0]->objlist.size()<<std::endl;
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    camera.set_speed(6.0f);
    int fpscounter = 0;
    float fpsct = 0;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fpsct += deltaTime;
        fpscounter++;
        if (fpsct >= 1) {
            fpsct = 0;
            /*system("cls");
            std::cout << "fps: " << fpscounter << '\n';*/
            fpscounter = 0;
        }
        //方块位置更新
        // static float counter = 0;
        // counter += deltaTime;
        // if(counter >= 0.2){
        //     counter = 0;

        // }
        /*for (int j = 0; j < Objectlist.size(); j++) {
            auto pos = Objectlist[j]->getPos();
            if (pos.x > 10) Objectlist[j]->setPos(-10, pos.y, pos.z);
            else Objectlist[j]->shift(2 * deltaTime, 0, 0);
        }*/

        processInput(window);
        rend();//渲染

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //释放内存
    /*for (auto ele : Objectlist2) {
        ele->Mod = nullptr;
        delete ele;
    }*/
    for (auto ele : Models) {
        ele->Mod = nullptr;
        delete ele;
    }
    for(auto ele : shadermodel_list) delete ele;
    for (auto ele : point_lights) {
        ele->Mod = nullptr;
        delete ele;
    }
    delete groundobj;
    delete lightshadermdl;
    delete groundshadermdl;
    delete axismodel;
    delete rainmodel;
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    static float C_last_time=0,UP_last_time=0,DOWN_last_time=0,picking_last_time=0, O_last_time=0, X_last_time=0, Q_last_time = 0, TAB_last_time;
    static int P_pressed = 0,LALT_pressed=0;
    static float last_time_0 = 0, last_time_1 = 0, last_time_2 = 0, last_time_3 = 0, last_time_4 = 0;
    
    float lastPressTime = 0.0;
    const float delayTime = 0.5;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (glfwGetTime() - C_last_time > 0.25) {
            torch_open = torch_open > 0.5 ? 0 : 1;
            C_last_time = glfwGetTime();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!P_pressed) {
            camera_mode = 1 - camera_mode;
            P_pressed = 1;
            if (camera_mode == 1) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
    else {
        P_pressed = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        if (LALT_pressed == 0) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            LALT_pressed = 1;
        }  
    }else {
        if (LALT_pressed == 1) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            LALT_pressed = 0;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS) {
        if (glfwGetTime() - UP_last_time > 0.04) {
            if (camera.MovementSpeed < 59.9f) camera.MovementSpeed += 1;
            UP_last_time = glfwGetTime();
        }  
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS) {
        if (glfwGetTime() - DOWN_last_time > 0.04) {
            if (camera.MovementSpeed > 1.01f) camera.MovementSpeed -= 1;
            DOWN_last_time = glfwGetTime();
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS &&
            glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        if (glfwGetTime() - picking_last_time > 0.25) {
            Object* temp_obj = nullptr;
            temp_obj = get_Target_object((int)lastX, (int)lastY);
            if (temp_obj == target_obj) {
                target_obj = nullptr;
                isModelSelected = false;
            }
            else
                target_obj = temp_obj;
            glm::vec3 pos = get_Target_world((int)lastX, (int)lastY);
            std::printf("last_ptr: %u, target_ptr: %u; point world position: %f,%f,%f\n", (GLuint)last_obj, (GLuint)target_obj, pos.x, pos.y, pos.z); 
            picking_last_time = glfwGetTime();
            
            if ((GLuint)target_obj != 0) { // 如果选中了某个模型,如果选中的模型不是地面，进入编辑模式
                if ((GLuint)target_obj != (GLuint)groundobj)
                    isModelSelected = true;
                if ((GLuint)target_obj == (GLuint)groundobj && (GLuint)last_obj != (GLuint)groundobj && isModelSelected==true) { // 如果现在选择的是地面，而上一次选择的模型不是地面，则移动模型位置
                    if(last_obj->Mod->category == 3 || last_obj->Mod->category == 4)
                        last_obj->setPos(pos.x, pos.y + 0.5*ModelscaleFactor , pos.z);
                    else if(last_obj->Mod->category == 1)
                        last_obj->setPos(pos.x, pos.y, pos.z);
                    
                    isModelSelected = false;
                }
            }
            else {
                isModelSelected = false;
            }

            last_obj = target_obj;
            cout << "next mode: " << isModelSelected << endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
        if (glfwGetTime() - picking_last_time > 0.25) {
            // 在这里执行删除模型的操作
            if (isModelSelected == true) {
                cout << Models.size() << endl;
                for (int i = 0; i < Models.size(); i++) {
                    if (Models[i] == last_obj) {
                        for (int j = i; j + 1 < Models.size(); j++) {
                            Models[j] = Models[j + 1];
                        }
                        Models.pop_back();
                        break;
                    }
                }
                last_obj->delshadermodel();
                isModelSelected = false;
                last_obj = nullptr;
                target_obj = nullptr;
                cout << Models.size() << endl;
            }
        }
    }
    
    // 左Alt 和 X 被按下生成
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (glfwGetTime() - X_last_time > 0.25 && isModelSelected == false) {
            // 0 被按下生成0
            if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
                float currentTime = glfwGetTime();
                if (currentTime - last_time_0 > 2) {
                    if (isModelSelected == false) {
                        glm::vec3 pos = get_Target_world((int)lastX, (int)lastY);
                        pos += glm::vec3(0, 0.5, 0);
                        Models.push_back(new Object(pos, shadermodel_list[0]));
                        cout << "num: " << Models.size() << endl;
                    }
                    last_time_0 = currentTime;
                }
            }
            // 1 被按下
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                float currentTime = glfwGetTime();
                if (currentTime - last_time_1 > 2) {
                    if (isModelSelected == false) {
                        glm::vec3 pos = get_Target_world((int)lastX, (int)lastY);
                        pos += glm::vec3(0, 0.5, 0);
                        Models.push_back(new Object(pos, shadermodel_list[1]));
                    }
                    last_time_1 = currentTime;
                }
            }
            // 2 被按下
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                float currentTime = glfwGetTime();
                if (currentTime - last_time_2 > 2) {
                    if (isModelSelected == false) {
                        glm::vec3 pos = get_Target_world((int)lastX, (int)lastY);
                        pos += glm::vec3(0, 0, 0);
                        Models.push_back(new Object(pos, shadermodel_list[2]));
                    }
                    last_time_2 = currentTime;
                }
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS){
        float scaleFactor = 1.0f; // 缩放因子

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            // 按下方向上键时，增大模型
            scaleFactor = 1.15f; // 或者你可以根据需要调整缩放因子
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            // 按下方向下键时，缩小模型
            scaleFactor = 0.85f; // 或者你可以根据需要调整缩放因子
        }

        
        // 在这里执行缩放模型的操作
        if (isModelSelected == true) {
            //cout << "scale: " << scaleFactor << endl;
            target_obj->scalemat = glm::scale(target_obj->scalemat, glm::vec3(scaleFactor));
            ModelscaleFactor = target_obj->scalemat[0][0];
        }
        
        UP_last_time = glfwGetTime();
        
    }
    //网格模式打开/关闭
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (glfwGetTime() - Q_last_time > 0.5) {
            ground_grid = 1 - ground_grid;
            Q_last_time = glfwGetTime();
        }
    }
    // 调整渲染风格，按tab键切换
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (glfwGetTime() - TAB_last_time > 0.2) {
            render_style = (render_style + 1) % render_style_number;
        }
        TAB_last_time = glfwGetTime();
    }
}

void RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) return;
    else if (camera_mode == 1) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
            camera.ProcessMouseMovement(-xoffset, -yoffset);
    }

}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

//获取指定位置物体
Object* get_Target_object(int winX,int winY) {
    static GLuint obj_ptr[4];
    Object* temp_obj = nullptr;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
    glReadPixels(winX,(int)SCR_HEIGHT-winY, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, obj_ptr);
    glReadBuffer(GL_NONE);
    temp_obj = (Object*)(((ull)obj_ptr[0] << 32) + (ull)obj_ptr[1]);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return temp_obj;
}

//读指定位置发射的射线，与世界内物体相交的位置的世界坐标
glm::vec3 get_Target_world(int winX, int winY) {
    glm::vec3 world_;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(winX, (int)SCR_HEIGHT - winY, 1, 1, GL_RGB, GL_FLOAT, &world_);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return world_;
}