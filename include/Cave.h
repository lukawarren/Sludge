#pragma once
#include "Common.h"
#include "Area.h"

class Cave : public Area
{
public:
    Cave(const int width, const int height, const unsigned int seed, const Portal parentPortal);
    ~Cave();

    void LoadAreas() override;
    Cell GetStartingCell() const override;
    void Look(Player& player) const override;
    void Move(Player& player, const Direction direction, const int distance) const override;
    std::vector<ItemStack>& GetItems(const Cell cell) override;
    std::string GetPortalText() const override;

private:
    struct CaveCell
    {
        bool present = false;
        std::vector<ItemStack> items = {
            { Item::Items::CaveMushroom, 2 }
        };
    };

    Portal parentPortal;
    CaveCell* cells;
    int width;
    int height;
    
    static std::vector<std::string> descriptions;

    bool IsValid(const int x, const int y) const;
};