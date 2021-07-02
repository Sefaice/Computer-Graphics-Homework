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
void drawTriangle();
void drawLineFloat(float x0, float y0, float x1, float y1);
void drawLineInt(int x0, int y0, int x1, int y1);
void drawFilledTriangle();
bool isInTriangle(int x, int y);
void drawCircle();
void drawToolBar();

float stepx = (float)2 / (float)SCR_WIDTH;
float stepy = (float)2 / (float)SCR_HEIGHT;
int triangleVertices[6];
int circlex = 0, circley = 0, circler = 0;
// imgui
bool my_tool_active = true;
glm::vec4 my_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
bool fill_triangle = false;

int main()
{
	// 初始化配置glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2", NULL, NULL);
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

	// 调用Shader类加载着色器代码
	Shader myShader("../../hw3/src/vertexShader.vs.glsl", "../../hw3/src/fragmentShader.fs.glsl");

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
		glClear(GL_COLOR_BUFFER_BIT);

		// draw
		myShader.use();
		myShader.setVec4("color", my_color);
		glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
		std::cout << glm::to_string(projection) << std::endl;
		myShader.setMat4("projection", projection);
		//drawLineFloat(0.0, 0.0, -0.4, 0.1 );
		//drawLineInt(0, 0, -240, 135);
		if(fill_triangle)
			drawFilledTriangle();
		else
			drawTriangle();
		drawCircle();
		

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

void drawTriangle()
{
	drawLineInt(triangleVertices[0], triangleVertices[1], triangleVertices[2], triangleVertices[3]);
	drawLineInt(triangleVertices[0], triangleVertices[1], triangleVertices[4], triangleVertices[5]);
	drawLineInt(triangleVertices[2], triangleVertices[3], triangleVertices[4], triangleVertices[5]);
}

void drawLineFloat(float x0, float y0, float x1, float y1)
{
	std::vector<float> points;
	float deltax = (x1 - x0) * 2 / SCR_WIDTH;
	float deltay = (y1 - y0) * 2 / SCR_HEIGHT;

	if (deltax < 0 && deltay < 0) {
		deltax = -deltax;
		deltay = -deltay;
		float tem = x0; x0 = x1; x1 = tem;
		tem = y0; y0 = y1; y1 = tem;
	}
	bool flipSecondQuadrant = false;
	if (deltax < 0 && deltay > 0) {
		flipSecondQuadrant = true;
		deltax = -deltax;
		x0 = -x0; x1 = -x1;
	}
	bool flipFourthQuadrant = false;
	if (deltax > 0 && deltay < 0) {
		flipFourthQuadrant = true;
		deltay = -deltay;
		y0 = -y0; y1 = -y1;
	}
	bool flipFirstQuadrant = false;
	if (deltay > deltax) {
		flipFirstQuadrant = true;
		float tem = deltax; deltax = deltay; deltay = tem;
		tem = x0; x0 = y0; y0 = tem;
		tem = x1; x1 = y1; y1 = tem;
	}

	if (deltax == 0)
		return;

	// core
	float d = 2 * deltay - deltax;
	points.push_back(x0);
	points.push_back(y0);
	float y = y0;
	for (float x = x0; x < x1; x += stepx) {
		if (d >= 0) {
			y += stepy;
			d = d + 2 * (deltay - deltax);
		} else {
			d = d + 2 * deltay;
		}
		points.push_back(x + stepx);
		points.push_back(y);
	}

	if (flipFirstQuadrant)
		for (int i = 0; i < points.size(); i += 2) {
			float tem = points[i]; points[i] = points[i + 1]; points[i + 1] = tem;
		}
	if (flipFourthQuadrant)
		for (int i = 0; i < points.size(); i += 2) 
			points[i + 1] = -points[i + 1];
	if (flipSecondQuadrant)
		for (int i = 0; i < points.size(); i += 2)
			points[i] = -points[i];

	int pointsNum = points.size() / 2;
	//std::cout << pointsNum << std::endl;

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointsNum);
}

