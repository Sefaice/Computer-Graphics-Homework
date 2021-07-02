//
//  terrain.h
//  GL_Final
//
//  Created by 朱俊凯 on 2019/6/11.
//  Copyright © 2019年 朱俊凯. All rights reserved.
//

#ifndef terrain_h
#define terrain_h
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "texture.h"
#include "../resourceManager/resource_manager.h"

using namespace std;

class Terrain{
public:
    glm::vec3  Position, Size;
    unsigned int VAO,VBO;
    int width, height;
    Shader shader;
    vector<glm::vec3> m_vertexs;
    vector<glm::vec3> m_textures;
    vector<glm::vec3> m_total;
    Terrain();
    Terrain(glm::vec3 pos, glm::vec3 size);
    Shader initTerrainShader(const char* name);
    void vao_vbo_init();
    void Draw();
};


#endif /* terrain_h */
