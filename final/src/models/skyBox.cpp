//
//  skyBox.cpp
//  GL_Final
//
//  Created by 朱俊凯 on 2019/6/12.
//  Copyright © 2019年 朱俊凯. All rights reserved.
//

#include "skyBox.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include <iostream>

SkyBox::SkyBox():
m_cubeTextureId(0),skyboxVAO(0){
    ResourceManager::LoadShader("../../final/src/shaders/skyboxShader.vs.glsl", "../../final/src/shaders/skyboxShader.fs.glsl", nullptr, "skybox");
    m_Shader = ResourceManager::GetShader("skybox");
}
SkyBox::~SkyBox(){
}
void SkyBox::initSkyBox()
{
	float skyboxVertices[] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVBO;
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

bool SkyBox::loadCubeMap()
{
	glGenTextures(1, &m_cubeTextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTextureId);

	std::vector<std::string> images;
	//images.push_back("../../final/src/res/images/SkyBox/riverside_west.BMP");//左
	//images.push_back("../../final/src/res/images/SkyBox/riverside_east.BMP");//右
	//images.push_back("../../final/src/res/images/SkyBox/riverside_up.BMP");//上
	//images.push_back("../../final/src/res/images/SkyBox/riverside_down.BMP");//下
	//images.push_back("../../final/src/res/images/SkyBox/riverside_south.BMP");//后
	//images.push_back("../../final/src/res/images/SkyBox/riverside_north.BMP");//前
	images.push_back("../../final/src/res/images/SkyBox/cloudy/bluecloud_rt.jpg");
	images.push_back("../../final/src/res/images/SkyBox/cloudy/bluecloud_lf.jpg");
	images.push_back("../../final/src/res/images/SkyBox/cloudy/bluecloud_up.jpg");
	images.push_back("../../final/src/res/images/SkyBox/cloudy/bluecloud_dn.jpg");
	images.push_back("../../final/src/res/images/SkyBox/cloudy/bluecloud_bk.jpg");
	images.push_back("../../final/src/res/images/SkyBox/cloudy/bluecloud_ft.jpg");

	int nW = 0, nH = 0, nrChannels = 0;
	unsigned char* pChImg = nullptr;
	for (int i = 0; i < images.size(); i++)
	{
		//stbi_set_flip_vertically_on_load(false);
		pChImg = stbi_load(images[i].c_str(), &nW, &nH, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			nW, nH, 0, GL_RGB, GL_UNSIGNED_BYTE, pChImg);
		stbi_image_free(pChImg);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_Shader.Use();
	m_Shader.SetInteger("skybox", 0);

	return true;
}

void SkyBox::drawSkyBox(glm::mat4 camera_view) 
{
	glDepthFunc(GL_LEQUAL);

	m_Shader.Use();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1280 / (float)720, 0.0f, 1000.0f);
	m_Shader.SetMatrix4("view", camera_view, true);
	m_Shader.SetMatrix4("projection", projection, true);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTextureId);

	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}
