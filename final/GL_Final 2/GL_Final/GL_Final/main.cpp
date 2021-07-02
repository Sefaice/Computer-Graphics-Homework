// glew
#include <GL/glew.h>
// glfw3
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "resourceManager/resource_manager.h"
#include "camera.h"
#include "models/terrain.h"
#include "models/skyBox.h"
#include "models/Snow.h"
#include "resourceManager/model.h"

#include <iostream>
#include <vector>

// shader_Path
//char* vs_path="../../final/src/vertexShader.vs.glsl";
//char* fs_path="../../final/src/fragmentShader.fs.glsl";

// 屏幕尺寸变量
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawCube();
void drawSphere();

std::vector<glm::vec3> positions;
unsigned int oceanEdge = 100;
const float L = 0.2f;
const float W = 2.0f / L;
const float A = 0.3f;
const float S = 0.2f;
const float P = S * W;
const glm::vec3 D = glm::vec3(1.0, 0.0, 1.0);

const float L1 = 0.2f;
const float W1 = 2.0f / L1;
const float A1 = 0.3f;
const float S1 = 0.2f;
const float P1 = S1 * W1;
const glm::vec3 D1 = glm::vec3(1.0, 0.0, 0.0);
Shader initWaveShader(const char* vertexPath, const char* fragmentPath,const char* name);
void initOcean();
void drawOcean();

void initModels();
void drawModels();
void initSnow(Snow &snow);
void drawSnow(Snow &snow,float deltaTime);


Camera camera(glm::vec3(0.0, 100.0, 10.0), glm::vec3(0.0, 1.0, 0.0));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;


std::map<std::string, Model> Models;

