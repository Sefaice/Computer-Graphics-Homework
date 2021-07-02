//
//  skyBox.cpp
//  GL_Final
//
//  Created by 朱俊凯 on 2019/6/12.
//  Copyright © 2019年 朱俊凯. All rights reserved.
//

#include "skyBox.h"
#include <SOIL.h>

SkyBox::SkyBox():
m_cubeTextureId(0),skyboxVAO(0){
    ResourceManager::LoadShader("./shaders/skybox.vs", "./shaders/skybox.fs", nullptr, "skybox");
    m_Shader = ResourceManager::GetShader("skybox");
}
SkyBox::~SkyBox(){
}
void SkyBox::initSkyBox(){
    m_Shader.Use();
    GLfloat cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // Setup skybox VAO
    GLuint cubeVBO;
    glGenVertexArrays(1, &skyboxVAO);
    {
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);
}
bool SkyBox::loadCubeMap(const std::vector<char*>& images){
    m_Shader.Use();
    glGenTextures(1, &m_cubeTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTextureId);
    {
        int nW = 0, nH = 0;
        unsigned char* pChImg = nullptr;
        
        for (int i = 0; i < images.size(); i++)
        {
            pChImg = SOIL_load_image(images[i], &nW, &nH, 0, SOIL_LOAD_RGB);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                         nW, nH, 0, GL_RGB, GL_UNSIGNED_BYTE, pChImg);
            SOIL_free_image_data(pChImg);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}
void SkyBox::drawSkyBox(glm::mat4 camera_view){
    m_Shader.Use();
    glm::mat4 model=glm::mat4(1.0f);
    model=glm::scale(model, glm::vec3(1000.0f,1000.0f,1000.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1280 / (float)720, 0.0f, 2000.0f);
    m_Shader.SetMatrix4("model", model,true);
    m_Shader.SetMatrix4("view", camera_view,true);
    m_Shader.SetMatrix4("projection", projection,true);
    glBindVertexArray(skyboxVAO);
    {
        glActiveTexture(GL_TEXTURE0);
        m_Shader.SetInteger("skybox", 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTextureId);
        {
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    glBindVertexArray(0);
}
