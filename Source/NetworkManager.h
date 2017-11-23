#pragma once
#include <GLFW/glfw3.h>

#include "Network/Socket.h"
#include "Application.h"

class NetworkManager : public ApplicationModule
{
public:
    NetworkManager();
    ~NetworkManager();

    std::string name() const override { return "Network Manager"; }

    void drawDebugMenu() override;

private:
    bool initSockets();
    void shutdownSockets();
    Socket getSocket();

    Socket socket_;
};