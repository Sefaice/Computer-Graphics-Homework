#include <stdio.h>

#include "terrain.h"
//#include <SOIL.h>

//#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <iostream>
#include <cstring>

Terrain::Terrain(): Position(0, 0, 0), Size(1, 1, 1) { }

Terrain::Terrain(glm::vec3 pos, glm::vec3 size)
: Position(pos), Size(size){
	int nrChannels;
    //unsigned char* image = stbi_load("../../final/src/res/images/plane-smoother.jpg", &width, &height, &nrChannels, 0);
    unsigned char* image = stbi_load("../../final/src/res/images/plane-blur1.jpg", &width, &height, &nrChannels, 0);
    // printf("%d ,%d ,%d  :%d  ,%d ,%d \n",width,height,image[99],image[100],image[240500],image[240600]);
    m_vertexs.clear();
	m_normals.clear();
    m_textures.clear();
	for (int i = 0; i < height - 1; i++)   // 0 1 2 |0| 3 4 5 |1|
	{
		for (int k = 0; k < width - 1; k++)
		{
			// vertices and texcoords
			glm::vec3 p = glm::vec3(k, image[3 * (i * width + k)], i);
			m_vertexs.push_back(p);
			m_textures.push_back(glm::vec2((float)k / (float)width * 10.0f, (float)i / (float)height * 10.0f));
			
			// normals, average of 4 neighbor triangles
			if (i == 0 || i == height - 2 || k == 0 || k == width - 2)
			{
				m_normals.push_back(glm::vec3(0, 1, 0));
				continue;
			}
			glm::vec3 p1 = glm::vec3((k - 1), image[3 * (i * width + (k - 1))], i); // left
			glm::vec3 p2 = glm::vec3(k, image[3 * ((i - 1) * width + k)], (i - 1)); // top
			glm::vec3 p3 = glm::vec3((k + 1), image[3 * (i * width + (k + 1))], i); // right
			glm::vec3 p4 = glm::vec3(k, image[3 * ((i + 1) * width + k)], (i + 1)); // down
			glm::vec3 e1 = p1 - p;
			glm::vec3 e2 = p2 - p;
			glm::vec3 e3 = p3 - p;
			glm::vec3 e4 = p4 - p;
			glm::vec3 n1 = glm::normalize(glm::cross(e2, e1));
			glm::vec3 n2 = glm::normalize(glm::cross(e3, e2));
			glm::vec3 n3 = glm::normalize(glm::cross(e4, e3));
			glm::vec3 n4 = glm::normalize(glm::cross(e1, e4));
			glm::vec3 n = glm::normalize(n1 + n2 + n3 + n4);
			m_normals.push_back(n);
		}
	}
}

Shader& Terrain::initTerrainShader(const char* name) 
{
	ResourceManager::LoadShader("../../final/src/shaders/terrain/terrain.vs.glsl", "../../final/src/shaders/terrain/terrain.fs.glsl", nullptr, name);
	this->shader = ResourceManager::GetShader(name);

	ResourceManager::LoadTexture("../../final/src/res/images/sand.jpg", false, "sand");
	ResourceManager::LoadTexture("../../final/src/res/images/grass2.jpg", false, "grass");
	ResourceManager::LoadTexture("../../final/src/res/images/rock.jpeg", false, "snowfield");

	this->shader.Use();
	this->shader.SetInteger("texture0", 6, true);
	this->shader.SetInteger("texture1", 7, true);
	this->shader.SetInteger("texture2", 8, true);
	this->shader.SetInteger("shadowMap", 9, true);

	// gbuffer shader
	Shader gShader = ResourceManager::LoadShader("../../final/src/shaders/terrain/terrainGbuffer.vs.glsl", "../../final/src/shaders/terrain/terrainGbuffer.fs.glsl", nullptr, "terrainGbuffer");
	gShader.Use();
	gShader.SetInteger("texture0", 6, true);
	gShader.SetInteger("texture1", 7, true);
	gShader.SetInteger("texture2", 8, true);

	return this->shader;
}

