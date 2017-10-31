#include "Socket.h"
#include <winsock2.h>
#include <fstream>

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
    return true;
}

void Socket::close()
{
    closesocket(handle_);
}

bool Socket::send(const Address &destination, const void *data, int size)
{
    int sent_bytes = sendto(handle_, (const char*)data, sizeof(data), 0, (sockaddr*)destination.address(), sizeof(sockaddr_in));
    if (sent_bytes != sizeof(data))
    {
        printf("failed to send packet\n");
        return false;
    }
    return true;
}

bool Socket::recieve()
{
    return true;
}