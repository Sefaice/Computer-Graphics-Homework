#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad4.3/glad.h>

#include <vector>
#include <iostream>

#include "../resourceManager/resource_manager.h"

class Ocean
{
private:
	int oceanEdge;
	unsigned int oceanVAO;
	unsigned int oceanIndexCount;

public:
	Shader oceanShader;
	float oceanHeight;
	unsigned int reflectionBuffer, refractionBuffer;
	unsigned int reflectionTexture, refractionTexture;

	void initOcean(int _oceanEdge, float _oceanHeight);
	void initOceanVertices();
	void initReFBOs();
	void drawOcean(glm::mat4 view, glm::mat4 projection, float t, glm::vec3 lightDir, glm::vec3 lightColor, glm::vec3 cameraPos);
};