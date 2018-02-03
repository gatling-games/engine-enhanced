#pragma once

#include "Editor/MainWindow.h"
#include "Utils/Singleton.h"

class EditorManager : public Singleton<EditorManager>
{
public:
    EditorManager(GLFWwindow* window, bool setupGLFSCallbacks);
    ~EditorManager();

    // Handles window resizing
    void resize(int width, int height);

    // Redraws the editor window(s).
    void render();

private:
    GLFWwindow* glfwWindow_;
    MainWindow mainWindow_;
};
