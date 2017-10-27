#include "DebugGUI.h"

#include <imgui.h>
#include <imgui\examples\opengl3_example\imgui_impl_glfw_gl3.h>

#include <GLFW\glfw3.h>

DebugGUI::DebugGUI(GLFWwindow* window, bool setupGLFWCallbacks)
{
	window_ = window;
	visible_ = true;

	ImGui_ImplGlfwGL3_Init(window, setupGLFWCallbacks);

	// Adjust the ui scale based on monitor DPI
	// This allows the ui to work properly with hidpi monitors.
	// Adapted from http://www.glfw.org/docs/latest/monitor_guide.html
	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	if (monitorCount > 0)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(monitors[0]);

		int widthMM, heightMM;
		glfwGetMonitorPhysicalSize(monitors[0], &widthMM, &heightMM);
		const float dpi = mode->width / (widthMM / 25.4f);

		// Set to 1.0 scaling at 110DPI (dont go below 1.0)
		// Aka 2.0 scaling on a 15" macbook pro retina
		ImGui::GetIO().FontGlobalScale = (dpi < 110.0f ? 1.0f : dpi / 110.0f);
	}
}

DebugGUI::~DebugGUI()
{
	ImGui_ImplGlfwGL3_Shutdown();
}

bool DebugGUI::visible() const
{
	return visible_;
}

void DebugGUI::show()
{
	visible_ = true;
}

void DebugGUI::hide()
{
	visible_ = false;
}

void DebugGUI::frameStart()
{
	if (!visible_)
	{
		return;
	}

	ImGui_ImplGlfwGL3_NewFrame();
	ImGui::ShowTestWindow();
}

void DebugGUI::render()
{
	if (!visible_)
	{
		return;
	}

	int display_w, display_h;
	glfwGetFramebufferSize(window_, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();
}