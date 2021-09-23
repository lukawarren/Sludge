#pragma once
#include "Common.h"
#include "Area.h"
#include "Building.h"

class Town : public Area
{
public:
    Town(const unsigned int seed, const Portal parentPortal);
    ~Town();

    void LoadAreas() override;
    Cell GetStartingCell() const override;
    void Look(Player& player) const override;
    void Move(Player& player, const Direction direction, const int distance) const override;
    std::vector<ItemStack>& GetItems(const Cell cell) override;
    std::string GetPortalText() const override;

private:
    struct TownCell
    {
        bool present = false;
        std::vector<ItemStack> items = {};
    };

    Portal parentPortal;
    std::string name;
    TownCell* cells;
    int width;
    int height;

    static std::vector<std::string> names;

    bool IsValid(const int x, const int y) const;
};