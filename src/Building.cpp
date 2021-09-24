#include "Building.h"
#include "Item.h"
#include "Game.h"

std::vector<std::string> Building::grandFurniture;
std::vector<std::string> Building::grandFloors;
std::vector<std::string> Building::grandWalls;

std::vector<std::string> Building::humbleFurniture;
std::vector<std::string> Building::humbleFloors;
std::vector<std::string> Building::humbleWalls;

Building::Building(const unsigned int seed, const Portal parentPortal, const bool grand) :
    Area(seed), grand(grand), parentPortal(parentPortal)
{
    // Add exit
    portals[GetStartingCell()] = parentPortal;

    // Test vendor
    vendors[GetStartingCell()] = Vendor();
    name = vendors[GetStartingCell()].name + "'s general goods";

    // Load descriptions if need be
    if (grandFurniture.size() > 0) return;
    
    grandFurniture = ReadLines("buildings/furniture_grand.txt");
    grandFloors = ReadLines("buildings/floors_grand.txt");
    grandWalls = ReadLines("buildings/walls_grand.txt");

    humbleFurniture = ReadLines("buildings/furniture_humble.txt");
    humbleFloors = ReadLines("buildings/floors_humble.txt");
    humbleWalls = ReadLines("buildings/walls_humble.txt");
}

void Building::LoadAreas() {}
Cell Building::GetStartingCell() const { return 0; }

void Building::Look(Player& player) const
{
    player << "You are in a building.\n";
    srand(seed * 10000 + player.cell);

    // Descriptions
    const std::string& furniture = grand ? grandFurniture[rand() % grandFurniture.size()] : humbleFurniture[rand() % humbleFurniture.size()];
    const std::string& floors = grand ? grandFloors[rand() % grandFloors.size()] : humbleFloors[rand() % humbleFloors.size()];
    const std::string& walls = grand ? grandWalls[rand() % grandWalls.size()] : humbleWalls[rand() % humbleWalls.size()];

    player << "- " << walls << "\n";
    player << "- " << furniture << "\n";
    player << "- " << floors << "\n";

    // Vendors
    if (vendors.count(player.cell))
        player << "- The building's proprietor, " << vendors.at(player.cell).name << ", invites you to trade...\n";
}

void Building::Move(Player& player, const Direction direction, const int distance) const
{
    player << "There is nowhere else to move to\n";
}

std::vector<ItemStack>& Building::GetItems(const Cell cell)
{
    return items;
}

std::string Building::GetPortalText() const
{
    return "You stand by a doorway to a building unknown...";
}

Building::~Building() {}