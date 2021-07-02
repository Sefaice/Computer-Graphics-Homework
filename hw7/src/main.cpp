// glad
#include <glad/glad.h>
// glfw3
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "../../common/shader.h"
#include "./camera_quaternion.h"

#include <iostream>
#include <vector>
#include <limits>

// 屏幕尺寸变量
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void calcOrthoProjs();
void drawSceneWithModel(const Shader &shader);
void drawCube();
void drawSphere();
void drawQuad();
void drawToolBar();

glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 object_color = glm::vec4(0.5f, 0.6f, 0.7f, 1.0f);
glm::vec3 light_pos = glm::vec3(-8.0, 6.0, 0.0);

glm::mat4 view, lightView, lightProjection[3], lightSpaceMatrix[3];
float near_plane = 0.1f, far_plane = 25.0f;
const unsigned int cascade_num = 3;
float cascadeZ[] = {near_plane, 8.0f, 17.0f, far_plane};
float cascadeOverlap = 1.1f;
const unsigned int SHADOW_WIDTH = 1280, SHADOW_HEIGHT = 1280;
// DEBUG
// ---------------------
float dxn, dxf, dyn, dyf, dzn, dzf;
// -----------------------

// imgui
bool my_tool_active = true;

Camera camera(glm::vec3(0.0, 4.0, 12.0), glm::vec3(0.0, 1.0, 0.0));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;
float fov = 45.0f;

