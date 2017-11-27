#pragma once

#include "Address.h"

class Socket
{
public:

    Socket();
    ~Socket();

    bool open(unsigned short port);

    void close();

    bool isOpen() const;

    bool send(const Address &destination, const void *data, int size);

    bool recieve();

    unsigned short port();

private:

    unsigned short port_;
    int handle_;

};