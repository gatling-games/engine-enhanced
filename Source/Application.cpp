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
    mode_(ApplicationMode::Edit),
    window_(window),
    running_(true)
{
    // Create engine modules
    editorManager_ = new EditorManager(window, true);
    inputManager_ = new InputManager(window);
    resourceManager_ = new ResourceManager("Resources/", "Build/CompiledResources");
    sceneManager_ = new SceneManager();
    renderManager_ = new RenderManager();

    // Create core classes
    clock_ = new Clock();
    clock_->setPaused(true);

    // Create a Quit menu item
    MainWindowMenu::instance()->addMenuItem("File/Exit", [&] { running_ = false; });
}

Application::~Application()
{
    // Save all source files when the application exits
    resourceManager_->saveAllSourceFiles();

    // Delete modules in opposite order to
    // how they were created.
    delete sceneManager_;
    delete editorManager_;
    delete inputManager_;
    delete clock_;
}

void Application::enterPlayMode()
{
    // Do nothing if already in play mode.
    if(isPlaying())
    {
        return;
    }

    // First, save the current scene
    // This allows the original scene to be restored when exiting play mode.
    SceneManager::instance()->saveScene();

    // Now, enter play mode and start the clock
    mode_ = ApplicationMode::Play;
    Clock::instance()->restart();
}

void Application::enterEditMode()
{
    // Do nothing if already in edit mode
    if(isEditing())
    {
        return;
    }

    // Enter edit mode and stop the clock
    mode_ = ApplicationMode::Edit;
    Clock::instance()->stop();

    // When we entered play mode, we saved the scene.
    // We now need to restore the original scene to undo changes made while playing.
    SceneManager::instance()->openScene(SceneManager::instance()->scenePath());
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
        std::string titleWithFPS = name_ + " - " + SceneManager::instance()->sceneName() + " [" + std::to_string(frameRate).substr(0, 4) + "FPS] [" + std::to_string(frameTime).substr(0, 5) + "ms]";
        if (isEditing()) titleWithFPS += " [Editing]";
        if (isPlaying()) titleWithFPS += " [Playing]";
        glfwSetWindowTitle(window_, titleWithFPS.c_str());
    }
}

void Application::drawFrame()
{
    // Re-draw the editor window
    editorManager_->render();
}
