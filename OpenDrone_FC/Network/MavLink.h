#pragma once
#include <cstdint>
class MavLink
{
public:
    void start();
private:
    uint32_t lastRecieved = 0;
};

