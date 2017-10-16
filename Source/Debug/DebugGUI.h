#pragma once

struct GLFWwindow;

class DebugGUI
{
public:
	DebugGUI(GLFWwindow* window, bool setupGLFWCallbacks);
	~DebugGUI();
	
	bool visible() const;
	void show();
	void hide();

	void frameStart();
	void render();

private:
	GLFWwindow* window_;
	bool visible_;
};
