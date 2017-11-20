#pragma once

#include <string>

struct GLFWwindow;

class Clock;
class EditorManager;
class InputManager;
class ResourceManager;
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
    Application(GLFWwindow* window, int width, int height);
    ~Application();

    // Called when the window is resized.
    void resize(int newWidth, int newHeight);

    // Called during the game loop.
    void frameStart();
    void drawFrame();

private:
    Clock* clock_;
    EditorManager* editorManager_;
    InputManager* inputManager_;
    ResourceManager* resourceManager_;
    RenderManager* renderManager_;
};