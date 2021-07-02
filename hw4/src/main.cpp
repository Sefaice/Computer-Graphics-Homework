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

#include <iostream>
#include <vector>

// 屏幕尺寸变量
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void drawCube();
void drawToolBar();

// imgui
bool my_tool_active = true;
glm::vec4 my_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
float my_scaling = 1.0f;
bool my_translation = true;
bool my_rotation = true;


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

	// 用glad加载函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	// 调用Shader类加载着色器代码
	Shader myShader("../../hw4/src/vertexShader.vs.glsl", "../../hw4/src/fragmentShader.fs.glsl");

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
		// 用户输入
		processInput(window);

		// 清空颜色缓冲
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw
		myShader.use();
		myShader.setVec4("color", my_color);
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0, 1.0, 1.0));
		if (my_rotation) {
			model = glm::rotate(model, float(sin(glfwGetTime())), glm::vec3(0.0, 0.0, 1.0));
		}
		if (my_translation) {
			model = glm::translate(model, glm::vec3(5.0f * float(sin(glfwGetTime())), 0.0, 0.0));
		}
		model = glm::scale(model, glm::vec3(my_scaling));
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		myShader.setMat4("model", model);
		myShader.setMat4("view", view);
		myShader.setMat4("projection", projection);
		drawCube();
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
}

void drawCube()
{
	float vertices[] = {
		-2.0f, -2.0f, -2.0f,
		2.0f, -2.0f, -2.0f,
		2.0f, 2.0f, -2.0f,
		-2.0f, 2.0f, -2.0f,
		-2.0f, -2.0f, 2.0f,
		2.0f, -2.0f, 2.0f,
		2.0f, 2.0f, 2.0f,
		-2.0f, 2.0f, 2.0f
	};
	unsigned int indices[] = {
		0, 1, 2, // back face
		0, 2, 3,
		5, 1, 2, // right face
		5, 2, 6,
		7, 6, 2, // top face
		7, 2, 3,
		4, 5, 6, // front face
		4, 6, 7,
		0, 4, 7, // left face
		0, 7, 3,
		0, 1, 5, // bottom face
		0, 5, 4
	};

	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
	ImGui::ColorEdit4("Color", (float*)&my_color);
	// vertex input
	ImGui::Checkbox("Translation", &my_translation);
	ImGui::Checkbox("Rotation", &my_rotation);
	ImGui::SliderFloat("Scaling", &my_scaling, 0.5f, 2.0f);
	
	ImGui::End();
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}