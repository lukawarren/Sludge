#pragma once
#include "Common.h"

#define MAX_PLAYER_HEALTH 100

class Player
{
public:
    Player(const std::string& name, unsigned int area, const Cell cell) : name(name), level(0), area(area), health(MAX_PLAYER_HEALTH), money(1000), cell(cell) {};
    ~Player() {}

    std::string name;
    unsigned int level;
    unsigned int area;
    int health;
    int money;
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