#pragma once
#include <string>
class Address
{
public:

    Address();

    Address(unsigned char a,
        unsigned char b,
        unsigned char c,
        unsigned char d,
        unsigned short port);

    Address(unsigned int address,
        unsigned short port);

    unsigned int address() const { return address_; }
    unsigned short port() const { return port_; }

private:

    unsigned int address_;
    unsigned short port_;
};