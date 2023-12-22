#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"object.h"
#include<IMG/ffimage.cpp>
#include <iostream>
#include<algorithm>
#include"common.h"
#include"pointlit.h"
#include"ground.h"

#include"light.h"
#include"Axis_generator.h"
#include<Windows.h>
#include<filesystem>

#include"Rain_generator.h"
//shader_model
std::vector<BaseModelObj*> shadermodel_list;

//deffered_rendering
unsigned int framebuffer;
unsigned int textureColorbuffer, posbuffer, normalbuffer,specolorbuffer,objidbuffer;
Shader deffered_shader;
//Axis
Axismodel* axismodel;
//Rain
RainModel* rainmodel;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

uint creatSkyBoxVAO();
uint createSkyBoxTex(std::string,std::string,std::string);

void rend(Light& main_light);
void RenderQuad();

void addlights(Light& light);

Object* get_Target_object(int winX, int winY);
glm::vec3 get_Target_world(int winX, int winY);

GLuint quadVAO = 0;
GLuint quadVBO;
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;

// 全局变量，渲染风格标记和总数
uint render_style = 0;
uint render_style_number = 5;
//point_light
std::vector<Pointlight*> point_lights;

void load_thread() {
    while (1) {
        string path;
        std::cout<<"ENTWR THE PATH OF THE MODEL, EXAMPLE: model/tree.fbx"<<endl;
        cin >> path;
       
        bool existed = 0;
        int cata;
        for (BaseModelObj*  ele : shadermodel_list) {    //遍历现有的渲染器
            if (ele->modelPath == path) {
                existed = 1;
                cata = ele->category;
            }
        }
        
        if (existed) {       //存在则加一个实体
            Models.push_back(new Object(glm::vec3(0, 0, 0), shadermodel_list[cata-1]));
            Models[Models.size() - 1]->scalemat = glm::scale(Models[Models.size() - 1]->scalemat, glm::vec3(0.05f));
        } 
        else {        //不存在则创建渲染器
            shadermodel_list.push_back(new ModelObj1(path));
            Models.push_back(new Object(glm::vec3(0, 0, 0), shadermodel_list[shadermodel_list.size()-1]));
            Models[Models.size()-1]->scalemat = glm::scale(Models[Models.size() - 1]->scalemat, glm::vec3(0.05f));

        }
    }
}
// 光照
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  -0.9f,  2.0f),
    glm::vec3(2.3f, -0.9f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -6.0f),
    glm::vec3(0.0f,  -0.9f, -3.0f)
};

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

void rend(Light& main_light){
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
    main_light.set_light(deffered_shader);
    RenderQuad();
    //渲染风格控制参数
    deffered_shader.setUint("render_style", (GLuint)render_style);
    //第三阶段渲染：不属于延迟渲染管线的对象
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲
    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    
    //点光源模型
    lightCube_model->render(lightPos, projection, view, camera.Position);
    
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

int main(){
    /*Create_gray();
    return 0;*/
    //初始化地面模型
    Create_ground_model(m_path + "ground/t1.png", 0.1f);
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
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Group_", NULL, NULL);
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
    Shader deffered_shader_(s_path + "sshader/s2.vs", s_path + "sshader/s2_phong.fs");
    deffered_shader = deffered_shader_;
    
    //生成天空盒(顶点对象，纹理)
    VAO_sky = creatSkyBoxVAO();
    _textureSky = createSkyBoxTex();
    _shader_sky.initialize(s_path + "sshader/skybox.vs", s_path + "sshader/skybox.fs");
    //初始化Axis
    axismodel = new Axismodel();
    //初始化RainModel
    rainmodel = new RainModel();
    //生成模型和对应着色器
    DirLight parallel{
    glm::vec3(0.3f, -0.7f, 1.0f),
    glm::vec3(0.3f, 0.3f, 0.3f),
    glm::vec3(0.3f, 0.3f, 0.3f)
    //glm::vec3(0.4f, 0.4f, 0.4f)
    };

    lightCube_model = new LightCube();
    Light main_light(parallel);
    addlights(main_light);


    for (int i = 0; i < 4; i++) {
        point_lights.push_back(new Pointlight(pointLightPositions[i]));
    }


    // 地面
    groundshadermdl = new Ground_Model(m_path + "texture/grass.jpg");
    shadermodel_list.push_back(new ModelObj3());
    shadermodel_list.push_back(new ModelObj4());
    shadermodel_list.push_back(new ModelObj1("model/nanosuit/nanosuit.obj"));
    shadermodel_list.push_back(new ModelObj1());
    //shadermodel_list.push_back(new ModelObj1(m_path + "model/tree.fbx"));

    groundobj = new Object(glm::vec3(27, -1, 25), groundshadermdl);//groundshadermdl
    
    Models.push_back(new Object(glm::vec3(0, 0, 1), shadermodel_list[1]));
    Models.push_back(new Object(glm::vec3(0, -1, -4), shadermodel_list[2]));
    //Models.push_back(new Object(glm::vec3(22, 1,0 ), shadermodel_list[3]));
    Models[1]->scalemat = glm::scale(Models[1]->scalemat, glm::vec3(0.15f));
    //Models[2]->scalemat = glm::scale(Models[2]->scalemat, glm::vec3(0.05f));
    
    for(int i=0;i<5;i++){
        Models.push_back(new Object(glm::vec3(i-5,0,0),shadermodel_list[0]));
    }


    //std::cout<<shadermodel_list[0]->objlist.size()<<std::endl;
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    camera.set_speed(6.0f);
    int fpscounter = 0;
    float fpsct = 0;

    HANDLE h; //线程句柄
    h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)load_thread, NULL, 1, 0); //创建子线程
    ResumeThread(h);

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
        processInput(window);
        rend(main_light);//渲染

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //释放内存
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
    delete lightCube_model;
    delete groundshadermdl;
    delete axismodel;
    delete rainmodel;
    CloseHandle(h);
    glfwTerminate();
    return 0;
}

