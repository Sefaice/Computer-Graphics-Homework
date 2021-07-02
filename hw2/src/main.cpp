// glad
#include <glad/glad.h>
// glfw3
#include <GLFW/glfw3.h>

// IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>

#include "../../common/shader.h"

// ��Ļ�ߴ����
const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const float points[] = {
	// triangle1
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 
	0.3f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
	// triangle2
	0.1f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.6f, 0.3f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.4f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
};

const int pointIndices[] = {
	0, 1, 2,
	3, 4, 5
};

int main()
{
	// ��ʼ������glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// �󶨴��ڴ�С�ı�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��glad���غ���ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ����Shader�������ɫ������
	Shader myShader("../../hw2/src/vertexShader.vs", "../../hw2/src/fragmentShader.fs");

	// ������
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind VAO
	glBindVertexArray(VAO);
	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	// bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices), pointIndices, GL_STATIC_DRAW);
	// ���Ӷ�������1-λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ���Ӷ�������2-��ɫ
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	// unbind VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// ���ǲ���unbind VEO buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// unbind VAO
	glBindVertexArray(0);

	// ��ʼ��IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool my_tool_active = true;
	glm::vec4 my_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// �Ƿ�����߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// �û�����
		processInput(window);

		// �����ɫ����
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw
		myShader.use();
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		myShader.setVec4("color", my_color);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
		ImGui::End();
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
		// �������¼�����������
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// ����
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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