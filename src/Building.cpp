#include "Building.h"
#include "Item.h"
#include "Game.h"

std::vector<std::string> Building::grandFurniture;
std::vector<std::string> Building::grandFloors;
std::vector<std::string> Building::grandWalls;

std::vector<std::string> Building::humbleFurniture;
std::vector<std::string> Building::humbleFloors;
std::vector<std::string> Building::humbleWalls;

std::vector<std::string> Building::weaponNames;
std::vector<std::string> Building::armourNames;
std::vector<std::string> Building::foodNames;
std::vector<std::string> Building::homeNames;

std::vector<std::string> Building::tavernAdjectives;
std::vector<std::string> Building::tavernNouns;

Building::Building(const unsigned int seed, const Portal parentPortal, const Type type, const bool grand) :
    Area(seed), grand(grand), parentPortal(parentPortal)
{
    // Add exit
    portals[GetStartingCell()] = parentPortal;

    // Load descriptions and names if need be
    if (grandFurniture.size() == 0)
    {
        grandFurniture = ReadLines("buildings/furniture_grand.txt");
        grandFloors = ReadLines("buildings/floors_grand.txt");
        grandWalls = ReadLines("buildings/walls_grand.txt");

        humbleFurniture = ReadLines("buildings/furniture_humble.txt");
        humbleFloors = ReadLines("buildings/floors_humble.txt");
        humbleWalls = ReadLines("buildings/walls_humble.txt");

        weaponNames = ReadLines("buildings/vendors/weapons.txt");
        armourNames = ReadLines("buildings/vendors/armour.txt");
        foodNames = ReadLines("buildings/vendors/food.txt");
        homeNames = ReadLines("buildings/vendors/homes.txt");

        tavernAdjectives = ReadLines("buildings/vendors/taverns-adjectives.txt");
        tavernNouns = ReadLines("buildings/vendors/taverns-nouns.txt");
    }

    // Make vendor
    switch (type)
    {
        case Tavern:
            vendors[GetStartingCell()] = Vendor(Vendor::Type::Drinks);
        break;

        case Food:
            vendors[GetStartingCell()] = Vendor(Vendor::Type::Food);
        break;

        case Weapons:
            vendors[GetStartingCell()] = Vendor(Vendor::Type::Weapons);
        break;

        case Armour:
            vendors[GetStartingCell()] = Vendor(Vendor::Type::Armour);
        break;

        default:
        break;
    };

    // Chose name
    switch (type)
    {
        case Tavern:
            name = "The " + tavernAdjectives[rand() % tavernAdjectives.size()] + " " + tavernNouns[rand() % tavernNouns.size()];
        break;

        case Food:
            name = vendors[GetStartingCell()].name + "'s " + foodNames[rand() % foodNames.size()];
        break;

        case Weapons:
            name = vendors[GetStartingCell()].name + "'s " + weaponNames[rand() % weaponNames.size()];
        break;

        case Armour:
            name = vendors[GetStartingCell()].name + "'s " + armourNames[rand() % armourNames.size()];
        break;

        case Home:
            name = Vendor().name + "'s " + homeNames[rand() % homeNames.size()];
        break;

        default:
            name = Vendor().name + "'s property";
        break;
    }
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