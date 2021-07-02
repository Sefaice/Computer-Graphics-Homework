#include "modelObjects.h"

void ModelObjects::initModels()
{
	ResourceManager::LoadShader("../../final/src/shaders/model/model.vs.glsl", "../../final/src/shaders/model/model.fs.glsl", nullptr, "model_shader");
	ResourceManager::LoadShader("../../final/src/shaders/model/modelGbuffer.vs.glsl", "../../final/src/shaders/model/modelGbuffer.fs.glsl", nullptr, "modelGbuffer");

	Model bamboo("../../final/src/res/models/bamboo/bamboo.obj");
	Models["bamboo"] = bamboo;

	Model bamboo2("../../final/src/res/models/bamboo2/bamboo2.obj");
	Models["bamboo2"] = bamboo2;

	/*Model house("../../final/src/res/models/house/house.obj");
	Models["house"] = house;*/
}

void ModelObjects::drawModels(glm::mat4 view, glm::mat4 projection, glm::vec3 lightDir, glm::vec3 lightColor, glm::vec3 viewPos)
{
	// Alpha测试，但这个方法被舍弃了，在shader手动实现
	//glEnable(GL_ALPHA_TEST);  
	//glAlphaFunc(GL_GREATER, 0.5f);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader model_shader = ResourceManager::GetShader("model_shader");
	model_shader.Use();
	model_shader.SetMatrix4("projection", projection);
	model_shader.SetMatrix4("view", view);
	model_shader.SetVector3f("lightDir", lightDir);
	model_shader.SetVector3f("lightColor", lightColor);
	model_shader.SetVector3f("viewPos", viewPos);

	//bamboo
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(30.0f, 0.0f, 200.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
	model_shader.SetMatrix4("model", model);
	Models["bamboo"].Draw(model_shader);

	//bamboo2
	model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(60.0f, 0.0f, 220.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));    // it's a bit too big for our scene, so scale it down
	model_shader.SetMatrix4("model", model);
	Models["bamboo2"].Draw(model_shader);

	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(90.0f, 0.0f, 220.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
	//model_shader.SetMatrix4("model", model);
	//Models["bamboo2"].Draw(model_shader);

	////house
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -12.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
	//model_shader.SetMatrix4("model", model);
	//Models["house"].Draw(model_shader);
}

void ModelObjects::DrawDepthMap(Shader& depthShader)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	depthShader.Use();
	//depthShader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

	//bamboo
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(30.0f, 0.0f, 200.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
	depthShader.SetMatrix4("model", model);
	Models["bamboo"].Draw(depthShader);

	//bamboo2
	model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(60.0f, 0.0f, 220.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));    // it's a bit too big for our scene, so scale it down
	depthShader.SetMatrix4("model", model);
	Models["bamboo2"].Draw(depthShader);

	////house
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -12.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
	//depthShader.SetMatrix4("model", model);
	//Models["house"].Draw(depthShader);
}

void ModelObjects::DrawGbuffer(glm::mat4 view, glm::mat4 projection)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader gShader = ResourceManager::GetShader("modelGbuffer");
	gShader.Use();
	gShader.SetMatrix4("projection", projection);
	gShader.SetMatrix4("view", view);

	//bamboo
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(30.0f, 0.0f, 200.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
	gShader.SetMatrix4("model", model);
	Models["bamboo"].Draw(gShader);

	//bamboo2
	model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(60.0f, 0.0f, 220.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));    // it's a bit too big for our scene, so scale it down
	gShader.SetMatrix4("model", model);
	Models["bamboo2"].Draw(gShader);

	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(90.0f, 0.0f, 220.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
	//gShader.SetMatrix4("model", model);
	//Models["bamboo2"].Draw(gShader);

	////house
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -12.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
	//gShader.SetMatrix4("model", model);
	//Models["house"].Draw(gShader);
}

