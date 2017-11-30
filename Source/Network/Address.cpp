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
    unsigned int n = a.address();
    os << ((n >> 24) & 0xff) << "." << ((n >> 16) & 0xff) << "." << ((n >> 8) & 0xff) << "." << (n & 0xff);
    return os;
}