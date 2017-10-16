#include "DebugGUI.h"

#include <imgui.h>
#include <imgui\examples\opengl3_example\imgui_impl_glfw_gl3.h>

#include <GLFW\glfw3.h>

DebugGUI::DebugGUI(GLFWwindow* window, bool initialize)
{
	window_ = window;

	ImGui_ImplGlfwGL3_Init(window, true);


	ImGui::GetIO().FontGlobalScale = 2.0f;
}

DebugGUI::~DebugGUI()
{
	ImGui_ImplGlfwGL3_Shutdown();
}

void DebugGUI::frameStart()
{
	ImGui_ImplGlfwGL3_NewFrame();
	ImGui::ShowTestWindow();
}

void DebugGUI::render()
{
	int display_w, display_h;
	glfwGetFramebufferSize(window_, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();
}