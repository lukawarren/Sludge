#include "Vendor.h"
#include "Game.h"

std::vector<std::string> Vendor::names;

Vendor::Vendor()
{
    // Default constructor, generate names anyway for
    // random name genearation in Building.cpp
    if (names.size() == 0) names = ReadLines("names.txt");
    name = names[rand() % names.size()];
}

Vendor::Vendor(const Type type)
{
    if (names.size() == 0) names = ReadLines("names.txt");
    name = names[rand() % names.size()];

    switch (type)
    {

        case Weapons:
            for (const auto& weaponGroup : Game::Get().weapons)
                if (rand() % 100 <= 40)
                    items.emplace_back(weaponGroup[rand() % weaponGroup.size()], rand() % 2 + 1);
        break;

        case Armour:
            for (const auto& armourGroup : Game::Get().armours)
                if (rand() % 100 <= 60)
                    items.emplace_back(armourGroup[rand() % armourGroup.size()], rand() % 2 + 1);
        break;

        case Food:
            for (const auto& food : Game::Get().foods)
                items.emplace_back(food, rand() % 5 + 4);
        break;
        
        case Drinks:
            for (const auto& drink : Game::Get().drinks)
                items.emplace_back(drink, rand() % 5 + 4);
        break;

    }
}

void Vendor::AddItem(ItemID itemID)
{
    for (auto& stack : items)
    {
        if (stack.item == itemID)
        {
            stack.number++;
            return;
        }
    }

    items.emplace_back(itemID, 1);
}

void Vendor::RemoveItem(size_t vectorIndex)
{
    // Remove 1 from stack, or remove item entirely
    if (items[vectorIndex].number > 1) items[vectorIndex].number--;
    else items.erase(items.begin() + vectorIndex);
}

Vendor::~Vendor() {}