void drawLineInt(int x0, int y0, int x1, int y1)
{
	std::vector<float> points;
	int deltax = (x1 - x0);
	int deltay = (y1 - y0);

	if (deltax < 0 && deltay < 0) {
		deltax = -deltax;
		deltay = -deltay;
		int tem = x0; x0 = x1; x1 = tem;
		tem = y0; y0 = y1; y1 = tem;
	}
	bool flipSecondQuadrant = false;
	if (deltax < 0 && deltay > 0) {
		flipSecondQuadrant = true;
		deltax = -deltax;
		x0 = -x0; x1 = -x1;
	}
	bool flipFourthQuadrant = false;
	if (deltax > 0 && deltay < 0) {
		flipFourthQuadrant = true;
		deltay = -deltay;
		y0 = -y0; y1 = -y1;
	}
	bool flipFirstQuadrant = false;
	if (deltay > deltax) {
		flipFirstQuadrant = true;
		int tem = deltax; deltax = deltay; deltay = tem;
		tem = x0; x0 = y0; y0 = tem;
		tem = x1; x1 = y1; y1 = tem;
	}

	if (deltax == 0)
		return;

	// core
	int d = 2 * deltay - deltax;
	points.push_back(x0);
	points.push_back(y0);
	int y = y0;
	for (int x = x0; x < x1; x++) {
		if (d >= 0) {
			y ++;
			d = d + 2 * (deltay - deltax);
		}
		else {
			d = d + 2 * deltay;
		}
		points.push_back(x + 1);
		points.push_back(y);
	}

	if (flipFirstQuadrant)
		for (int i = 0; i < points.size(); i += 2) {
			float tem = points[i]; points[i] = points[i + 1]; points[i + 1] = tem;
		}
	if (flipFourthQuadrant)
		for (int i = 0; i < points.size(); i += 2)
			points[i + 1] = -points[i + 1];
	if (flipSecondQuadrant)
		for (int i = 0; i < points.size(); i += 2)
			points[i] = -points[i];
	/*
	for (int i = 0; i < points.size(); i += 2) {
		points[i] = (float)(points[i] * 2) / (float)SCR_WIDTH;
		points[i+1] = (float)(points[i+1] * 2) / (float)SCR_HEIGHT;
	}
	*/

	int pointsNum = points.size() / 2;
	//std::cout << pointsNum << std::endl;

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointsNum);
}

void drawFilledTriangle()
{
	// init line equations
	int lineParams[9];
	lineParams[0] = triangleVertices[3] - triangleVertices[1]; 
	lineParams[1] = triangleVertices[0] - triangleVertices[2];
	lineParams[2] = triangleVertices[2] * triangleVertices[1] - triangleVertices[0] * triangleVertices[3];
	lineParams[3] = triangleVertices[5] - triangleVertices[3];
	lineParams[4] = triangleVertices[2] - triangleVertices[4];
	lineParams[5] = triangleVertices[4] * triangleVertices[3] - triangleVertices[2] * triangleVertices[5];
	lineParams[6] = triangleVertices[1] - triangleVertices[5];
	lineParams[7] = triangleVertices[4] - triangleVertices[0];
	lineParams[8] = triangleVertices[0] * triangleVertices[5] - triangleVertices[4] * triangleVertices[1];

	// bound box
	int maxx = triangleVertices[0], minx = triangleVertices[0], maxy = triangleVertices[1], miny = triangleVertices[1];
	for (int i = 2; i < 6; i += 2) {
		if (maxx < triangleVertices[i]) maxx = triangleVertices[i];
		if (minx > triangleVertices[i]) minx = triangleVertices[i];
		if (maxy < triangleVertices[i+1]) maxy = triangleVertices[i+1];
		if (miny > triangleVertices[i+1]) miny = triangleVertices[i+1];
	}
	//std::cout << minx << " " << maxx << " " << miny << " " << maxy << std::endl;

	std::vector<float> points;
	for (int x = minx; x <= maxx; x++) {
		for (int y = miny; y < maxy; y++) {
			int p1 = lineParams[0] * x + lineParams[1] * y + lineParams[2];
			int p2 = lineParams[3] * x + lineParams[4] * y + lineParams[5];
			int p3 = lineParams[6] * x + lineParams[7] * y + lineParams[8];
			if ((p1 >= 0 && p2 >= 0 && p3 >= 0) || (p1 <= 0 && p2 <= 0 && p3 <= 0)) {
			//if (isInTriangle(x, y)) {
				points.push_back(x);
				points.push_back(y);
			}
		}
	}

	if (points.size() == 0)
		return;
	/*
	for (int i = 0; i < points.size(); i += 2) {
		points[i] = (float)(points[i] * 2) / (float)SCR_WIDTH;
		points[i + 1] = (float)(points[i + 1] * 2) / (float)SCR_HEIGHT;
	}
	*/
	int pointsNum = points.size() / 2;

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointsNum);
}

