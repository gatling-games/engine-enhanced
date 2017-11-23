#pragma once
#include <GLFW/glfw3.h>

#include "Network/Socket.h"
#include "Application.h"
#include "Utils/Singleton.h"

class NetworkManager : public ApplicationModule, public Singleton<NetworkManager>
{
public:
    NetworkManager();
    ~NetworkManager();

    std::string name() const override { return "Network Manager"; }

    void drawDebugMenu() override;
    //Called every frame
    void frameStart();

    Socket getSocket() const { return socket_; }

private:
    bool initSockets();
    void shutdownSockets();
    Socket getSocket();

    Socket socket_;
};