#include "Town.h"
#include "PerlinNoise.hpp"
#include "Item.h"
#include "Game.h"

std::vector<std::string> Town::names;

Town::Town(const unsigned int seed, const Portal parentPortal) :
    Area(seed), parentPortal(parentPortal)
{
    // Decide dimiensions
    const int pathLength = 10;
    const int wingLength = 0;
    width = wingLength*2 + 1;
    height = pathLength;

    // Create empty rooms
    cells = new TownCell[width * height];
    
    // Create path from the middle of the bottom row, upwards
    const Cell origin = GetStartingCell();

    for (int i = 0; i < pathLength; ++i)
        cells[i * width + width/2].present = true;

    // Add exit
    portals[origin] = parentPortal;

    // Load names if need be then pick one, before removing it to avoid duplicates
    if (names.size() == 0) names = Game::Get().ReadLines("towns.txt"); // TODO: remove Game::Get()
    
    const size_t index = rand() % names.size();
    name = names[index];

    if (names.size() > 1)
        names.erase(names.begin() + index);
}

void Town::LoadAreas()
{
    // Test building
    const Portal portal = {
        Game::Get().GetAreaID(this),    // Exit area
        width/2                         // Exit cell
    };

    portals[width/2] = Portal { Game::Get().AddArea(new Building(0, portal)) };
}

Cell Town::GetStartingCell() const
{
    return (height-1) * width + width/2;
}

void Town::Look(Player& player) const
{
    const auto portal = GetPortal(player.cell);
    
    if (player.cell == GetStartingCell())
    {
        player << "You stand amidst the entrance to " << name << "!\n\n";

        player << "Reading a signpost you notice the following buildings:\n";
        player << "- Gulliver's Inn\n"; 
        player << "\n";
    }

    else if (portal.has_value())
        player << "- " << Game::Get().areas[portal.value().area]->GetPortalText() << "\n\n";

    player << "A path stretches out in each of these directions:\n";

    const int worldX = player.cell % width;
    const int worldY = player.cell / width;
    if (IsValid(worldX,   worldY-1)) player << "- W\n";
    if (IsValid(worldX-1, worldY  )) player << "- A\n";
    if (IsValid(worldX,   worldY+1)) player << "- S\n";
    if (IsValid(worldX+1, worldY  )) player << "- D\n";
}

void Town::Move(Player& player, const Direction direction, const int distance) const
{
    const int worldX = player.cell % width;
    const int worldY = player.cell / width;

    // Try walking at decreasing distances until successful (or we reach 0)
    int maxDistance = distance;
    while (maxDistance > 0)
    {
        if (direction == Direction::Up && IsValid(worldX, worldY - maxDistance))
            { player.cell -= width * maxDistance; break; }
        
        else if (direction == Direction::Down && IsValid(worldX, worldY + maxDistance))
            { player.cell += width * maxDistance; break; }

        else if (direction == Direction::Left && IsValid(worldX - maxDistance, worldY))
            { player.cell -= maxDistance; break; }

        else if (direction == Direction::Right && IsValid(worldX + maxDistance, worldY))
            { player.cell += maxDistance; break; }
        
        else maxDistance--;
    }

    if (maxDistance == 0) player << "You have hit a dead end\n";
    else Look(player);
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