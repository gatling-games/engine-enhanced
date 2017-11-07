#pragma once

#include "Application.h"
#include "Editor/EditorMainWindow.h"

class EditorManager : public ApplicationModule
{
public:
    EditorManager(GLFWwindow* window, bool setupGLFSCallbacks, ResourceManager* resourceManager);
    ~EditorManager();

    // ApplicationModule overrides
    std::string name() const { return "Editor Manager"; }

    // Adds a module to the debugging panel
    // This allows modules to display stats, toggles, buttons etc
    void addModuleToDebugPanel(ApplicationModule* module);

    // Handles window resizing
    void resize(int width, int height);

    // Redraws the editor window(s).
    void render();

private:
    GLFWwindow* glfwWindow_;
    EditorMainWindow mainWindow_;
};
