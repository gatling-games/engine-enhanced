#include "Address.h"
#include <winsock.h>

Address::Address()
{

}

Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
{
    address_ = (a << 24) | (b << 16) | (c << 8) | d;
    port_ = port;
}

Address::Address(unsigned int address, unsigned short port)
{
    address_ = address;
    port_ = port;
}

std::ostream& operator<<(std::ostream& os, const Address& a)
{
    sockaddr_in d;
    d.sin_addr.s_addr = htonl(a.address());
    os << inet_ntoa(d.sin_addr);
    return os;
}