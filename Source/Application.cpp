#include "Application.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "Utils/Clock.h"
#include "EditorManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "RenderManager.h"

Application::Application(const std::string &name, GLFWwindow* window)
    : name_(name),
    window_(window),
    running_(true)
{
    // Create core classes
    clock_ = new Clock();
    //Set time paused on startup for debug purposes
    clock_->setPaused(true);

    // Create engine modules
    editorManager_ = new EditorManager(window, true);
    inputManager_ = new InputManager(window);
    resourceManager_ = new ResourceManager("Resources/", "Build/Resources");
    sceneManager_ = new SceneManager();
    renderManager_ = new RenderManager();

    // Create a Quit menu item
    MainWindowMenu::instance()->addMenuItem("File/Exit", [&] { running_ = false; });
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
    // Update each module manager
    clock_->frameStart();
    inputManager_->frameStart(clock_);
    sceneManager_->frameStart();
    resourceManager_->update();

    // Put the FPS in the window title.
    // Update every 20 frames (start at frame 1)
    if (clock_->frameCount() % 20 == 1)
    {
        const float deltaTime = clock_->realDeltaTime();
        const float frameRate = 1.0f / deltaTime;
        const float frameTime = deltaTime * 1000.0f;
        const std::string titleWithFPS = name_ + " [" + std::to_string(frameRate).substr(0, 4) + "FPS] [" + std::to_string(frameTime).substr(0, 5) + "ms]";
        glfwSetWindowTitle(window_, titleWithFPS.c_str());
    }
}

void Application::drawFrame()
{
    // Re-draw the editor window
    editorManager_->render();
}
