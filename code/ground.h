#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include<opencv2/opencv.hpp>
#include<vector>
#include<cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"common.h"
#include"obj.h"
#include"vertex.h"
typedef unsigned char uchar;
BaseModelObj* groundshadermdl=nullptr;
std::vector<std::vector<uchar> > decode(const std::string path)   //pathΪͼƬ·��
{
    cv::Mat img = cv::imread(path.c_str());                // ��ͼƬ����Mat������
    //       ��ʾԭͼƬ
    //       namedWindow("old", WINDOW_NORMAL);
    //       imshow("old", img);
    //      waitKey(0);
    int w = img.cols * img.channels();     //����Ϊ3ͨ�������Ҫ��ͼƬ��ͨ����
    int h = img.rows;

    std::vector<std::vector<uchar> > array(h, std::vector<uchar>(w));      //��ʼ����άvector
    for (int i = 0; i < h; i++)
    {
        uchar* inData = img.ptr<uchar>(i);            //ptrΪָ��ͼƬ����ָ�룬����iΪ����
        for (int j = 0; j < w; j++)
        {
            array[i][j] = inData[j];
        }
    }
    return array;
}

std::vector<std::vector<glm::vec3> > decode2(const std::string path,float scale = 1.0f)   //pathΪͼƬ·��
{
    cv::Mat nimg = cv::imread(path.c_str());                // 将图片传入Mat容器中
    //       显示原图片
    //std::cout << nimg.channels()<<" "<< nimg.cols<<" "<< nimg.rows << '\n';
    cv::Mat img;
    cv::cvtColor(nimg,img, cv::COLOR_RGB2GRAY);
    /*cv::namedWindow("old", cv::WINDOW_NORMAL);
    cv::imshow("old", img);
    cv::waitKey(0);*/
    
    int w = img.cols;     //����Ϊ3ͨ�������Ҫ��ͼƬ��ͨ����
    int h = img.rows;
    int cen_w = w / 2,cen_h = h/2;

    std::vector<std::vector<glm::vec3> > array(h, std::vector<glm::vec3>(w));      //��ʼ����άvector
    for (int i = 0; i < h; i++)
    {
        uchar* inData = img.ptr<uchar>(i);            //ptrΪָ��ͼƬ����ָ�룬����iΪ����
        for (int j = 0; j < w; j++)
        {
            array[i][j] = glm::vec3((i - cen_h)*scale, (double)inData[j]*scale * 60 / 255, (j - cen_w)*scale);
        }
    }
    return array;
}

void code(std::vector<std::vector<uchar> > array)
{
    size_t h = array.size();
    size_t w = array[0].size();
    //��ʼ��ͼƬ�����س���
    cv::Mat img(h, (size_t)w, CV_8U, cv::Scalar(0));
    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            img.at<uchar>(i, j) = array[i][j];
        }
    }
    
    cv::imwrite("./ground/save.jpg", img);
}

void Create_gray() {
    int _size = 200;
    std::vector<std::vector<uchar>> arr(_size, std::vector<uchar>(_size, 0));
    for (int i = 0; i < arr.size(); i++) {
        for (int j = 0; j < arr[0].size(); j++) {
            arr[i][j] = glm::sin((double)(i+j)/100)*255;
        }
    }
    code(arr);
}

void f(float* p, glm::vec3 pos,glm::vec3 nor,float tx,float ty) {
    p[0] = pos.x; p[1] = pos.y; p[2] = pos.z;
    p[3] = nor.x; p[4] = nor.y; p[5] = nor.z;
    p[6] = tx; p[7] = ty;
}

