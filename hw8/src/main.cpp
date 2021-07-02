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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);
void drawCtrlPoints(float* points, int num, float t, Shader& shader);
void drawLine(int x0, int y0, int x1, int y1);
void drawLineInt(int x0, int y0, int x1, int y1);
void drawPoint(float x0, float y0);
void drawToolBar();

bool capture_input = true;
std::vector<float> inputPoints;
std::vector<float> bezierPoints;


// imgui
bool my_tool_active = true;
glm::vec4 my_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	// 初始化配置glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW8", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// 绑定窗口大小改变的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// mouse click callback
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// 用glad加载函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glEnable(GL_MULTISAMPLE);

	// enable change point size
	glEnable(GL_PROGRAM_POINT_SIZE);

	// 调用Shader类加载着色器代码
	Shader myShader("../../hw8/src/vertexShader.vs.glsl", "../../hw8/src/fragmentShader.fs.glsl");

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

	float t = 0.0;

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// show FPS
		int fps = 1 / deltaTime;
		glfwSetWindowTitle(window, ("FPS: " + std::to_string(fps)).c_str());
		
		// 用户输入
		processInput(window);

		// 清空颜色缓冲
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw
		myShader.use();
		myShader.setVec4("color", glm::vec4(1.0));
		glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
		//std::cout << glm::to_string(projection) << std::endl;
		myShader.setMat4("projection", projection);

		if (capture_input)
		{
			// draw input points
			myShader.setInt("pointSize", 8);
			for (int i = 0; i < inputPoints.size(); i+=2)
			{
				drawPoint(inputPoints[i], inputPoints[i + 1]);
			}
		}
		else
		{
			// recrusive draw ctrl points and middle points and line them up
			float* ctrlPoints = new float[inputPoints.size()];
			for (int i = 0; i < inputPoints.size(); i++)
			{
				ctrlPoints[i] = inputPoints[i];
			}
			drawCtrlPoints(ctrlPoints, inputPoints.size(), t, myShader);

			// draw bezier curve
			myShader.setInt("pointSize", 1);
			myShader.setVec4("color", glm::vec4(1.0, 0.0, 0.0, 1.0));
			for (int i = 0; i < bezierPoints.size(); i += 2)
			{
				drawPoint(bezierPoints[i], bezierPoints[i + 1]);
			}
			t += 0.002;
			if (t > 1.0)
			{
				t = 0.0;
				bezierPoints.clear();
				std::vector<float>().swap(bezierPoints);
			}

			//std::cout << t << std::endl;
			//delete ctrlPoints;
		}

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && capture_input)
	{

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		inputPoints.push_back(xpos);
		inputPoints.push_back(ypos);
	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void drawCtrlPoints(float* points, int num, float t, Shader& shader)
{
	if (num == 2)
	{
		shader.setInt("pointSize", 8);
		drawPoint(points[0], points[1]);
	
		bezierPoints.push_back(points[0]);
		bezierPoints.push_back(points[1]);
		return;
	}

	float* tmpPoints = new float[num - 2];
	for (int i = 0; i < num - 2; i += 2)
	{
		shader.setInt("pointSize", 8);
		drawPoint(points[i], points[i + 1]);
		drawPoint(points[i + 2], points[i + 3]);

		shader.setInt("pointSize", 1);
		drawLine(points[i], points[i + 1], points[i + 2], points[i + 3]);

		tmpPoints[i] = (1 - t) * points[i] + t * points[i + 2];
		tmpPoints[i + 1] = (1 - t) * points[i + 1] + t * points[i + 3];
	}

	memcpy(points, tmpPoints, sizeof(float) * (num-2));
	delete tmpPoints;

	drawCtrlPoints(points, num - 2, t, shader);
}

void drawPoint(float x0, float y0)
{
	float points[] = { x0, y0 };

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), points, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);

	// delete otherwise to much unfreed VBO&VAO will cause program stuck
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void drawLine(int x0, int y0, int x1, int y1)
{
	float points[] = { x0, y0, x1, y1 };

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), points, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
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

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
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
	if (ImGui::Button("DRAW BEZIER CURVE!"))
	{
		if (capture_input)
		{
			capture_input = false;
			// pop the last poimnt
			inputPoints.pop_back();
			inputPoints.pop_back();
		}
	}
		
	ImGui::End();
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}