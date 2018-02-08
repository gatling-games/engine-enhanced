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

    // Displays a file save dialog. Blocks until the dialog is closed.
    // Returns the path string if the user saved the file, and an empty
    // string if the user cancels.
    std::string showSaveDialog(const std::string &title, const std::string &fileName, 
        const std::string &fileExtension) const;

private:
    GLFWwindow* glfwWindow_;
    MainWindow mainWindow_;
};
