#pragma once

struct GLFWwindow;

class DebugGUI
{
public:
	DebugGUI(GLFWwindow* window, bool setupGLFWCallbacks);
	~DebugGUI();
	
	void frameStart();
	void render();

private:
	GLFWwindow* window_;
};
