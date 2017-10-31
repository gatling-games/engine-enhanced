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

private:

    int handle_;

};