#pragma once

#include <string>

struct GLFWwindow;

class Clock;
class EditorManager;
class InputManager;
class ResourceManager;
class SceneManager;
class RenderManager;

class ApplicationModule
{
public:
    virtual std::string name() const = 0;
    virtual void drawDebugMenu() { }
};

class Application
{
public:
    Application(GLFWwindow* window);
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
    Clock* clock_;
    EditorManager* editorManager_;
    InputManager* inputManager_;
    ResourceManager* resourceManager_;
    SceneManager* sceneManager_;
    RenderManager* renderManager_;
    bool running_;
};