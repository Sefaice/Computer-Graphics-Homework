// glad
#include <glad4.3/glad.h>
// glfw3
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#include "./camera.h"
#include "./models/terrain.h"
#include "./models/skyBox.h"
#include "./models/ocean.h"
#include "./models/Snow.h"
#include "./models/modelObjects.h"
#include "./models/shadow.h"

#include <iostream>
#include <vector>
#include <string.h>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void drawQuad();
void renderCube();
void initSnow(Snow& snow);
void drawSnow(Snow& snow, glm::mat4 view, glm::mat4 projection, float deltaTime);

Camera camera(glm::vec3(0.0, 10.0, 10.0), glm::vec3(0.0, 1.0, 0.0));
float near_plane = 0.1f, far_plane = 100.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

bool deferredShading = true;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "final", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CULL_FACE);

	// init ocean
	Ocean ocean;
	ocean.initOcean(1000, 0.0);
	//// init quad for refl/refr debug
	//Shader quadShader = ResourceManager::LoadShader("../../final/src/shaders/quadShader.vs.glsl", "../../final/src/shaders/quadShader.fs.glsl", nullptr, "quadShader");
	//quadShader.Use();
	//quadShader.SetInteger("ReTexture", 0);

	// init terrain
	Terrain m_terrain(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_terrain.initTerrainShader("terrain");
	m_terrain.vao_vbo_init();

	// init skybox
	SkyBox skybox;
	skybox.initSkyBox();
	skybox.loadCubeMap();

	//// init snow
	//Snow snow;
	//initSnow(snow);

	// init models
	ModelObjects modelObjects;
	modelObjects.initModels();

	// init lightbox
	glm::vec3 lightColor = glm::vec3(1.0);
	glm::vec3 lightPos = glm::vec3(30.0, 30.0, -30.0);
	glm::vec3 lightDir = glm::vec3(-1.0, -1.0, 1.0);
	Shader lightBoxShader = ResourceManager::LoadShader("../../final/src/shaders/lightBoxShader.vs.glsl", "../../final/src/shaders/lightBoxShader.fs.glsl", nullptr, "lightBoxShader");

	// init shadow
	Shadow shadow;
	shadow.initCSMShadow(near_plane, far_plane);
	// shadow debug
	Shader shadowDebugShader = ResourceManager::LoadShader("../../final/src/shaders/shadowDebugDepth.vs.glsl", "../../final/src/shaders/shadowDebugDepth.fs.glsl", nullptr, "shadowDebugDepth");
	shadowDebugShader.Use();
	shadowDebugShader.SetInteger("depthMap", 0);

	// init G-buffer
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedo;
	// init shaders
	Shader gQuadShader = ResourceManager::LoadShader("../../final/src/shaders/gQuadShader.vs.glsl", "../../final/src/shaders/gQuadShader.fs.glsl", nullptr, "gQuadShader");
	if (deferredShading)
	{
		// init fbo
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		
		// - position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		// - normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		// - color + specular color buffer
		glGenTextures(1, &gAlbedo);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
		// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		// create and attach depth buffer (renderbuffer)
		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// set gbuffer texture
		gQuadShader.use();
		gQuadShader.SetInteger("gPositionTexture", 0);
		gQuadShader.SetInteger("gNormalTexture", 1);
		gQuadShader.SetInteger("gAlbedoSpecTexture", 2);
		//gQuadShader.SetInteger("shadowMap", 3);
		gQuadShader.use();
		int depthMapIndexArr[] = { 7, 8, 9 };
		glUniform1iv(glGetUniformLocation(gQuadShader.ID, "depthMap"), 3, depthMapIndexArr);
	}

	// 是否采用线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// show FPS
		int fps = 1 / deltaTime;
		glfwSetWindowTitle(window, ("FPS: " + std::to_string(fps)).c_str());

		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 skyboxView = glm::mat3(view);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);

		// draw reflection & refraction texture
		// ------------------------------------
		// reflection
		// ----------
		glEnable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, ocean.reflectionBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// invert camera
		float cameraHeightY = camera.position.y - ocean.oceanHeight;
		camera.position.y -= 2 * cameraHeightY;
		camera.invertPitch();
		glm::mat4 invertCamView = camera.getViewMatrix();
		glm::mat4 invertCamSkyboxView = glm::mat3(invertCamView);
		// draw skybox
		skybox.drawSkyBox(invertCamSkyboxView);
		// draw terrain
		m_terrain.DrawRefl(invertCamView, projection, glm::vec4(0, 1, 0, 0));
		// draw models
		modelObjects.drawModels(invertCamView, projection, lightDir, lightColor, camera.position);
		// reset camera
		camera.position.y += 2 * cameraHeightY;
		camera.invertPitch();
		// refraction
		// ----------
		glBindFramebuffer(GL_FRAMEBUFFER, ocean.refractionBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw skybox
		skybox.drawSkyBox(skyboxView);
		// draw terrain
		m_terrain.DrawRefl(view, projection, glm::vec4(0, -1, 0, 0));
		glDisable(GL_CLIP_DISTANCE0);
		// ------------------------------------------

		// CSM
		// calc cascade ortho projecitons
		shadow.calcCSMOrthoProjs(view, lightPos);
		for (unsigned int i = 0; i < cascade_num; i++)
		{
			glViewport(0, 0, shadow.shadowWidth, shadow.shadowHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, shadow.depthMapFBOCSM);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow.depthMapCSM[i], 0);
			glDisable(GL_CULL_FACE);
			//glCullFace(GL_FRONT);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadow.depthShader.Use();
			shadow.depthShader.SetMatrix4("lightSpaceMatrix", shadow.lightSpaceMatrixCSM[i]);
			// draw terrain
			m_terrain.DrawDepthMap(shadow.depthShader);
			// draw models
			modelObjects.DrawDepthMap(shadow.depthShader);
			glEnable(GL_CULL_FACE);
			//glCullFace(GL_BACK);
		}
		// -------------------------------------------------
		////// debug shadow
		////// --------------------
		////glBindFramebuffer(GL_FRAMEBUFFER, 0);
		////glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		////glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		////shadowDebugShader.Use();
		////glActiveTexture(GL_TEXTURE0);
		////glBindTexture(GL_TEXTURE_2D, shadow.depthMap);
		////drawQuad();
		// ---------------------------------------------------------------------------

		if (deferredShading)
		{
			// draw G-Buffer
			// -----------------------
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// draw models
			modelObjects.DrawGbuffer(view, projection);
			// draw terrain
			m_terrain.DrawGbuffer(view, projection);
			// ------------------------

			// draw G-Buffer in quad
			// ------------------------
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gQuadShader.Use();
			gQuadShader.SetVector3f("lightDir", lightDir);
			gQuadShader.SetVector3f("lightColor", lightColor);
			glUniformMatrix4fv(glGetUniformLocation(gQuadShader.ID, "lightSpaceMatrix"), cascade_num, GL_FALSE, &shadow.lightSpaceMatrixCSM[0][0][0]);
			float cascadeEndClipSpaceZ[cascade_num];
			for (unsigned int i = 0; i < cascade_num; i++)
			{
				glm::vec4 cascadeEndClipSpace = projection * glm::vec4(0.0, 0.0, -shadow.cascadeZ[i + 1], 1.0);
				cascadeEndClipSpaceZ[i] = cascadeEndClipSpace.z;
			}
			glUniform1fv(glGetUniformLocation(gQuadShader.ID, "cascadeEndClipSpace"), cascade_num, &cascadeEndClipSpaceZ[0]);
			gQuadShader.SetMatrix4("view", view);
			gQuadShader.SetMatrix4("projection", projection);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gAlbedo);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, shadow.depthMapCSM[0]);
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, shadow.depthMapCSM[1]);
			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, shadow.depthMapCSM[2]);
			drawQuad();
			// ----------------------------------------------

			// draw scene
			// ----------------
			// copy depth buffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// draw ocean
			ocean.oceanShader.Use();
			glUniformMatrix4fv(glGetUniformLocation(ocean.oceanShader.ID, "lightSpaceMatrix"), cascade_num, GL_FALSE, &shadow.lightSpaceMatrixCSM[0][0][0]);
			glUniform1fv(glGetUniformLocation(ocean.oceanShader.ID, "cascadeEndClipSpace"), cascade_num, &cascadeEndClipSpaceZ[0]);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, shadow.depthMapCSM[0]);
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, shadow.depthMapCSM[1]);
			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, shadow.depthMapCSM[2]);
			ocean.drawOcean(view, projection, glfwGetTime(), lightDir, lightColor, camera.position);
			// draw skybox
			skybox.drawSkyBox(skyboxView);
			// --------------------------------------
		}
		else
		{
			//// draw scene
			//// ----------------
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//// draw ocean
			//ocean.drawOcean(view, projection, glfwGetTime(), lightDir, lightColor, camera.position);
			//// draw terrain
			//m_terrain.Draw(view, projection, shadow.lightSpaceMatrix, shadow.depthMap, lightDir, lightColor, camera.position);
			//// draw models
			//modelObjects.drawModels(view, projection, lightDir, lightColor, camera.position);
			//// draw skybox
			//skybox.drawSkyBox(skyboxView);
			//// --------------------------------------
		}
	
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBack(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.rotate(yoffset, xoffset);
}

