#include "shadow.h"
#include <iostream>

Shadow::Shadow() {}

void Shadow::initShadow(glm::vec3 lightPos)
{
	this->depthShader = ResourceManager::LoadShader("../../final/src/shaders/shadowDepthShader.vs.glsl", 
		"../../final/src/shaders/shadowDepthShader.fs.glsl", nullptr, "shadowDepthShader");
	this->shadowWidth = 1024;
	this->shadowHeight = 1024;
	
	// framebuffer
	glGenFramebuffers(1, &depthMapFBO);
	// gen texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// set to this to avoid outside in shadow
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// bind texture to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::mat4 lightProjection, lightView;
	float near_plane = 1.0f, far_plane = 100.0f;
	lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
	//lightProjection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
}

void Shadow::initCSMShadow(float near_plane, float far_plane)
{
	this->depthShader = ResourceManager::LoadShader("../../final/src/shaders/shadowDepthShader.vs.glsl",
		"../../final/src/shaders/shadowDepthShader.fs.glsl", nullptr, "shadowDepthShader");
	this->shadowWidth = 1024 * 3;
	this->shadowHeight = 1024 * 3;

	float cascadeOverlap = 1.1f;
	this->cascadeZ[0] = near_plane;
	this->cascadeZ[1] = 15.0f;
	this->cascadeZ[2] = 30.0f;
	this->cascadeZ[3] = far_plane;

	// framebuffer configuration
	// -------------------------
	glGenFramebuffers(1, &depthMapFBOCSM);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOCSM);
	// use depth texture for reading
	glGenTextures(cascade_num, depthMapCSM);
	for (unsigned int i = 0; i < cascade_num; i++)
	{
		glBindTexture(GL_TEXTURE_2D, depthMapCSM[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// set to this to avoid outside in shadow
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	// bind texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapCSM[0], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::calcCSMOrthoProjs(glm::mat4 view, glm::vec3 lightPos)
{
	// camera view matrix and inverse
	glm::mat4 viewinv = glm::inverse(view);

	// light space matrix
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

	float ar = (float)1280 / (float)720;
	float fov = 45.0f;
	float tanHalfHFOV = tanf(glm::radians(fov * ar / 2.0f));
	float tanHalfVFOV = tanf(glm::radians(fov / 2.0f));

	float cascadeOverlap = 1.1f;

	for (unsigned int i = 0; i < cascade_num; i++)
	{
		// cascade frustum in camera coords
		float zn = cascadeZ[i];
		float zf = cascadeZ[i + 1];
		float xn = zn * tanHalfHFOV;
		float xf = zf * tanHalfHFOV;
		float yn = zn * tanHalfVFOV;
		float yf = zf * tanHalfVFOV;

		glm::vec4 frustumCorners[8] = {
			glm::vec4(xn, yn, -zn, 1.0),
			glm::vec4(-xn, yn, -zn, 1.0),
			glm::vec4(xn, -yn, -zn, 1.0),
			glm::vec4(-xn, -yn, -zn, 1.0),
			glm::vec4(xf, yf, -zf, 1.0),
			glm::vec4(-xf, yf, -zf, 1.0),
			glm::vec4(xf, -yf, -zf, 1.0),
			glm::vec4(-xf, -yf, -zf, 1.0)
		};
		glm::vec4 frustumCornersLight[8];
		float minX = std::numeric_limits<float>::max();
		float maxX = -std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();
		float maxY = -std::numeric_limits<float>::max();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = -std::numeric_limits<float>::max();
		for (unsigned int j = 0; j < 8; j++)
		{
			// transfer back into world coords
			glm::vec4 vW = viewinv * frustumCorners[j];
			// transfer to lightview coords
			frustumCornersLight[j] = lightView * vW;
			// calc the AABB border of the depth map
			minX = glm::min(minX, frustumCornersLight[j].x);
			maxX = glm::max(maxX, frustumCornersLight[j].x);
			minY = glm::min(minY, frustumCornersLight[j].y);
			maxY = glm::max(maxY, frustumCornersLight[j].y);
			minZ = glm::min(minZ, frustumCornersLight[j].z);
			maxZ = glm::max(maxZ, frustumCornersLight[j].z);
		}

		// fix shimming edges
		float xUnitPerTexel = (maxX - minX) / this->shadowWidth;
		float yUnitPerTexel = (maxY - minY) / this->shadowHeight;
		minX = floor(minX / xUnitPerTexel) * xUnitPerTexel;
		maxX = floor(maxX / xUnitPerTexel) * xUnitPerTexel;
		minY = floor(minY / yUnitPerTexel) * yUnitPerTexel;
		maxY = floor(maxY / yUnitPerTexel) * yUnitPerTexel;
		minZ = floor(minZ / yUnitPerTexel) * yUnitPerTexel;
		maxZ = floor(maxZ / yUnitPerTexel) * yUnitPerTexel;

		//float r = maxX, l = minX, t = maxY, b = minY, n = maxZ, f = minZ;
		//float r = maxX + 3.0f, l = minX - 3.0f, t = maxY + 3.0f, b = minY - 3.0f, n = maxZ + 20.0f, f = minZ - 20.0f;
		float r = maxX * cascadeOverlap, l = minX * cascadeOverlap, t = maxY * cascadeOverlap, b = minY * cascadeOverlap, n = maxZ + 20.0f, f = minZ - 20.0f;
		// glm::mat4初始化是按列的...
		glm::mat4 lightProjectionCSM = glm::mat4(
			2.0f / (r - l), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (t - b), 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f / (f - n), 0.0f,
			-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f
		);

		lightSpaceMatrixCSM[i] = lightProjectionCSM * lightView;
	}
}
