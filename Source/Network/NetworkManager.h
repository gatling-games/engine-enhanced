#pragma once
#include <GLFW/glfw3.h>

#include "Socket.h"
#include "Application.h"

class NetworkManager : public ApplicationModule
{
public:
    explicit NetworkManager(GLFWwindow* window);
    ~NetworkManager();

private:
    void drawDebugMenu() override;
    bool initSockets();
    void shutdownSockets();

    GLFWwindow* window_;
};