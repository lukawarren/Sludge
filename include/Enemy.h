#pragma once
#include "Common.h"

class Enemy
{
public:
    struct Attack
    {
        std::vector<std::string> verbs;
        std::string limbs;
    };

    Enemy(const std::vector<Attack>& attacks, const std::string& name, int damage, int health) :
        attacks(attacks), name(name), damage(damage), health(health), maxHealth(health) {};
    
    ~Enemy() {}

    std::vector<Attack> attacks;
    std::string name;
    int damage;
    int health;
    int maxHealth;

    enum Enemies
    {
        Gremlin
    };
};