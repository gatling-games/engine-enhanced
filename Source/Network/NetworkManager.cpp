#include "NetworkManager.h"

#include <imgui.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")


NetworkManager::NetworkManager(GLFWwindow* window)
{
    if (!initSockets())
    {

    }
    window_ = window;
    Socket sock;
    sock.open(30000);
}

NetworkManager::~NetworkManager()
{
    shutdownSockets();
}

void NetworkManager::drawDebugMenu()
{
    ImGui::Text("Hello Network");
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
