#pragma once
#include "Common.h"

class Player
{
public:
    Player(const std::string& name, unsigned int area, const Cell cell) : name(name), level(0), area(area), health(100), cell(cell) {};
    ~Player() {}

    std::string name;
    unsigned int level;
    unsigned int area;
    int health;
    Cell cell;

    std::unordered_map<ItemID, unsigned int> items;
    std::optional<ItemID> weapon;
    
    std::string outputBuffer;

    template<typename T>
    Player& operator<< (const T& i)
    {
        outputBuffer += std::to_string(i);
        return *this;
    }

    Player& operator<< (const char* s)
    {
        outputBuffer += std::string(s);
        return *this;
    }

    Player& operator<< (const std::string& s)
    {
        outputBuffer += s;
        return *this;
    }
};