#include <stdio.h>
#include "terrain.h"
#include <SOIL.h>
#include <cstring>
Terrain::Terrain(): Position(0, 0, 0), Size(1, 1, 1) { }


Terrain::Terrain(glm::vec3 pos, glm::vec3 size)
: Position(pos), Size(size){
    unsigned char* image = SOIL_load_image("./assets/images/plane.jpg", &width, &height, 0, SOIL_LOAD_AUTO);
    printf("%d ,%d ,%d  :%d  ,%d ,%d \n",width,height,image[99],image[100],image[240500],image[240600]);
    m_vertexs.clear();
    m_textures.clear();
    for(int i=0; i<height-1; i++)   // 0 1 2 |0| 3 4 5 |1|
    {
        for(int k=0; k<width-1; k++)
        {
            // 生成顶点数组, 坐标按照三角网格处理 GL_TRIGANLES
            glm::vec3 v1(k+0,image[3*((i+0)*width+(k+0))],i+0);
            glm::vec3 v2(k+0,image[3*((i+1)*width+(k+0))],i+1);
            glm::vec3 v3(k+1,image[3*((i+1)*width+(k+1))],i+1);
            glm::vec3 v4(k+1,image[3*((i+0)*width+(k+1))],i+0);
            m_vertexs.push_back(v1);
            m_vertexs.push_back(v2);
            m_vertexs.push_back(v3);
            m_vertexs.push_back(v1);
            m_vertexs.push_back(v3);
            m_vertexs.push_back(v4);
        }
    }
    // 生成纹理坐标
    for(int i=0; i<m_vertexs.size(); i++)
    {
        glm::vec3 tmp(m_vertexs[i].x/width,m_vertexs[i].z/height,0);
        m_textures.push_back(tmp);
    }
    
}

Shader Terrain::initTerrainShader(const char* name){
    ResourceManager::LoadShader("shaders/terrain.vs.glsl", "shaders/terrain.fs.glsl", nullptr, name);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(-width, -100.0f, -height*0.5f) );
    model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
    
    projection = glm::perspective(glm::radians(45.0f), (float)1280 / (float)720, 0.1f, 1000.0f);
    ResourceManager::GetShader(name).SetMatrix4("model", model,true);
    ResourceManager::GetShader(name).SetMatrix4("view", view,true);
    ResourceManager::GetShader(name).SetMatrix4("projection", projection,true);
    ResourceManager::LoadTexture("./assets/images/rock.jpeg", GL_TRUE, "snowfield");
    ResourceManager::LoadTexture("./assets/images/SkyBox/riverside_down.BMP", GL_TRUE, "grass");
    Texture2D snowfield=ResourceManager::GetTexture("snowfield");
    Texture2D grass=ResourceManager::GetTexture("grass");
    glActiveTexture(GL_TEXTURE7);
    snowfield.Bind();
    ResourceManager::GetShader(name).SetInteger("texture1", 7,true);
    glActiveTexture(GL_TEXTURE8);
    grass.Bind();
    ResourceManager::GetShader(name).SetInteger("texture2", 8,true);
    this->shader=ResourceManager::GetShader(name);
    return ResourceManager::GetShader(name);
}
void Terrain::vao_vbo_init(){
    for(int i=0;i<m_vertexs.size();i++){
        m_total.push_back(m_vertexs[i]);
        m_total.push_back(m_textures[i]);
    }
    shader.Use();
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1, &VBO);
    //绑定顶点数组对象
    glBindVertexArray(VAO);
    //把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(m_total[0]), (void*)0);
    glEnableVertexAttribArray(0);
    //设置顶点属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(m_total[0]), (void*)(sizeof(m_total[0])));
    glEnableVertexAttribArray(1);
    //清楚绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}
void Terrain::Draw()
{
    shader.Use();
    Texture2D snowfield=ResourceManager::GetTexture("snowfield");
    Texture2D grass=ResourceManager::GetTexture("grass");
    glActiveTexture(GL_TEXTURE7);
    snowfield.Bind();
    glActiveTexture(GL_TEXTURE8);
    grass.Bind();
    glBindVertexArray(VAO);
    
    //把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_total.size()*sizeof(m_total[0]), &m_total[0], GL_DYNAMIC_DRAW);
    //清楚绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0,(int)m_vertexs.size());
}
