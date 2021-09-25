#include "World.h"
#include "Common.h"
#include "PerlinNoise.hpp"
#include "Game.h"
#include "Cave.h"
#include "Town.h"

World::World(const int width, const int height, const unsigned int seed) : Area(seed), width(width), height(height)
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
    const int nCaves = width * height / 512;
    const int nTowns = 4;

    unsigned int caveSeed = rand();
    unsigned int townSeed = rand();

    const auto OnRandomPosition = [&](const int count, const int minDistanceFromCentre, const std::function<Area*(const Portal)> f)
    {
        for (int i = 0; i < count; ++i)
        {
            // Pick valid cell
            auto cellX = rand() % width;
            auto cellY = rand() % height;

            while (GetTile(cellY * width + cellX) == Tile::Water || sqrtf(cellX*cellX + cellY*cellY) < minDistanceFromCentre)
            {
                cellX = rand() % width;
                cellY = rand() % height;
            }

            // Add portal
            const Portal portal = {
                Game::Get().GetAreaID(this),    // Exit area
                cellY * width + cellX           // Exit cell
            };
            portals[cellY * width + cellX] = { Game::Get().AddArea(f(portal)) };
        }
    };

    // Generate caves
    OnRandomPosition(nCaves, 0, [&](const Portal portal)
    {
        return new Cave
        (
            10,                                 // Width
            10,                                 // Height
            ++caveSeed,                         // Seed
            portal
        );
    });

    // Generate towns
    std::vector<Cell> towns;
    OnRandomPosition(nTowns, 5, [&](const Portal portal)
    {
        towns.emplace_back(portal.cell.value());
        return new Town(++townSeed, portal);
    });

    // Generate paths
    Cell centre = height / 2 * width + width / 2;
    for (size_t i = 0; i < towns.size(); ++i)
        CreatePath(towns[i], centre);
}

/*
    Uses A* path-finding
    https://en.wikipedia.org/wiki/A*_search_algorithm
*/
void World::CreatePath(const Cell startCell, const Cell endCell)
{
    // Make nodes
    struct Node
    {
        int x;
        int y;
        std::vector<Node*> neighbours;
        bool walkable;

        bool visited = false;
        Node* parent = nullptr;
        float localGoal = std::numeric_limits<float>::max();
        float globalGoal = std::numeric_limits<float>::max();
    };
    
    std::vector<Node> nodes(width * height, Node {});
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        { 
            nodes[y * width + x].x = x,
            nodes[y * width + x].y = y;
            nodes[y * width + x].walkable = tiles[y * width + x] != Tile::Water;
        }
    }

    // Work out neighbours
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        { 
            auto& node = nodes[y * width + x];

            if (y > 0)          node.neighbours.push_back(&nodes[(y - 1) * width + (x + 0)]);
            if (y < height-1)   node.neighbours.push_back(&nodes[(y + 1) * width + (x + 0)]);
            if (x > 0)          node.neighbours.push_back(&nodes[(y + 0) * width + (x - 1)]);
            if (x < width-1)    node.neighbours.push_back(&nodes[(y + 0) * width + (x + 1)]);
        }
    }

    const auto Distance = [](Node* a, Node* b)
    {
        // Normally, we would just use pythagoras but on terminal screens, characters are
        // twice as tall as they are long, and so this makes for weird results! To solve this,
        // make all vertical measurements twice as large.
        return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
    };

    // Starting conditions
    Node* start = &nodes[startCell];
    Node* end = &nodes[endCell];
    Node* current = start;
    current->localGoal = 0.0f;
    current->globalGoal = Distance(start, end);    

    // Have "not tested" list for processing
    std::list<Node*> untestedNodes = { start };

    // Continue until *a* path is found - not necessarily the absolute best path, but a path nonetheless
    while (!untestedNodes.empty() && current != end)
    {
        // Sort nodes by global goal, with lowest first
        untestedNodes.sort([](const Node* lhs, const Node* rhs)
        {
            return lhs->globalGoal < rhs->globalGoal;
        });

        // Ditch nodes already visited at front of the list....
        while (!untestedNodes.empty() && untestedNodes.front()->visited)
            untestedNodes.pop_front();

        // ...then if we're in trouble, abandon ship
        if (untestedNodes.empty()) break;

        // Node chosen, proceed
        current = untestedNodes.front();
        current->visited = true;
        
        for (auto neighbour : current->neighbours)
        {
            // If not visited, duly note down for later use
            if (!neighbour->visited && neighbour->walkable)
                untestedNodes.push_back(neighbour);
            
            // Work out potential lowest parent distance
            float possibleLowestGoal = current->localGoal + Distance(current, neighbour);

            // Update local goal of neighbour if value can be pushed lower by way of our parent
            if (possibleLowestGoal < neighbour->localGoal)
            {
                neighbour->parent = current;
                neighbour->localGoal = possibleLowestGoal;
                neighbour->globalGoal = neighbour->localGoal + Distance(neighbour, end);
            }
        }
    }

    // Walk from end node to start node, following parents and creating paths
    if (end != nullptr)
    {
        Node* n = end;
        while (n->parent != nullptr)
        {
            tiles[n->y * width + n->x] = Tile::Path;
            n = n->parent;
        }
    }
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
        player << "- " << Game::Get().areas[portal->area]->GetPortalText() << "\n";

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

            if (viewY == 0 && viewX == 0)
                player << "X"; // Print player
            
            else if (x >= 0 && y >= 0 && x < width && y < height && GetPortal(y * width + x).has_value())
            {
                // Tile is a portal, render as such
                player << "?";
            }
            else
            {
                // Tile is normal, render as such
                const auto tile = GetTile(x, y);
                const char buffer[2] =
                {
                    TileToChar(tile),
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
    Area::Move(player, direction, distance, width, height, [&](const int x, const int y)
    {
        const auto tile = GetTile(x, y);
        return tile != Tile::None && tile != Tile::Water;
    });
}

std::vector<ItemStack>& World::GetItems(const Cell cell)
{
    static std::vector<ItemStack> nullVector;
    return nullVector;
}

std::string World::GetPortalText() const { return ""; }

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

        case Tile::Path:
            return '#';

        default:
            return ' ';
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
                std::cout << "?";
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

World::~World()
{
    delete[] tiles;
}
