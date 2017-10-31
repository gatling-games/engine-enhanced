#include "Application.h"

#include "Utils/Clock.h"
#include "Debug/DebugGUI.h"
#include "InputManager.h"
#include "Network\NetworkManager.h"

Application::Application(GLFWwindow* window)
{
    // Create core classes
    clock_ = new Clock();
    debugGUI_ = new DebugGUI(window, true);

    // Create engine modules
    inputManager_ = new InputManager(window);
    networkManager_ = new NetworkManager(window);

    // Register engine module debug menus
    debugGUI_->addToDebugMenu(clock_, "Clock Manager");
    debugGUI_->addToDebugMenu(inputManager_, "Input Manager");
    debugGUI_->addToDebugMenu(networkManager_, "Network Manager");
}

Application::~Application()
{
    // Delete modules in opposite order to
    // how they were created.
    delete networkManager_;
    delete inputManager_;
    delete debugGUI_;
    delete clock_;
}

void Application::frameStart()
{
    clock_->frameStart();
    inputManager_->frameStart(clock_);
}

void Application::drawFrame()
{
    // Render the frame.
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw debug menus on top of the frame.
    debugGUI_->render();
}
