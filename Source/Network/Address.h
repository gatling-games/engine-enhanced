#pragma once

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

    unsigned int address() const;
    unsigned short port() const;

private:

    unsigned int address_;
    unsigned short port_;
};