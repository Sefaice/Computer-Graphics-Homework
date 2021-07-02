#ifndef MODELOBJECTS_H
#define MODELOBJECTS_H

#include <vector>
#include <map>
#include <string.h>

#include "../resourceManager/model.h"

class ModelObjects
{
private:
	std::map<std::string, Model> Models;

public:
	void initModels();
	void drawModels(glm::mat4 view, glm::mat4 projection, glm::vec3 lightDir, glm::vec3 lightColor, glm::vec3 viewPos);
	void DrawDepthMap(Shader& depthShader);
	void DrawGbuffer(glm::mat4 view, glm::mat4 projection);
};

#endif