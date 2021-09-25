#pragma once
#include "Common.h"
#include "Item.h"

class Vendor
{
public:

    enum Type
    {
        Weapons,
        Armour,
        Food,
        Drinks
    };

    Vendor();
    Vendor(const Type type);
    ~Vendor();

    void AddItem(ItemID itemID);
    void RemoveItem(size_t vectorIndex);

    static std::vector<std::string> names;
    std::vector<ItemStack> items;
    std::string name;
};