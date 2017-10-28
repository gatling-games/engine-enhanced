#pragma once

struct GLFWwindow;

#include <vector>

#include "Application.h"

class DebugGUI
{
public:
	DebugGUI(GLFWwindow* window, bool setupGLFWCallbacks);
	~DebugGUI();
	
	bool visible() const;
	void show();
	void hide();

    void addToDebugMenu(ApplicationModule* module, const char* name);

	void render();

private:
	GLFWwindow* window_;
	bool visible_;
    std::vector<ApplicationModule*> modules_;
    std::vector<const char*> moduleNames_;
};