unsigned int quadVAO = 0;
unsigned int quadVBO = 0;
void drawQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
																  // front face
																  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
																  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
																  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
																  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
																  -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
																  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
																														// left face
																														-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
																														-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
																														-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
																														-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
																														-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
																														-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
																																											  // right face
																																											  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
																																											  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
																																											  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
																																											  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
																																											  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
																																											  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
																																																								   // bottom face
																																																								   -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
																																																								   1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
																																																								   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
																																																								   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
																																																								   -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
																																																								   -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
																																																																						 // top face
																																																																						 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
																																																																						 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
																																																																						 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
																																																																						 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
																																																																						 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
																																																																						 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


void initSnow(Snow& snow) 
{
	GLfloat near_plane = 1.0f, far_plane = 1000.0f;
	GLfloat ws = 1000.0f;
	glm::mat4 lightProjection = glm::ortho(-ws, ws, -ws, ws, near_plane, far_plane);
	glm::mat4 lightView = camera.getViewMatrix();
	glm::mat4 lightSpaceMatrix1 = lightProjection * lightView;
	snow.setLightSpace(lightSpaceMatrix1);
}

void drawSnow(Snow& snow, glm::mat4 view, glm::mat4 projection, float deltaTime) 
{
	glm::mat4 model(1.0f);
	//model=glm::translate(model, glm::vec3());
	snow.Render(deltaTime, model, view, projection);
	snow.RenderShadow(deltaTime);
}