int main()
{
	// 初始化配置glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "homework", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// 绑定窗口大小改变的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	// config mouse display
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 用glad加载函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// 初始化IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// 调用Shader类加载着色器代码
	Shader depthShader("../../hw7/src/depth.vs.glsl", "../../hw7/src/depth.fs.glsl");
	Shader pShader("../../hw7/src/phong.vs.glsl", "../../hw7/src/phong.fs.glsl");

	// framebuffer configuration
	// -------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// use depth texture for reading
	unsigned int depthMap[cascade_num];
	glGenTextures(cascade_num, depthMap);
	for (unsigned int i = 0; i < cascade_num; i++)
	{
		glBindTexture(GL_TEXTURE_2D, depthMap[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// set to this to avoid outside in shadow
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	// bind texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[0], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pShader.use();
	int depthMapIndexArr[] = {0, 1, 2};
	glUniform1iv(glGetUniformLocation(pShader.ID, "depthMap"), 3, depthMapIndexArr);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// 计算时间间隔
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 用户输入
		processInput(window);

		// render scene into framebuffer from light's point of view
		// --------------------------------------------------------
		// calc cascade ortho projecitons
		calcOrthoProjs();
		for (unsigned int i = 0; i < cascade_num; i++)
		{
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
			//glDisable(GL_CULL_FACE);
			//glCullFace(GL_FRONT);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			depthShader.use();
			//lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
			//lightProjection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);
			depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix[i]);
			// draw scene
			drawSceneWithModel(depthShader);
			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_BACK);
		}

		// render quad with depth buffer
		// -----------------------------
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);
		pShader.setMat4("model", model);
		pShader.setMat4("view", view);
		pShader.setMat4("projection", projection);
		pShader.setVec3("lightColor", light_color);
		pShader.setVec3("lightPos", light_pos);
		pShader.setVec3("objectColor", object_color);
		pShader.setVec3("viewPos", camera.position);
		glUniformMatrix4fv(glGetUniformLocation(pShader.ID, "lightSpaceMatrix"), cascade_num, GL_FALSE, &lightSpaceMatrix[0][0][0]);
		// DEBUG
		// ------------------
		pShader.setFloat("dxn", dxn);
		pShader.setFloat("dxf", dxf);
		pShader.setFloat("dyn", dyn);
		pShader.setFloat("dyf", dyf);
		pShader.setFloat("dzn", dzn);
		pShader.setFloat("dzf", dzf);
		// ------------------
		float cascadeEndClipSpaceZ[cascade_num];
		for (unsigned int i = 0; i < cascade_num; i++)
		{
			glm::vec4 cascadeEndClipSpace = projection * glm::vec4(0.0, 0.0, -cascadeZ[i+1], 1.0);
			cascadeEndClipSpaceZ[i] = cascadeEndClipSpace.z;
		}
		glUniform1fv(glGetUniformLocation(pShader.ID, "cascadeEndClipSpace"), cascade_num, &cascadeEndClipSpaceZ[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap[2]);
		// draw scene
		drawSceneWithModel(pShader);

		//drawToolBar();
	
		// 检查调用事件，交换缓冲
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// IMGUI Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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

void calcOrthoProjs()
{
	// camera view matrix and inverse
	view = camera.getViewMatrix();
	glm::mat4 viewinv = glm::inverse(view);

	// light space matrix
	lightView = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	
	float ar = (float)SCR_WIDTH / (float)SCR_HEIGHT;
	float tanHalfHFOV = tanf(glm::radians(fov / 2.0f)) * ar;
	float tanHalfVFOV = tanf(glm::radians(fov / 2.0f));

	for (unsigned int i = 0; i < cascade_num; i++)
	{
		// cascade frustum in camera coords
		float zn = cascadeZ[i];
		float zf = cascadeZ[i + 1];
		float xn = zn * tanHalfHFOV;
		float xf = zf * tanHalfHFOV;
		float yn = zn * tanHalfVFOV;
		float yf = zf * tanHalfVFOV;
		// DEBUG
		// -----------------------------------------------
		//std::cout << xn << " " << xf << " " << ynn << " " << yf << " " << zn << " " << zf << std::endl;
		if (i == 1)
		{
			dxn = xn; dxf = xf; dyn = yn; dyf = yf; dzn = -zn; dzf = -zf;
		}
		// --------------------------------------------------

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
			//std::cout << vW.x << " " << vW.y << " " << vW.z << std::endl;
			// transfer to lightview coords
			frustumCornersLight[j] = lightView * vW;
			//std::cout << frustumCornersLight[j].x << " " << frustumCornersLight[j].y << " " << frustumCornersLight[j].z << std::endl;
			// calc the AABB border of the depth map
			minX = glm::min(minX, frustumCornersLight[j].x);
			maxX = glm::max(maxX, frustumCornersLight[j].x);
			minY = glm::min(minY, frustumCornersLight[j].y);
			maxY = glm::max(maxY, frustumCornersLight[j].y);
			minZ = glm::min(minZ, frustumCornersLight[j].z);
			maxZ = glm::max(maxZ, frustumCornersLight[j].z);
		}
		//std::cout << "111111: "<< minX << " " << maxX << " " << minY << " " << maxY << " " << minZ << " " << maxZ << std::endl;

		// fix shimming edges
		float xUnitPerTexel = (maxX - minX) / SHADOW_WIDTH;
		float yUnitPerTexel = (maxY - minY) / SHADOW_HEIGHT;
		minX = floor(minX / xUnitPerTexel) * xUnitPerTexel;
		maxX = floor(maxX / xUnitPerTexel) * xUnitPerTexel;
		minY = floor(minY / yUnitPerTexel) * yUnitPerTexel;
		maxY = floor(maxY / yUnitPerTexel) * yUnitPerTexel;
		/*minZ = floor(minZ / yUnitPerTexel) * yUnitPerTexel;
		maxZ = floor(maxZ / yUnitPerTexel) * yUnitPerTexel;*/

		//float r = maxX + 3.0f, l = minX - 3.0f, t = maxY + 3.0f, b = minY - 3.0f,
		float r = maxX * cascadeOverlap, l = minX * cascadeOverlap, t = maxY * cascadeOverlap, b = minY * cascadeOverlap,
			n = maxZ + 3.0f, f = minZ - 3.0f;
		// glm::mat4初始化是按列的...
		lightProjection[i] = glm::mat4(2.0f / (r - l),     0.0f,               0.0f,               0.0f,
									   0.0f,               2.0f / (t - b),     0.0f,               0.0f,
									   0.0f,               0.0f,               2.0f / (f - n),     0.0f,
									   -(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f);
		//std::cout << glm::to_string(glm::ortho(minX, maxX, minY, maxY, maxZ, minZ)) << std::endl;
		//std::cout << glm::to_string(lightProjection[i]) << std::endl;
		//std::cout << std::endl;

		lightSpaceMatrix[i] = lightProjection[i] * lightView;
	}
}

void drawSceneWithModel(const Shader &shader)
{
	// cube
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0, 1.0, 5.0));
	//model = glm::rotate(model, (float)sin(glfwGetTime()), glm::vec3(1.0, 1.0, 1.0));
	shader.setMat4("model", model);
	drawCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0, 1.0, 0.0));
	shader.setMat4("model", model);
	drawCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0, 1.0, -5.0));
	shader.setMat4("model", model);
	drawCube();
	// plane
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(30.0, 0.1, 30.0));
	shader.setMat4("model", model);
	drawCube();
	// sphere
	model = glm::translate(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 5.0));
	shader.setMat4("model", model);
	drawSphere();
	model = glm::translate(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 0.0));
	shader.setMat4("model", model);
	drawSphere();
	model = glm::translate(glm::mat4(1.0f), glm::vec3(1.5, 1.5, -5.0));
	shader.setMat4("model", model);
	drawSphere();
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void drawCube()
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