void Terrain::vao_vbo_init()
{
	std::vector<unsigned int> indices;
	unsigned int eWidth = this->width - 1;
	unsigned int eHeight = this->height - 1;
	bool oddRow = false;
	for (int y = 0; y < eHeight - 1; y++) {
		if (!oddRow) {
			for (int x = 0; x < eWidth; x++) {
				indices.push_back(y * eWidth + x);
				indices.push_back((y + 1) * eWidth + x);
			}
		}
		else {
			for (int x = eWidth - 1; x >= 0; x--) {
				indices.push_back((y + 1) * eWidth + x);
				indices.push_back(y * eWidth + x);
			}
		}
		oddRow = !oddRow;
	}
	this->terrainIndexCount = indices.size();

	for (int i = 0; i < m_vertexs.size(); i++)
	{
		m_total.push_back(m_vertexs[i].x);
		m_total.push_back(m_vertexs[i].y);
		m_total.push_back(m_vertexs[i].z);
		m_total.push_back(m_normals[i].x);
		m_total.push_back(m_normals[i].y);
		m_total.push_back(m_normals[i].z);
		m_total.push_back(m_textures[i].x);
		m_total.push_back(m_textures[i].y);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_total.size() * sizeof(float), &m_total[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// EBO
	unsigned int terrainEBO;
	glGenBuffers(1, &terrainEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Terrain::DrawRefl(glm::mat4 view, glm::mat4 projection, glm::vec4 oceanPlane)
{
    shader.Use();
	Texture2D sand = ResourceManager::GetTexture("sand");
	Texture2D grass = ResourceManager::GetTexture("grass");
	Texture2D snowfield = ResourceManager::GetTexture("snowfield");
	glActiveTexture(GL_TEXTURE6);
	sand.Bind();
    glActiveTexture(GL_TEXTURE7);
	grass.Bind();
    glActiveTexture(GL_TEXTURE8);
	snowfield.Bind();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-width * 0.5f * 0.1f, -1.1f, -height * 0.5f * 0.1f));
	model = glm::scale(model, glm::vec3(0.1f));
	shader.SetMatrix4("model", model);
	shader.SetMatrix4("view", view);
	shader.SetMatrix4("projection", projection);
	shader.SetVector4f("oceanPlane", oceanPlane);

	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, (int)m_vertexs.size());
	glDrawElements(GL_TRIANGLE_STRIP, this->terrainIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Terrain::DrawDepthMap(Shader& depthShader)
{
	depthShader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-width * 0.5f * 0.1f, -1.1f, -height * 0.5f * 0.1f));
	model = glm::scale(model, glm::vec3(0.1f));
	depthShader.SetMatrix4("model", model);
	//depthShader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
	
	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, (int)m_vertexs.size());
	glDrawElements(GL_TRIANGLE_STRIP, this->terrainIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Terrain::Draw(glm::mat4 view, glm::mat4 projection, glm::mat4 lightSpaceMatrix, unsigned int shadowMap, 
	glm::vec3 lightDir, glm::vec3 lightColor, glm::vec3 viewPos)
{
	shader.Use();
	Texture2D snowfield = ResourceManager::GetTexture("snowfield");
	Texture2D grass = ResourceManager::GetTexture("grass");
	Texture2D sand = ResourceManager::GetTexture("sand");
	glActiveTexture(GL_TEXTURE6);
	sand.Bind();
	glActiveTexture(GL_TEXTURE7);
	grass.Bind();
	glActiveTexture(GL_TEXTURE8);
	snowfield.Bind();
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-width * 0.5f * 0.1f, -1.1f, -height * 0.5f * 0.1f));
	model = glm::scale(model, glm::vec3(0.1f));
	shader.SetMatrix4("model", model);
	shader.SetMatrix4("view", view);
	shader.SetMatrix4("projection", projection);
	shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
	shader.SetVector3f("lightDir", lightDir);
	shader.SetVector3f("lightColor", lightColor);
	shader.SetVector3f("viewPos", viewPos);

	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, (int)m_vertexs.size());
	glDrawElements(GL_TRIANGLE_STRIP, this->terrainIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Terrain::DrawGbuffer(glm::mat4 view, glm::mat4 projection)
{
	Shader gShader = ResourceManager::GetShader("terrainGbuffer");
	gShader.Use();
	Texture2D snowfield = ResourceManager::GetTexture("snowfield");
	Texture2D grass = ResourceManager::GetTexture("grass");
	Texture2D sand = ResourceManager::GetTexture("sand");
	glActiveTexture(GL_TEXTURE6);
	sand.Bind();
	glActiveTexture(GL_TEXTURE7);
	grass.Bind();
	glActiveTexture(GL_TEXTURE8);
	snowfield.Bind();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-width * 0.5f * 0.1f, -1.1f, -height * 0.5f * 0.1f));
	model = glm::scale(model, glm::vec3(0.1f));
	gShader.SetMatrix4("model", model);
	gShader.SetMatrix4("view", view);
	gShader.SetMatrix4("projection", projection);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, this->terrainIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}