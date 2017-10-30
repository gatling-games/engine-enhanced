#include "Address.h"

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

unsigned int Address::address() const
{
    return address_;
}

unsigned short Address::port() const
{
    return port_;
}