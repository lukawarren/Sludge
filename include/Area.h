#pragma once
#include "Player.h"

class Area
{
public:

    enum Direction
    {
        Up,
        Down,
        Left,
        Right,
        NUM_DIRECTIONS
    };

    struct Portal
    {
        AreaID area;
        std::optional<Cell> cell;
    };

    virtual ~Area() {}
    virtual void LoadAreas() = 0;

    virtual void Render() const = 0;
    virtual Cell GetStartingCell() const = 0;
    virtual void Look(Player& player) const = 0;
    virtual void Move(Player& player, const Direction direction, const int distance) const = 0;
    
    virtual std::vector<ItemID> GetItems(const Cell cell) const
    {
        return {};
    }
    
    std::optional<Portal> GetPortal(const Cell cell) const
    {
        if (portals.count(cell)) return portals.at(cell);
        return {};
    }


protected:
    std::unordered_map<Cell, Portal> portals;
};