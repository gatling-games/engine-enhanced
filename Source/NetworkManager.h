#pragma once
#include <GLFW/glfw3.h>

#include "Socket.h"
#include "Application.h"

class NetworkManager : public ApplicationModule
{
public:
    explicit NetworkManager(GLFWwindow* window);
    ~NetworkManager();

    void drawDebugMenu() override;

private:
    bool initSockets();
    void shutdownSockets();
    Socket getSocket();

    GLFWwindow* window_;
    Socket socket_;
};