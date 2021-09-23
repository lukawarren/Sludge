#pragma once
#include "Common.h"

class Item
{
public:
    Item(const std::string& name, const std::string& description, const int price, const int attack = 1) :
        name(name), description(description), price(price), attack(attack) {}

    Item() {}
    ~Item() {}

    enum Items
    {
        CaveMushroom,
        NUM_ITEMS
    };

    std::string name;
    std::string description;
    int price;
    int attack;
};

struct ItemStack
{
    ItemID item;
    unsigned int number;

    ItemStack(ItemID item, unsigned int number) : item(item), number(number) {}
};