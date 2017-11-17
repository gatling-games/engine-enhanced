#include "Application.h"

#include <GL/gl3w.h>
#include <GLFW\glfw3.h>

#include "Utils/Clock.h"
#include "EditorManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "RenderManager.h"

Application::Application(GLFWwindow* window, int width, int height)
{
    // Create core classes
    clock_ = new Clock();

    // Create engine modules
    inputManager_ = new InputManager(window);
    resourceManager_ = new ResourceManager("Resources/", "Build/Resources");
    editorManager_ = new EditorManager(window, true, resourceManager_);
    renderManager_ = new RenderManager();

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
    // Re-draw the editor window
    editorManager_->render();
}
