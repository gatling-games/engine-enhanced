#include "NetworkManager.h"

#include <imgui.h>
#include <WinSock2.h>


NetworkManager::NetworkManager()
{
    if (!initSockets())
    {

    }
    socket_ = Socket();
    socket_.open(0);
}

NetworkManager::~NetworkManager()
{
    shutdownSockets();
}

void NetworkManager::drawDebugMenu()
{
    ImGui::Text("Listening on port %d", socket_.port());
}

bool NetworkManager::initSockets()
{
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
}

void NetworkManager::shutdownSockets()
{
    WSACleanup();
}
