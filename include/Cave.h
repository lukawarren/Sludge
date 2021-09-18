#pragma once
#include "Common.h"
#include "Area.h"

class Cave : public Area
{
public:
    Cave(const int width, const int height, const unsigned int seed, const Portal parentPortal);
    ~Cave();

    void LoadAreas() override;
    void Render() const override;
    Cell GetStartingCell() const override;
    
    void Look(Player& player) const override;
    void Move(Player& player, const Direction direction, const int distance) const override;

    std::vector<ItemID> GetItems(const Cell cell) const override;

private:
    struct Room
    {
        bool present = false;
    };

    Portal parentPortal;
    Room* rooms;
    int width;
    int height;
    
    static std::vector<std::string> descriptions;

    bool IsValid(const int x, const int y) const;
};