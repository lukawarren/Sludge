#pragma once
#include "Common.h"
#include "Area.h"

class Building : public Area
{
public:
    Building(const unsigned int seed, const Portal parentPortal, const bool grand = false);
    ~Building();

    void LoadAreas() override;
    Cell GetStartingCell() const override;
    void Look(Player& player) const override;
    void Move(Player& player, const Direction direction, const int distance) const override;
    std::vector<ItemStack>& GetItems(const Cell cell) override;
    std::string GetPortalText() const override;

    bool grand;
    std::string name;

private:
    Portal parentPortal;
    std::vector<ItemStack> items;

    static std::vector<std::string> grandFurniture;
    static std::vector<std::string> grandFloors;
    static std::vector<std::string> grandWalls;

    static std::vector<std::string> humbleFurniture;
    static std::vector<std::string> humbleFloors;
    static std::vector<std::string> humbleWalls;
};