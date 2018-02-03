#pragma once

#include <string>

struct GLFWwindow;

class Clock;
class EditorManager;
class InputManager;
class ResourceManager;
class SceneManager;
class RenderManager;

class Application
{
public:
    Application(const std::string &name, GLFWwindow* window);
    ~Application();

    bool running() const { return running_; }

    // Called when the window is resized.
    void resize(int newWidth, int newHeight);

    // Called when the window (re)gains focus.
    void windowFocused();

    // Called during the game loop.
    void frameStart();
    void drawFrame();

private:
    const std::string name_;

    // The main window
    GLFWwindow* window_;

    // The clock manager
    Clock* clock_;

    // Module managers
    EditorManager* editorManager_;
    InputManager* inputManager_;
    ResourceManager* resourceManager_;
    SceneManager* sceneManager_;
    RenderManager* renderManager_;

    // True until quitting
    bool running_;
};