void Create_ground_model(const std::string path,float scale=1.0f) {
    std::vector<std::vector<glm::vec3> > arr = decode2(path,scale);
    int step = 8;
    ground_vertex_len = (arr.size() - 1) * (arr[0].size() - 1) * 6;
    float* res = new float[ground_vertex_len * step];
    float* ptr = res;
    for (int i = 0; i < arr.size()-1; i++) {
        for (int j = 0; j < arr[0].size()-1; j++) {
            // i,j;i+1,j;i,j+1;i+1,j+1;
            glm::vec3 v1 = arr[i + 1][j + 1] - arr[i][j];
            glm::vec3 nor1 = glm::normalize(glm::cross(v1, arr[i + 1][j] - arr[i][j]));
            glm::vec3 nor2 = glm::normalize(glm::cross(arr[i][j+1] - arr[i][j],v1));
            f(ptr, arr[i][j], nor1, arr[i][j].x, arr[i][j].z);
            f(ptr + step, arr[i+1][j], nor1, arr[i+1][j].x, arr[i+1][j].z);
            f(ptr + 2*step, arr[i + 1][j+1], nor1, arr[i + 1][j+1].x, arr[i+ 1][j+1].z);
            f(ptr + 3*step, arr[i][j], nor2, arr[i][j].x, arr[i][j].z);
            f(ptr + 4*step, arr[i][j+1], nor2, arr[i][j + 1].x, arr[i][j + 1].z);
            f(ptr + 5*step, arr[i + 1][j + 1], nor2, arr[i + 1][j + 1].x, arr[i + 1][j + 1].z);
            ptr += 6 * step;
        }
    }
    ground = res;
}

void Create_ground_model_2(const std::string path) {
    std::vector<std::vector<glm::vec3> > arr = decode2(path);
    int step = 8;
    ground_vertex_len = (arr.size() - 1) * (arr[0].size() - 1) * 6;
    float* res = new float[ground_vertex_len * step];
    float* ptr = res;
    float tex_scale = 0.2;
    for (int i = 0; i < arr.size() - 1; i++) {
        for (int j = 0; j < arr[0].size() - 1; j++) {
            // i,j;i+1,j;i,j+1;i+1,j+1;
            glm::vec3 v1 = arr[i + 1][j + 1] - arr[i][j];
            glm::vec3 nor1 = glm::normalize(glm::cross(v1, arr[i + 1][j] - arr[i][j]));
            glm::vec3 nor2 = glm::normalize(glm::cross(arr[i][j + 1] - arr[i][j], v1));
            f(ptr, arr[i][j], nor1, 0, 0);
            f(ptr + step, arr[i + 1][j], nor1, 1 * tex_scale, 0);
            f(ptr + 2 * step, arr[i + 1][j + 1], nor1, 1 * tex_scale, 1 * tex_scale);
            f(ptr + 3 * step, arr[i][j], nor2, 0, 0);
            f(ptr + 4 * step, arr[i][j + 1], nor2, 0, 1 * tex_scale);
            f(ptr + 5 * step, arr[i + 1][j + 1], nor2, 1 * tex_scale, 1 * tex_scale);
            ptr += 6 * step;
        }
    }
    ground = res;
}


struct Ground_Model :public BaseModelObj {
    unsigned int texMap;
    long groundlen;
    Ground_Model(const std::string texpath) :BaseModelObj(s_path+"ground.vs", s_path+"ground.fs") {
        groundlen = ground_vertex_len * 8;
        VAO = create_texmodel(ground, groundlen * sizeof(float));
        texMap = gentexture(texpath);
    }
    void render(glm::vec3& lightPos, glm::mat4& projection, glm::mat4& view, glm::vec3& viewPos) {
        shader.use();
        shader.setInt("material.diffuse", 0);
        //shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", 8.0f);
        // directional light
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texMap);
        // camera
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // world transformation
        shader.setVec3("viewPos", viewPos);
        shader.setInt("ground_grid", ground_grid);
        // std::printf("%f,%f,%f\n",viewPos.x,viewPos.y,viewPos.z);
        //render all object
        glBindVertexArray(VAO);
        for (auto ele : objlist) {
            shader.setUint("object_ptr_l", (GLuint)ele);
            shader.setUint("object_ptr_h", (GLuint)((ull)ele >> 32));
            shader.setMat4("model", ele->getmodel());
            glDrawArrays(GL_TRIANGLES, 0, groundlen); //
        }
    }
};