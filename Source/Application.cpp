#include "Application.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "Utils/Clock.h"
#include "EditorManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "RenderManager.h"

Application::Application(GLFWwindow* window)
{
    // Create core classes
    clock_ = new Clock();
	//Set time paused on startup for debug purposes
	clock_->setPaused(true);

    // Create engine modules
    inputManager_ = new InputManager(window);
    resourceManager_ = new ResourceManager("Resources/", "Build/Resources");
    editorManager_ = new EditorManager(window, true);
    sceneManager_ = new SceneManager();
    renderManager_ = new RenderManager();

    // Register engine module debug menus
    editorManager_->addModuleToDebugPanel(clock_);
    editorManager_->addModuleToDebugPanel(inputManager_);
    editorManager_->addModuleToDebugPanel(resourceManager_);
    editorManager_->addModuleToDebugPanel(sceneManager_);
}

Application::~Application()
{
    // Delete modules in opposite order to
    // how they were created.
    delete sceneManager_;
    delete editorManager_;
    delete inputManager_;
    delete clock_;
}

void Application::resize(int width, int height)
{
    // Each time a resize occurs we need to update the backbuffer size.
    Framebuffer::backbuffer()->setResolution(width, height);

    // Inform relevent modules of the resize
    editorManager_->resize(width, height);
}

void Application::windowFocused()
{
    // Resources may have been changed via external tools
    // so check for changed resources when focus switches
    // back to the application window.
    resourceManager_->importChangedResources();
}

void Application::frameStart()
{
    clock_->frameStart();
    inputManager_->frameStart(clock_);
    sceneManager_->frameStart();
    resourceManager_->update();
}

void Application::drawFrame()
{
    // Re-draw the editor window
    editorManager_->render();
}
