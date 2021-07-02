// glad
#include <glad/glad.h>
// glfw3
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "../../common/shader.h"
#include "./camera_quaternion.h"

#include <iostream>
#include <vector>

// 屏幕尺寸变量
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawCube();
void drawSphere();
void drawToolBar();

// imgui
bool my_tool_active = true;
bool phong = true;
bool ifDrawCube = true;
glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 object_color = glm::vec4(0.5f, 0.6f, 0.7f, 1.0f);
//glm::vec3 light_pos = glm::vec3(0.5, 2.0, 3.0);
glm::vec3 light_pos = glm::vec3(0.0, 3.0, 3.0);
float ambientStrength = 0.1;
float diffuseStrength = 1.0;
float specularStrength = 0.5;
int specularPower = 32;

Camera camera(glm::vec3(0.0, 0.0, 15.0), glm::vec3(0.0, 1.0, 0.0));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 用glad加载函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	// 调用Shader类加载着色器代码
	Shader pShader("../../hw6/src/phong.vs.glsl", "../../hw6/src/phong.fs.glsl");
	Shader gShader("../../hw6/src/gouraud.vs.glsl", "../../hw6/src/gouraud.fs.glsl");

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

	// 是否采用线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// 计算时间间隔
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 用户输入
		processInput(window);

		// 清空颜色缓冲
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw
		if (phong)
			pShader.use();
		else
			gShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(1.0, 1.0, 1.0));
		model = glm::scale(model, glm::vec3(4.0));
		float camPosX = sin(glfwGetTime()) * 10.0f;
		float camPosZ = cos(glfwGetTime()) * 10.0f;
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		if (phong)
		{
			pShader.setMat4("model", model);
			pShader.setMat4("view", view);
			pShader.setMat4("projection", projection);
			pShader.setVec3("lightColor", light_color);
			pShader.setVec3("lightPos", light_pos);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
			pShader.setVec3("objectColor", object_color);
			pShader.setVec3("viewPos", camera.position);
			pShader.setFloat("ambientStrength", ambientStrength);
			pShader.setFloat("diffuseStrength", diffuseStrength);
			pShader.setFloat("specularStrength", specularStrength);
			pShader.setInt("specularPower", specularPower);
		}
		else
		{
			gShader.setMat4("model", model);
			gShader.setMat4("view", view);
			gShader.setMat4("projection", projection);
			gShader.setVec3("lightColor", light_color);
			gShader.setVec3("lightPos", light_pos);
			gShader.setVec3("objectColor", object_color);
			gShader.setVec3("viewPos", camera.position);
			gShader.setFloat("ambientStrength", ambientStrength);
			gShader.setFloat("diffuseStrength", diffuseStrength);
			gShader.setFloat("specularStrength", specularStrength);
			gShader.setInt("specularPower", specularPower);
		}
		
		if(ifDrawCube)
			drawCube();
		else
			drawSphere();
	
		drawToolBar();
	
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
	/*if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.rotate(yoffset, xoffset);*/
}


unsigned int cubeVAO = 0;
void drawCube()
{
	if (cubeVAO == 0)
	{
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
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
		// count positions and normals
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		//std::vector<glm::vec2> texcoords;
		for (int i = 0; i <= sphereSteps; i++) {
			for (int j = 0; j <= sphereSteps; j++) {
				float xstep = (float)i / (float)sphereSteps;
				float ystep = (float)j / (float)sphereSteps;
				float x = sin(ystep * pi) * cos(xstep * 2 * pi);
				float y = cos(ystep * pi);
				float z = sin(ystep * pi) * sin(xstep * 2 * pi);
				positions.push_back(glm::vec3(x, y, z));
				normals.push_back(glm::vec3(x, y, z));
				//texcoords.push_back(glm::vec2(xstep, ystep));
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

			/*data.push_back(texcoords[i].x);
			data.push_back(texcoords[i].y);*/
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
		/*glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));*/
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		// EBO
		glGenBuffers(1, &sphereEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
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
	ImGui::Checkbox("Phong Model", &phong);
	ImGui::Checkbox("Draw Cube", &ifDrawCube);
	ImGui::ColorEdit4("Light Color", (float*)&light_color);
	ImGui::ColorEdit4("Object Color", (float*)&object_color);
	ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("diffuseStrength", &diffuseStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f);
	ImGui::SliderInt("specularPower", &specularPower, 1, 128);
	/*ImGui::Checkbox("Translation", &my_translation);
	ImGui::Checkbox("Rotation", &my_rotation);
	ImGui::SliderFloat("Scaling", &my_scaling, 0.5f, 2.0f);*/
	
	ImGui::End();
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}