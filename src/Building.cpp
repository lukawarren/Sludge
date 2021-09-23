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
    Area(seed), parentPortal(parentPortal), grand(grand)
{
    // Add exit
    portals[GetStartingCell()] = parentPortal;

    // Load descriptions if need be
    if (grandFurniture.size() > 0) return;

    // TODO: remove Game::Get()
    grandFurniture = Game::Get().ReadLines("buildings/furniture_grand.txt");
    grandFloors = Game::Get().ReadLines("buildings/floors_grand.txt");
    grandWalls = Game::Get().ReadLines("buildings/walls_grand.txt");

    humbleFurniture = Game::Get().ReadLines("buildings/furniture_humble.txt");
    humbleFloors = Game::Get().ReadLines("buildings/floors_humble.txt");
    humbleWalls = Game::Get().ReadLines("buildings/walls_humble.txt");
}

void Building::LoadAreas() {}
Cell Building::GetStartingCell() const { return 0; }

void Building::Look(Player& player) const
{
    player << "You are in a building.\n\n";
    srand(player.cell);

    // Descriptions
    const std::string& furniture = grand ? grandFurniture[rand() % grandFurniture.size()] : humbleFurniture[rand() % humbleFurniture.size()];
    const std::string& floors = grand ? grandFloors[rand() % grandFloors.size()] : humbleFloors[rand() % humbleFloors.size()];
    const std::string& walls = grand ? grandWalls[rand() % grandWalls.size()] : humbleWalls[rand() % humbleWalls.size()];

    player << walls << " " << furniture << " " << floors << "\n";
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