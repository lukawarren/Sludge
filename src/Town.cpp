#include "Town.h"
#include "PerlinNoise.hpp"
#include "Item.h"
#include "Game.h"

std::vector<std::string> Town::names;

Town::Town(const unsigned int seed, const Portal parentPortal) :
    Area(seed), parentPortal(parentPortal)
{
    width = rand() % 2 + 4;
    height = rand() % 2 + 2;
    cells = new TownCell[width*height];

    // Add exit
    portals[GetStartingCell()] = parentPortal;

    // Load names if need be then pick one, before removing it to avoid duplicates
    if (names.size() == 0) names = ReadLines("towns.txt");
    
    const size_t index = rand() % names.size();
    name = names[index];

    if (names.size() > 1)
        names.erase(names.begin() + index);
}

void Town::LoadAreas()
{
    const int nBuildings = width*2;

    for (int i = 0; i < nBuildings; ++i)
    {
        const Cell cell = rand() % (width * height);
        if (cell == GetStartingCell()) continue;

        const Portal portal = {
            Game::Get().GetAreaID(this),    // Exit area
            cell                            // Exit cell
        };
        
        cells[cell].building = new Building(rand(), portal, rand() % 100 <= 20);
        portals[cell] = Portal { Game::Get().AddArea(cells[cell].building) };
    }
}

Cell Town::GetStartingCell() const
{
    return (height-1) * width + width/2; // Bottom-middle
}

void Town::Look(Player& player) const
{
    player << "You stand in " << name << "!\n\n";

    const int playerX = player.cell % width;
    const int playerY = player.cell / width;

    // Top and bottom borders
    const auto DrawEdge = [&]()
    {
        for (int i = 0; i < width+2; ++i) player << "#";
        player << "\n";
    };

    DrawEdge();

    for (int y = 0; y < height; y++)
    {
        player << "#";

        for (int x = 0; x < width; ++x)
        {
            if (x == playerX && y == playerY) player << "X";
            else if (GetPortal(y * width + x).has_value()) player << "?";
            else player << " ";
        }

        player << "#\n";
    }

    DrawEdge();
    player << "\n";

    if (player.cell == GetStartingCell())
        player << "The exit to the wider world stands here\n";

    else if (GetPortal(player.cell).has_value())
    {
        player << "Facing the street lies a ";

        if (cells[player.cell].building->grand) player << "fine establishment";
        else player << "building like any other";
        
        player << ", " << cells[player.cell].building->name << "\n";
    }

    else
        player << "The street seems devoid of any places of note\n";
}

void Town::Move(Player& player, const Direction direction, const int distance) const
{
    Area::Move(player, direction, distance, width, height, [&](const int x, const int y)
    {
        return IsValid(x, y);
    });
}

std::vector<ItemStack>& Town::GetItems(const Cell cell)
{
    return cells[cell].items;
}

std::string Town::GetPortalText() const
{
    return "The town of " + name + " stands here...";
}

bool Town::IsValid(const int x, const int y) const
{
    return x >= 0 && y >= 0 && x < width && y < height && cells[y * width + x].present;
}

Town::~Town()
{
    delete[] cells;
}