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

std::vector<std::string> Building::weaponFurniture;
std::vector<std::string> Building::armourFurniture;
std::vector<std::string> Building::foodFurniture;
std::vector<std::string> Building::homeFurniture;
std::vector<std::string> Building::tavernFurniture;

Building::Building(const unsigned int seed, const Portal parentPortal, const Type type, const bool grand) :
    Area(seed), grand(grand), type(type), parentPortal(parentPortal)
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

        weaponNames = ReadLines("buildings/vendors/weapons_names.txt");
        armourNames = ReadLines("buildings/vendors/armour_names.txt");
        foodNames = ReadLines("buildings/vendors/food_names.txt");
        homeNames = ReadLines("buildings/vendors/homes_names.txt");

        tavernAdjectives = ReadLines("buildings/vendors/taverns_adjectives.txt");
        tavernNouns = ReadLines("buildings/vendors/taverns_nouns.txt");

        weaponFurniture = ReadLines("buildings/vendors/weapons_furniture.txt");
        armourFurniture = ReadLines("buildings/vendors/armour_furniture.txt");
        foodFurniture = ReadLines("buildings/vendors/food_furniture.txt");
        homeFurniture = ReadLines("buildings/vendors/homes_furniture.txt");
        tavernFurniture = ReadLines("buildings/vendors/taverns_furniture.txt");
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

    // Place-specific descriptions
    switch (type)
    {
        case Tavern:
            player << "- " << tavernFurniture[rand() % tavernFurniture.size()] << "\n";
        break;

        case Food:
            player << "- " << foodFurniture[rand() % foodFurniture.size()] << "\n";
        break;

        case Weapons:
            player << "- " << weaponFurniture[rand() % weaponFurniture.size()] << "\n";
        break;

        case Armour:
            player << "- " << armourFurniture[rand() % armourFurniture.size()] << "\n";
        break;

        case Home:
            player << "- " << homeFurniture[rand() % homeFurniture.size()] << "\n";
        break;

        default:
        break;
    }

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