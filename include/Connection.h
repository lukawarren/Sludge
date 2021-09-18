#pragma once
#include "Common.h"

class Connection
{
public:
    Connection(int clientFd, const std::string address);
    ~Connection();
};