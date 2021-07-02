#include "../resourceManager/resource_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

const unsigned int cascade_num = 3;

class Shadow
{
private:
	

public:
	Shader depthShader;
	unsigned int shadowWidth;
	unsigned int shadowHeight;
	// normal shadow mapping
	unsigned int depthMap;
	unsigned int depthMapFBO;
	glm::mat4 lightSpaceMatrix;
	// CSM
	unsigned int depthMapCSM[3];
	unsigned int depthMapFBOCSM;
	float cascadeZ[4];
	glm::mat4 lightSpaceMatrixCSM[3];

	Shadow();
	void initShadow(glm::vec3 lightPos);
	void initCSMShadow(float near_plane, float far_plane);
	void calcCSMOrthoProjs(glm::mat4 view, glm::vec3 lightPos);
};