#pragma once
#include<Shader/shader.h>
#include <camera/camera.h>
// settings
const unsigned int SCR_WIDTH = 1120;
const unsigned int SCR_HEIGHT = 630;
unsigned int VAO_cube = 0;
unsigned int VAO_sky = 0;
//光照贴图
unsigned int _textureBox = 0;
unsigned int _textureSky = 0;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int camera_mode = 1;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// lighting
glm::vec3 lightPos(0.0f, 2.0f, -1.0f);
Shader _shader_sky;
//ground
float* ground=nullptr;
long ground_vertex_len=0;

//running setting
float torch_open = 1;

// select model
bool isModelSelected = false;

// 模型列表
vector<Object*> Models;

// 地面模型
Object* groundobj;
//网格开关
int ground_grid = 1;

// 缩放因子
float ModelscaleFactor = 1.0f;