#pragma once

struct GLFWwindow;
class Clock;
class DebugGUI;
class InputManager;
class NetworkManager;

class ApplicationModule
{
public:
    virtual void drawDebugMenu() { }
};

class Application
{
public:
    Application(GLFWwindow* window);
    ~Application();

    void frameStart();
    void drawFrame();

private:
    Clock* clock_;
    DebugGUI* debugGUI_;
    InputManager* inputManager_;
    NetworkManager* networkManager_;
};