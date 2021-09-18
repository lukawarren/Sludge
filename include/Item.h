#pragma once
#include "Common.h"

class Item
{
public:
    Item(const std::string& name, const std::string& description, const std::function<void(Player&)> onUse) :
        name(name), description(description), onUse(onUse) {}

    Item() {}
    ~Item() {}

    enum Items
    {
        caveMushroom,
        NUM_ITEMS
    };

    std::string name;
    std::string description;

    void Use(Player& player) const
    {
        onUse(player);
    }

private:
    std::function<void(Player&)> onUse;

};

struct ItemStack
{
    ItemID item;
    unsigned int number;

    ItemStack(ItemID item, unsigned int number) : item(item), number(number) {}
};