int main()
{
    // 初始化配置glfw
    glfwInit();
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
    //glfwWindowHint(GLFW_SAMPLES, 4);
    
    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "final", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 绑定窗口大小改变的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // config mouse display
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // 用glad加载函数指针
    glewInit();
    
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // 调用Shader类加载着色器代码
    Shader wave_shader=initWaveShader("shaders/vertexShader.vs.glsl", "shaders/fragmentShader.fs.glsl","wave_shader");
    // 是否采用线框模式
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    initOcean();
    
    Terrain m_terrain(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f));
    Shader terrain_shader=m_terrain.initTerrainShader("terrain");
    m_terrain.vao_vbo_init();
    
    Snow snow;
    initSnow(snow);
    
    
    initModels();
    
    std::vector<char*> images;
    images.push_back("./assets/images/SkyBox/riverside_west.BMP");//左
    images.push_back("./assets/images/SkyBox/riverside_east.BMP");//右
    images.push_back("./assets/images/SkyBox/riverside_up.BMP");//上
    images.push_back("./assets/images/SkyBox/riverside_down.BMP");//下
    images.push_back("./assets/images/SkyBox/riverside_south.BMP");//后
    images.push_back("./assets/images/SkyBox/riverside_north.BMP");//前
    
    SkyBox skybox;
    skybox.initSkyBox();
    skybox.loadCubeMap(images);
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // 计算时间间隔
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // 用户输入
        processInput(window);
        
        // 清空颜色缓冲
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        skybox.drawSkyBox(camera.getViewMatrix());
        glEnable(GL_DEPTH_TEST);
        // draw
        terrain_shader.Use();
        terrain_shader.SetMatrix4("view", camera.getViewMatrix());
        m_terrain.Draw();
        drawModels();
        
        wave_shader.Use();
        wave_shader.SetVector3f("viewPos", camera.position);
        wave_shader.SetMatrix4("view", camera.getViewMatrix());
        //drawSphere();
        //drawCube();
        //drawOcean();
        
        glDisable(GL_DEPTH_TEST);
        drawSnow(snow,deltaTime);
        glEnable(GL_DEPTH_TEST);
        // 检查调用事件，交换缓冲
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveBack(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveLeft(deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    camera.rotate(yoffset, xoffset);
}

unsigned int cubeVAO = 0;
void drawCube()
{
    if (cubeVAO == 0)
    {
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };
        
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &cubeVAO);
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int indexCount;
unsigned int sphereVAO = 0;
void drawSphere()
{
    if (sphereVAO == 0)
    {
        // sphere data
        unsigned int sphereSteps = 64;
        float pi = 3.1415926;
        // count positions and normals
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        //std::vector<glm::vec2> texcoords;
        for (int i = 0; i <= sphereSteps; i++) {
            for (int j = 0; j <= sphereSteps; j++) {
                float xstep = (float)i / (float)sphereSteps;
                float ystep = (float)j / (float)sphereSteps;
                float x = sin(ystep * pi) * cos(xstep * 2 * pi);
                float y = cos(ystep * pi);
                float z = sin(ystep * pi) * sin(xstep * 2 * pi);
                positions.push_back(glm::vec3(x, y, z));
                normals.push_back(glm::vec3(x, y, z));
                //texcoords.push_back(glm::vec2(xstep, ystep));
            }
        }
        // indices
        std::vector<unsigned int> indices;
        bool oddRow = false;
        for (int y = 0; y < sphereSteps; y++) {
            if (!oddRow) {
                for (int x = 0; x <= sphereSteps; x++) {
                    indices.push_back(y * (sphereSteps + 1) + x);
                    indices.push_back((y + 1) * (sphereSteps + 1) + x);
                }
            }
            else {
                for (int x = sphereSteps; x >= 0; x--) {
                    indices.push_back((y + 1) * (sphereSteps + 1) + x);
                    indices.push_back(y * (sphereSteps + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();
        // put into data
        std::vector<float> data;
        for (int i = 0; i < positions.size(); i++) {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
            
            /*data.push_back(texcoords[i].x);
             data.push_back(texcoords[i].y);*/
        }
        // VBO VAO EBO
        unsigned int sphereVBO, sphereEBO;
        // VBO
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        // VAO
        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);
        /*glEnableVertexAttribArray(0);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(1);
         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
         glEnableVertexAttribArray(2);
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));*/
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        // EBO
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


Shader initWaveShader(const char* vertexPath, const char* fragmentPath, const char* name){
    ResourceManager::LoadShader(vertexPath, fragmentPath, nullptr, name);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    //model = glm::rotate(model, (float)sin(glfwGetTime()), glm::vec3(1.0, 1.0, 1.0));
    model = glm::scale(model, glm::vec3(10.0, 1.0, 10.0));
    view = camera.getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::vec3 lightColor = glm::vec3(1.0);
    glm::vec3 lightPos = glm::vec3(0.0, 10.0, 0.0);
    glm::vec3 objectColor = glm::vec3(0.33, 0.55, 0.69);
    ResourceManager::GetShader(name).SetMatrix4("model", model,true);
    ResourceManager::GetShader(name).SetMatrix4("view", view,true);
    ResourceManager::GetShader(name).SetMatrix4("projection", projection,true);
    ResourceManager::GetShader(name).SetVector3f("lightColor", lightColor,true);
    ResourceManager::GetShader(name).SetVector3f("lightPos", lightPos,true);
    ResourceManager::GetShader(name).SetVector3f("objectColor", objectColor,true);
    ResourceManager::GetShader(name).SetVector3f("viewPos", camera.position,true);
    return ResourceManager::GetShader("wave_shader");
}

void initOcean()
{
    positions.clear();
    for (int i = 0; i < oceanEdge; i++)
    {
        for (int j = 0; j < oceanEdge; j++)
        {
            positions.push_back(glm::vec3(-0.5 + i * 1.0 / (float)oceanEdge, 0, -0.5 + j * 1.0 / (float)oceanEdge));
        }
    }
}

void drawOcean()
{
    std::vector<glm::vec3> data;
    float t = glfwGetTime();
    for (int i = 0; i < positions.size(); i++)
    {
        float x = positions[i].x;
        float z = positions[i].z;
        float y = A * sin(glm::dot(D, glm::vec3(x, 0.0, z) * W + t * P)) + A1 * sin(glm::dot(D1, glm::vec3(x, 0.0, z) * W1 + t * P1));
        data.push_back(glm::vec3(x, y, z));
        
        float nx = -W * D.x * A * cos(glm::dot(D, glm::vec3(x, 0.0, z) * W + t * P)) +
        (-W1 * D1.x * A1 * cos(glm::dot(D1, glm::vec3(x, 0.0, z) * W1 + t * P1)));
        float ny = 1.0;
        float nz = -W * D.z * A * cos(glm::dot(D, glm::vec3(x, 0.0, z) * W + t * P)) +
        (-W1 * D1.z * A1 * cos(glm::dot(D1, glm::vec3(x, 0.0, z) * W1 + t * P1)));
        glm::vec3 normal = glm::normalize(glm::vec3(nx, ny, nz));
        data.push_back(normal);
    }
    
    std::vector<unsigned int> indices;
    /*for (int j = 0; j < oceanEdge - 1; j++) {
     for (int i = 0; i < oceanEdge - 1; i++) {
     indices.push_back(j * oceanEdge + i);
     indices.push_back((j + 1) * oceanEdge + i);
     indices.push_back(j * oceanEdge + i + 1);
     
     indices.push_back((j + 1) * oceanEdge + i);
     indices.push_back((j + 1) * oceanEdge + i + 1);
     indices.push_back(j * oceanEdge + i + 1);
     }
     }*/
    bool oddRow = false;
    for (int y = 0; y < oceanEdge - 1; y++) {
        if (!oddRow) {
            for (int x = 0; x < oceanEdge; x++) {
                indices.push_back(y * (oceanEdge)+x);
                indices.push_back((y + 1) * oceanEdge + x);
            }
        }
        else {
            for (int x = oceanEdge - 1; x >= 0; x--) {
                indices.push_back((y + 1) * oceanEdge + x);
                indices.push_back(y * oceanEdge + x);
            }
        }
        oddRow = !oddRow;
    }
    unsigned int oceanIndexCount = indices.size();
    
    // VBO
    unsigned int oceanVBO;
    glGenBuffers(1, &oceanVBO);
    glBindBuffer(GL_ARRAY_BUFFER, oceanVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
    // VAO
    unsigned int oceanVAO;
    glGenVertexArrays(1, &oceanVAO);
    glBindVertexArray(oceanVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(data[0]), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(data[0]), (void*)(sizeof(data[0])));
    // EBO
    unsigned int oceanEBO;
    glGenBuffers(1, &oceanEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oceanEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(oceanVAO);
    glDrawElements(GL_TRIANGLE_STRIP, oceanIndexCount, GL_UNSIGNED_INT, 0);
    
    glDeleteVertexArrays(1, &oceanVAO);
    glDeleteBuffers(1, &oceanVBO);
    glDeleteBuffers(1, &oceanEBO);
}

void initModels(){
    
    ResourceManager::LoadShader("shaders/1.model_loading.vs", "shaders/1.model_loading.fs", nullptr, "model_shader");
    Shader model_shader=ResourceManager::GetShader("model_shader");
    model_shader.Use();
    
    Model bamboo("./assets/models/bamboo/bamboo.obj");
    Models["bamboo"]=bamboo;

    Model bamboo2("./assets/models/bamboo2/bamboo2.obj");
    Models["bamboo2"]=bamboo2;

    Model house("./assets/models/house/house.obj");
    Models["house"]=house;
}
void drawModels(){
    glEnable(GL_ALPHA_TEST);  // 启用Alpha测试
    glAlphaFunc(GL_GREATER, 0.5f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Shader model_shader=ResourceManager::GetShader("model_shader");
    model_shader.Use();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
    glm::mat4 view = camera.getViewMatrix();
    model_shader.SetMatrix4("projection", projection);
    model_shader.SetMatrix4("view", view);
    
    //bamboo
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(30.0f, 0.0f, 200.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));    // it's a bit too big for our scene, so scale it down
    model_shader.SetMatrix4("model", model);
    Models["bamboo"].Draw(model_shader);

    //bamboo2
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(60.0f, 0.0f, 220.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));    // it's a bit too big for our scene, so scale it down
    model_shader.SetMatrix4("model", model);
    Models["bamboo2"].Draw(model_shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(90.0f, 0.0f, 220.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
    model_shader.SetMatrix4("model", model);
    Models["bamboo2"].Draw(model_shader);

    //house
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(100.0f, 0.0f, 180.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));    // it's a bit too big for our scene, so scale it down
    model_shader.SetMatrix4("model", model);
    Models["house"].Draw(model_shader);
}

void initSnow(Snow &snow){
    GLfloat near_plane = 1.0f, far_plane = 1000.0f;
    GLfloat ws = 1000.0f;
    glm::mat4 lightProjection = glm::ortho(-ws, ws, -ws, ws, near_plane, far_plane);
    glm::mat4 lightView = camera.getViewMatrix();
    glm::mat4 lightSpaceMatrix1 = lightProjection * lightView;
    snow.setLightSpace(lightSpaceMatrix1);
}
void drawSnow(Snow &snow,float deltaTime){
    glm::mat4 projection(1.0f);
    glm::mat4 model(1.0f);
    //model=glm::translate(model, glm::vec3());
    glm::mat4 view = camera.getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.001f, 2000.0f);
    snow.Render(deltaTime, model, view, projection);
    snow.RenderShadow(deltaTime);
}
