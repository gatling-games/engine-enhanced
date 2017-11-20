#include "Application.h"

#include <GLFW\glfw3.h>

#include "Utils/Clock.h"
#include "EditorManager.h"
#include "InputManager.h"
#include "ResourceManager.h"

Application::Application(GLFWwindow* window, int width, int height)
{
    // Create core classes
    clock_ = new Clock();

    // Create engine modules
    inputManager_ = new InputManager(window);
    resourceManager_ = new ResourceManager("Resources/", "Build/Resources");
    editorManager_ = new EditorManager(window, true);

    // Register engine module debug menus
    editorManager_->addModuleToDebugPanel(clock_);
    editorManager_->addModuleToDebugPanel(inputManager_);
    editorManager_->addModuleToDebugPanel(resourceManager_);
}

Application::~Application()
{
    // Delete modules in opposite order to
    // how they were created.
    delete inputManager_;
    delete editorManager_;
    delete clock_;
}

void Application::resize(int width, int height)
{
    editorManager_->resize(width, height);
}

void Application::frameStart()
{
    clock_->frameStart();
    inputManager_->frameStart(clock_);
}

void Application::drawFrame()
{
    // Render the frame.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Re-draw the editor window
    editorManager_->render();
}