bool isInTriangle(int x, int y) {
	float x0 = triangleVertices[4] - triangleVertices[0], y0 = triangleVertices[5] - triangleVertices[1];
	float x1 = triangleVertices[2] - triangleVertices[0], y1 = triangleVertices[3] - triangleVertices[1];
	float x2 = x - triangleVertices[0], y2 = y - triangleVertices[1];

	float temp_00 = x0 * x0 + y0 * y0;
	float temp_01 = x0 * x1 + y0 * y1;
	float temp_02 = x0 * x2 + y0 * y2;
	float temp_11 = x1 * x1 + y1 * y1;
	float temp_12 = x1 * x2 + y1 * y2;

	float u = float(temp_11 * temp_02 - temp_01 * temp_12) / (float)(temp_00 * temp_11 - temp_01 * temp_01);
	float v = float(temp_00 * temp_12 - temp_01 * temp_02) / (float)(temp_00 * temp_11 - temp_01 * temp_01);
	if (u + v <= 1 && u >= 0 && v >= 0) return true;
	return false;
}

void drawCircle()
{
	if (circler == 0)
		return;

	std::vector<float> points;

	int x0 = 0;
	int y0 = circler;
	int d = (x0 + 1)*(x0 + 1) + (y0 - 0.5)*(y0 - 0.5) - circler * circler;
	int y = y0;
	points.push_back(x0);
	points.push_back(y0);
	for (int x = x0; x < circler * sqrt(2) / 2; x++) {
		if (d >= 0) {
			y--;
			d = d + 2 * (x - y) + 5;
		}
		else {
			d = d + 2 * x + 3;
		}
		points.push_back(x + 1);
		points.push_back(y);
	}

	int eightNum = points.size();
	for (int i = 0; i < eightNum; i += 2) {
		points.push_back(points[i + 1]);
		points.push_back(points[i]);
	}
	int fourNum = points.size();
	for (int i = 0; i < fourNum; i += 2) {
		points.push_back(points[i]);
		points.push_back(-points[i + 1]);
	}
	int halfNum = points.size();
	for (int i = 0; i < halfNum; i += 2) {
		points.push_back(-points[i]);
		points.push_back(points[i + 1]);
	}

	for (int i = 0; i < points.size(); i += 2) {
		points[i] += circlex;
		points[i + 1] += circley;
	}
	/*
	for (int i = 0; i < points.size(); i += 2) {
		points[i] = (float)(points[i] * 2) / (float)SCR_WIDTH;
		points[i + 1] = (float)(points[i + 1] * 2) / (float)SCR_HEIGHT;
	}
	*/
	int pointsNum = points.size() / 2;
	//std::cout << pointsNum << std::endl;

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointsNum);
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
	
	ImGui::SliderInt("Triangle x0", &triangleVertices[0], 0, SCR_WIDTH);
	ImGui::SliderInt("Triangle y0", &triangleVertices[1], 0, SCR_HEIGHT);
	ImGui::SliderInt("Triangle x1", &triangleVertices[2], 0, SCR_WIDTH);
	ImGui::SliderInt("Triangle y1", &triangleVertices[3], 0, SCR_HEIGHT);
	ImGui::SliderInt("Triangle x2", &triangleVertices[4], 0, SCR_WIDTH);
	ImGui::SliderInt("Triangle y2", &triangleVertices[5], 0, SCR_HEIGHT);
	ImGui::Checkbox("Fill Triangle", &fill_triangle);
	ImGui::SliderInt("Circle x", &circlex, 0, SCR_WIDTH);
	ImGui::SliderInt("Circle y", &circley, 0, SCR_HEIGHT);
	ImGui::SliderInt("Circle radius", &circler, 0, SCR_HEIGHT);
	ImGui::End();
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}