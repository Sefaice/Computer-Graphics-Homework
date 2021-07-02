//
//  terrain.h
//  GL_Final
//
//  Created by 朱俊凯 on 2019/6/11.
//  Copyright © 2019年 朱俊凯. All rights reserved.
//

#ifndef terrain_h
#define terrain_h
//#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

//#include "../resourceManager/shader.h"
//#include "../resourceManager/texture.h"
#include "../resourceManager/resource_manager.h"

using namespace std;

class Terrain{
public:
    glm::vec3  Position, Size;
    unsigned int VAO, VBO;
    int width, height;
    Shader shader;
    vector<glm::vec3> m_vertexs;
    vector<glm::vec3> m_normals;
	vector<glm::vec2> m_textures;
	vector<float> m_total;
	unsigned int terrainIndexCount;
    Terrain();
    Terrain(glm::vec3 pos, glm::vec3 size);
    Shader& initTerrainShader(const char* name);
    void vao_vbo_init();
    void DrawRefl(glm::mat4 view, glm::mat4 projection, glm::vec4 oceanPlane);
	void DrawDepthMap(Shader& depthShader);
	void Draw(glm::mat4 view, glm::mat4 projection, glm::mat4 lightSpaceMatrix, unsigned int shadowMap, 
		glm::vec3 lightDir, glm::vec3 lightColor, glm::vec3 viewPos);
	void DrawGbuffer(glm::mat4 view, glm::mat4 projection);
};

#endif /* terrain_h */
