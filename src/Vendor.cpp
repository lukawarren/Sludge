#include "Vendor.h"
#include "Game.h"

std::vector<std::string> Vendor::names;

Vendor::Vendor()
{
    if (names.size() == 0) names = ReadLines("names.txt");
    name = names[rand() % names.size()];

    for (const auto& weaponGroup : Game::Get().weapons)
        items.emplace_back(weaponGroup[rand() % weaponGroup.size()], rand() % 3 + 1);
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