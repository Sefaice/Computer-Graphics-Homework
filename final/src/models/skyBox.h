//
//  skyBox.h
//  GL_Final
//
//  Created by 朱俊凯 on 2019/6/12.
//  Copyright © 2019年 朱俊凯. All rights reserved.
//

#ifndef skyBox_h
#define skyBox_h

#include "../resourceManager/resource_manager.h"
#include "../camera.h"
#include <vector>
//#include <GL/glew.h>
#include <glm/glm.hpp>

class SkyBox{
public:
    SkyBox();
    ~SkyBox();
    void initSkyBox();
    bool loadCubeMap();
    void drawSkyBox(glm::mat4 camera_view);
    
    GLuint  m_cubeTextureId;//CubeMap textureid
    Shader  m_Shader;
    GLuint  skyboxVAO;
};

#endif /* skyBox_h */
