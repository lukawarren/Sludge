#pragma once
#include "Common.h"
#include "Area.h"

class Building : public Area
{
public:

    enum Type
    {
        Tavern,
        Food,
        Weapons,
        Armour,
        Home,
        NUM_TYPES
    };

    Building(const unsigned int seed, const Portal parentPortal, const Type type, const bool grand);
    ~Building();

    void LoadAreas() override;
    Cell GetStartingCell() const override;
    void Look(Player& player) const override;
    void Move(Player& player, const Direction direction, const int distance) const override;
    std::vector<ItemStack>& GetItems(const Cell cell) override;
    std::string GetPortalText() const override;

    bool grand;
    std::string name;
    Type type;

private:
    Portal parentPortal;
    std::vector<ItemStack> items;

    static std::vector<std::string> grandFurniture;
    static std::vector<std::string> grandFloors;
    static std::vector<std::string> grandWalls;

    static std::vector<std::string> humbleFurniture;
    static std::vector<std::string> humbleFloors;
    static std::vector<std::string> humbleWalls;

    static std::vector<std::string> weaponNames;
    static std::vector<std::string> armourNames;
    static std::vector<std::string> foodNames;
    static std::vector<std::string> homeNames;

    static std::vector<std::string> tavernAdjectives;
    static std::vector<std::string> tavernNouns;

    static std::vector<std::string> weaponFurniture;
    static std::vector<std::string> armourFurniture;
    static std::vector<std::string> foodFurniture;
    static std::vector<std::string> homeFurniture;
    static std::vector<std::string> tavernFurniture;
};