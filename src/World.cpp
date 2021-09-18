#include "World.h"
#include "Common.h"
#include "PerlinNoise.hpp"
#include "Game.h"
#include "Cave.h"

World::World(const int width, const int height, const unsigned int seed) : width(width), height(height), seed(seed)
{
    tiles = new Tile[width * height];
    memset(tiles, ' ', sizeof tiles[0] * width * height);

    // Make Perlin generator with specified seed
    siv::BasicPerlinNoise<float> perlinNoise(seed);

    // At every set of integer coordinates, Perlin noise will
    // always return 0, and even if we just added a constant
    // value, results just wouldn't "flow", so we need some
    // sort of scale to fix this. The larger the scale, the
    // more "zoomed out" we become.
    const float scale = (1.0f / 5.0f);
    const int octaves = 8;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // Generate initial nosie for landmass(es)
            auto heightMap = perlinNoise.normalizedOctaveNoise2D_0_1(x * scale, y * scale, octaves);

            // Make values further from the centre lower to promote an island-ish look
            const float distanceX = std::pow(float(x) - float(width / 2), 2);
            const float distanceY = std::pow(float(y) - float(height / 2), 2);
            const float distance = std::sqrt(distanceX + distanceY) / float(width);
            heightMap -= distance * 2.0f;

            // Roughly "re-normalise" to 0.1 scale
            heightMap *= 5.0f;

            auto* tile = &tiles[y * width + x];
            if (heightMap > 2.0f) *tile = Tile::Rock;
            else if (heightMap > 0.5f) *tile = Tile::Grass;
            else *tile = Tile::Water;
        }
    }
}

void World::LoadAreas()
{
    srand(seed);
    int caveSeed = 0;

    // Go through each walkable tile and randomly generate caves
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (rand() % 100 > 95)
            {
                const auto cell = y * width + x;
                if (GetTile(cell) == Tile::Water) continue;

                auto caveArea = Game::Get().AddArea
                (
                    new Cave
                    (
                        10,                                 // Width
                        10,                                 // Height
                        ++caveSeed,                         // Seed
                        {
                            Game::Get().GetAreaID(this),    // Exit area
                            cell                            // Exit cell
                        }
                    )
                );

                // Add enterance
                portals[cell] = { caveArea };
            }
        }
    }
}

void World::Render() const
{
    // Top and bottom borders; add 2 to account for left and right borders below
    const auto DrawEdge = [&]()
    {
        for (int i = 0; i < width+2; ++i) std::cout << "#";
        std::cout << std::endl;
    };

    DrawEdge();

    for (int y = 0; y < height; ++y)
    {
        std::cout << "#";

        for (int x = 0; x < width; ++x)
        {
            const auto cell = y * width + x;
            const auto portal = GetPortal(cell);
            
            if (portal.has_value())
            {
                // Tile is a portal
                std::cout << "C";
            }
            else
            {
                // Tile is normal, render as per usual
                const auto tile = tiles[cell];
                std::cout << TileToChar(tile);
            }
        }

        std::cout << "#" << std::endl;
    }

    DrawEdge();
}

Cell World::GetStartingCell() const
{
    // Centre
    return (height / 2) * width + (width/2);
}

void World::Look(Player& player) const
{
    const int worldX = player.cell % width;
    const int worldY = player.cell / width;
    player << "- Glancing at your map you determine your position to be [" << worldX << "," << worldY << "]\n";

    // Notify player of portals
    const auto portal = GetPortal(player.cell);
    if (portal.has_value())
        player << "- You stand amidst the enterance of a mysterious cave...\n";

    const int viewWidth = 10;
    const int viewHeight = 5;

    // Top and bottom borders
    const auto DrawEdge = [&]()
    {
        for (int i = 0; i < viewWidth+1; ++i) player << "#";
        player << "\n";
    };

    player << "\n";
    DrawEdge();

    for (int viewY = -viewHeight/2+1; viewY < viewHeight/2; ++viewY)
    {
        player << "#";

        for (int viewX = -viewWidth/2+1; viewX < viewWidth/2; ++viewX)
        {
            const int x = worldX+viewX;
            const int y = worldY+viewY;

            if (x >= 0 && y >= 0 && x < width && y < height && GetPortal(y * width + x).has_value())
            {
                // Tile is a portal, render as such
                player << "C";
            }
            else
            {
                // Tile is normal, render as such
                const auto tile = GetTile(x, y);
                const char buffer[2] =
                {
                    (viewY == 0 && viewX == 0) ? 'X' : TileToChar(tile),
                    '\0'
                };

                player << buffer;
            }
        }

        player << "#\n";
    }

    DrawEdge();
}

void World::Move(Player& player, const Direction direction, const int distance) const
{
    const int worldX = player.cell % width;
    const int worldY = player.cell / width;

    const auto TileIsWalkable = [&](const int x, const int y)
    {
        const auto tile = GetTile(x, y);
        return tile != Tile::None && tile != Tile::Water;
    };

    // Try walking at decreasing distances until successful (or we reach 0)
    int maxDistance = distance;
    while (maxDistance > 0)
    {
        if (direction == Direction::Up && TileIsWalkable(worldX, worldY - maxDistance))
            { player.cell -= width * maxDistance; break; }
        
        else if (direction == Direction::Down && TileIsWalkable(worldX, worldY + maxDistance))
            { player.cell += width * maxDistance; break; }

        else if (direction == Direction::Left && TileIsWalkable(worldX - maxDistance, worldY))
            { player.cell -= maxDistance; break; }

        else if (direction == Direction::Right && TileIsWalkable(worldX + maxDistance, worldY))
            { player.cell += maxDistance; break; }
        
        else maxDistance--;
    }

    Look(player);
}

World::Tile World::GetTile(Cell cell) const
{
    bool validPosition = (cell >= 0 && cell < width*height);
    if (!validPosition) return Tile::None;

    return tiles[cell];
}

World::Tile World::GetTile(const int x, const int y) const
{
    if (x < 0 || x > width) return Tile::None;
    if (y < 0 || y > height) return Tile::None;
    return tiles[y * width + x];
}

char World::TileToChar(const Tile t) const
{
    switch (t)
    {
        case Tile::Grass:
            return '/';
        
        case Tile::Rock:
            return '+';
        
        case Tile::Water:
            return '~';

        default:
            return ' ';
    }
}

std::string World::TileToString(const Tile t) const
{
    switch (t)
    {
        case World::Tile::Grass: return "Grass";
        case World::Tile::Water: return "Water";
        case World::Tile::Rock:  return "Rock";
        case World::Tile::None:  return " ";
        default:                 return "Unknown";
    }
}

World::~World()
{
    delete[] tiles;
}