void addlights(Light& light) {
    for (int i = 0; i < 4; i++) {
        _PointLight l{
            glm::vec3(pointLightPositions[i]),
            1.0f,
            0.09f,
            0.032f,
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3(1.0f, 1.0f, 1.0f),
        };
        light.AddPointLight(l);
    }
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
    
    //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS &&
    //    isModelSelected == true && glfwGetTime() - picking_last_time > 0.25) {
    //    isModelSelected = false;
    //    //target_obj->modelmat = origin;
    //}

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
                if ((GLuint)target_obj != (GLuint)groundobj) {
                    isModelSelected = true;
                } 
                else if ((GLuint)target_obj == (GLuint)groundobj && (GLuint)last_obj != (GLuint)groundobj && isModelSelected == true) { // 如果现在选择的是地面，而上一次选择的模型不是地面，则移动模型位置
                    if (last_obj->Mod->category == 3 || last_obj->Mod->category == 4)
                        last_obj->setPos(pos.x, pos.y + 0.5 * ModelscaleFactor, pos.z);
                    else if (last_obj->Mod->category == 1)
                        last_obj->setPos(pos.x, pos.y, pos.z);
                    target_obj = nullptr;
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
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS &&
        isRotate == true&& glfwGetTime() - picking_last_time > 0.25) {
        isRotate = false;
        //target_obj->modelmat = origin;
    }

    //发旋转信号
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        if (glfwGetTime() - picking_last_time > 0.25) {
            Object* temp_obj = nullptr;
            temp_obj = get_Target_object((int)lastX, (int)lastY);
            if (temp_obj != target_obj) {
                target_obj = temp_obj;
                isRotate = true;
            }
            glm::vec3 pos = get_Target_world((int)lastX, (int)lastY);
            //std::printf("last_ptr: %u, target_ptr: %u; point world position: %f,%f,%f\n", (GLuint)last_obj, (GLuint)target_obj, pos.x, pos.y, pos.z);
            picking_last_time = glfwGetTime();

            
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
        if (glfwGetTime() - picking_last_time > 0.25) {
            // 在这里执行删除模型的操作
            if (isModelSelected == true) {
                std::cout << Models.size() << endl;
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
                std::cout << Models.size() << endl;
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
                        std::cout << "num: " << Models.size() << endl;
                    }
                    last_time_0 = currentTime;
                }
            }
            // 1 被按下
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                float currentTime = glfwGetTime();
                if (currentTime - last_time_1 > 0.25) {
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
                if (currentTime - last_time_2 > 0.25) {
                    if (isModelSelected == false) {
                        glm::vec3 pos = get_Target_world((int)lastX, (int)lastY);
                        pos += glm::vec3(0, 0, 0);
                        Models.push_back(new Object(pos, shadermodel_list[2]));
                        Models.back()->scalemat = glm::scale(Models.back()->scalemat, glm::vec3(0.15f));
                    }
                    last_time_2 = currentTime;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                float currentTime = glfwGetTime();
                if (currentTime - last_time_3 > 0.25) {
                    if (isModelSelected == false) {
                        glm::vec3 pos = get_Target_world((int)lastX, (int)lastY);
                        pos += glm::vec3(0, 0, 0);
                        Models.push_back(new Object(pos, shadermodel_list[3]));
                        Models.back()->scalemat = glm::scale(Models.back()->scalemat, glm::vec3(0.02f));
                        Models.back()->scalemat = glm::rotate(Models.back()->scalemat,glm::radians(-90.0f), glm::vec3(1,0,0));
                    }
                    last_time_3 = currentTime;
                }
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS){
        float scaleFactor = 1.0f; // 缩放因子

       
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS == GLFW_PRESS) {
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

    float distance = glm::sqrt(xoffset * xoffset + yoffset * yoffset);

    if (target_obj == nullptr) {
        isRotate = FALSE;
    }

    if (isRotate&&(GLuint)target_obj!=(GLuint)groundobj) {
        float sensity = 0.005f;
        glm::mat3 inverse_ = glm::inverse(glm::mat3(target_obj->scalemat));
        glm::vec3 horizonal_axis =  glm::normalize(inverse_ * glm::cross(camera.Forward, camera.WorldUp));
        glm::vec3 vertical_axis = camera.WorldUp;
        target_obj->scalemat = glm::rotate(target_obj->scalemat, xoffset * sensity, vertical_axis);
        if (!(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS))
            target_obj->scalemat = glm::rotate(target_obj->scalemat, -yoffset * sensity, horizonal_axis);
        //glm::vec3 axis = glm::normalize(glm::vec3(yoffset, xoffset, 0.0f));
        //glm::quat rotationQuat = glm::angleAxis(angle, axis);
        //// 将四元数转为旋转矩阵
        //glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);
        ////origin = target_obj->modelmat;
        //target_obj->modelmat = target_obj->modelmat* rotationMatrix;

    }


    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS||isRotate) return;
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