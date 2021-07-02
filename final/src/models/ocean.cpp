#include "ocean.h"

void Ocean::initOcean(int _oceanEdge = 100, float _oceanHeight = 0.0)
{
	this->oceanEdge = _oceanEdge;
	this->oceanHeight = _oceanHeight;

	oceanShader = ResourceManager::LoadShader("../../final/src/shaders/oceanShader.vs.glsl", "../../final/src/shaders/oceanShader.fs.glsl", nullptr, "oceanShader");
	ResourceManager::LoadTexture("../../final/src/res/images/water-texture-2.tga", false, "oceanTexture");
	ResourceManager::LoadTexture("../../final/src/res/images/waterNormal.png", false, "oceanNormalTexture");
	ResourceManager::LoadTexture("../../final/src/res/images/waterDUDV.png", false, "oceanDuDvTexture");
	oceanShader.Use();
	oceanShader.SetInteger("OceanTexture", 0);
	oceanShader.SetInteger("OceanNormalTexture", 1);
	oceanShader.SetInteger("Skybox", 2);
	oceanShader.SetInteger("ReflectionTexture", 3);
	oceanShader.SetInteger("RefractionTexture", 4);
	oceanShader.SetInteger("OceanDuDvTexture", 5);
	int depthMapIndexArr[] = { 7, 8, 9 };
	glUniform1iv(glGetUniformLocation(oceanShader.ID, "depthMap"), 3, depthMapIndexArr);

	initOceanVertices();
	initReFBOs();
}

void Ocean::initOceanVertices()
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texcoords;

	for (int i = 0; i < oceanEdge; i++)
	{
		for (int j = 0; j < oceanEdge; j++)
		{
			positions.push_back(glm::vec3(-0.5 + i * 1.0 / (float)oceanEdge, 0, -0.5 + j * 1.0 / (float)oceanEdge));
			texcoords.push_back(glm::vec2((float)i / (float)oceanEdge * 2.0f, (float)j / (float)oceanEdge * 2.0f));
			//texcoords.push_back(glm::vec2((float)i / (float)oceanEdge, (float)j / (float)oceanEdge));
		}
	}

	std::vector<unsigned int> indices;
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
	this->oceanIndexCount = indices.size();

	std::vector<float> data;
	for (int i = 0; i < positions.size(); i++) {
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		data.push_back(texcoords[i].x);
		data.push_back(texcoords[i].y);
	}

	// VBO
	unsigned int oceanVBO;
	glGenBuffers(1, &oceanVBO);
	glBindBuffer(GL_ARRAY_BUFFER, oceanVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	// VAO
	glGenVertexArrays(1, &oceanVAO);
	glBindVertexArray(oceanVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	// EBO
	unsigned int oceanEBO;
	glGenBuffers(1, &oceanEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oceanEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Ocean::initReFBOs()
{
	// init reflection FBO
	glGenFramebuffers(1, &reflectionBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionBuffer);
	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Reflection Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// init refraction FBO
	glGenFramebuffers(1, &refractionBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionBuffer);
	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Refraction Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Ocean::drawOcean(glm::mat4 view, glm::mat4 projection, float t, glm::vec3 lightDir, glm::vec3 lightColor, glm::vec3 cameraPos)
{
	oceanShader.Use();
	glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(40.0, 1.0, 40.0));
	//glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
	oceanShader.SetMatrix4("model", model);
	oceanShader.SetMatrix4("view", view);
	oceanShader.SetMatrix4("projection", projection);
	oceanShader.SetFloat("t", t);
	oceanShader.SetVector3f("lightDir", lightDir);
	oceanShader.SetVector3f("lightColor", lightColor);
	oceanShader.SetVector3f("viewPos", cameraPos);
	glActiveTexture(GL_TEXTURE0);
	ResourceManager::GetTexture("oceanTexture").Bind();
	glActiveTexture(GL_TEXTURE1);
	ResourceManager::GetTexture("oceanNormalTexture").Bind();
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glActiveTexture(GL_TEXTURE5);
	ResourceManager::GetTexture("oceanDuDvTexture").Bind();

	glBindVertexArray(this->oceanVAO);
	glDrawElements(GL_TRIANGLE_STRIP, this->oceanIndexCount, GL_UNSIGNED_INT, 0);

	/*glDeleteVertexArrays(1, &oceanVAO);
	glDeleteBuffers(1, &oceanVBO);
	glDeleteBuffers(1, &oceanEBO);*/
}