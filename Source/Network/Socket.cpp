#include "Socket.h"
#include <WinSock2.h>
#include <fstream>
#include <WS2tcpip.h>

Socket::Socket()
{
 
}

Socket::~Socket()
{
    close();
}

bool Socket::open(unsigned short port)
{
    
    handle_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (handle_ <= 0)
    {
        printf("failed to create socket\n");
        return false;
    }

    sockaddr_in address;
    socklen_t len = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(handle_, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
    {
        printf("failed to bind socket\n");
        return false;
    }
    DWORD nonBlocking = 1;
    if (ioctlsocket(handle_, FIONBIO, &nonBlocking) != 0)
    {
        printf("Failed to set non blocking\n");
        return false;
    }
    getsockname(handle_, (sockaddr*)&address, &len);
    port_ = ntohs(address.sin_port);
    return true;
}

void Socket::close()
{
    closesocket(handle_);
}

bool Socket::send(const Address &destination, const void *data, int size)
{
    sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = htonl(destination.address());
    to.sin_port = htons(destination.port());
    int sent_bytes = sendto(handle_, (const char*)data, sizeof(data), 0, (sockaddr*)&to, sizeof(sockaddr_in));
    if (sent_bytes != sizeof(data))
    {
        printf("failed to send packet\n");
        return false;
    }
    return true;
}

int Socket::recieve()
{
    
    while (true)
    {
        unsigned char packetData[256];

        unsigned int maxPacketSize = sizeof(packetData);

        sockaddr_in from;
        socklen_t fromLenth = sizeof(from);

        int bytes = recvfrom(handle_, (char*)packetData, maxPacketSize, 0, (sockaddr*)&from, &fromLenth);
        if (bytes <= 0)
        {
            break;
        }
        unsigned int from_address = ntohl(from.sin_addr.s_addr);
        unsigned int from_port = ntohs(from.sin_port);
        printf("Bytes recieved: %d\n Sender:%d %d\n Data:%s\n", bytes, from_address, from_port, packetData);
    }
    return true;
}

unsigned short Socket::port()
{
    return port_;
}