int indexCount;
unsigned int sphereVAO = 0;
void drawSphere()
{
	if (sphereVAO == 0)
	{
		// sphere data
		unsigned int sphereSteps = 64;
		float pi = 3.1415926;
		// count positions, normals and texcoords
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texcoords;
		for (int i = 0; i <= sphereSteps; i++) {
			for (int j = 0; j <= sphereSteps; j++) {
				float xstep = (float)i / (float)sphereSteps;
				float ystep = (float)j / (float)sphereSteps;
				float x = sin(ystep * pi) * cos(xstep * 2 * pi);
				float y = cos(ystep * pi);
				float z = sin(ystep * pi) * sin(xstep * 2 * pi);
				positions.push_back(glm::vec3(x, y, z));
				normals.push_back(glm::vec3(x, y, z));
				texcoords.push_back(glm::vec2(xstep, ystep));
			}
		}
		// indices
		std::vector<unsigned int> indices;
		bool oddRow = false;
		for (int y = 0; y < sphereSteps; y++) {
			if (!oddRow) {
				for (int x = 0; x <= sphereSteps; x++) {
					indices.push_back(y * (sphereSteps + 1) + x);
					indices.push_back((y + 1) * (sphereSteps + 1) + x);
				}
			}
			else {
				for (int x = sphereSteps; x >= 0; x--) {
					indices.push_back((y + 1) * (sphereSteps + 1) + x);
					indices.push_back(y * (sphereSteps + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();
		// put into data
		std::vector<float> data;
		for (int i = 0; i < positions.size(); i++) {
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);

			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);

			data.push_back(texcoords[i].x);
			data.push_back(texcoords[i].y);
		}
		// VBO VAO EBO
		unsigned int sphereVBO, sphereEBO;
		// VBO
		glGenBuffers(1, &sphereVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		// VAO
		glGenVertexArrays(1, &sphereVAO);
		glBindVertexArray(sphereVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		// EBO
		glGenBuffers(1, &sphereEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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

void drawToolBar()
{
	// IMGUI 
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	// Create a window
	ImGui::Begin("Tool Bar", &my_tool_active, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	// Edit a color (stored as ~4 floats)
	/*ImGui::Checkbox("Translation", &my_translation);
	ImGui::Checkbox("Rotation", &my_rotation);
	ImGui::SliderFloat("Scaling", &my_scaling, 0.5f, 2.0f);*/
	
	ImGui::End();
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}