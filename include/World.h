#pragma once
#include "Common.h"
#include "Area.h"

class World : public Area
{
public:
    World(const int width, const int height, const unsigned int seed);
    ~World();

    enum Tile : char
    {
        Grass,
        Water,
        Rock,
        None
    };

    void LoadAreas() override;
    void Render() const override;
    Cell GetStartingCell() const override;
    void Look(Player& player) const override;
    void Move(Player& player, const Direction direction, const int distance) const override;

private:
    Tile* tiles;
    int width;
    int height;
    int seed;

    Tile GetTile(Cell cell) const;
    Tile GetTile(const int x, const int y) const;

    char TileToChar(const Tile t) const;
    std::string TileToString(const Tile t) const;
};