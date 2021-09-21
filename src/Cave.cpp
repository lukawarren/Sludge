#include "Cave.h"
#include "PerlinNoise.hpp"
#include "Item.h"
#include "Game.h"

std::vector<std::string> Cave::descriptions;

Cave::Cave(const int width, const int height, const unsigned int seed, const Portal parentPortal) :
    Area(seed), parentPortal(parentPortal), width(width), height(height)
{
    // Create empty rooms
    rooms = new Room[width * height];

    // Random-walk to generate cave, starting from centre
    const int numRooms = 5;
    const int steps = 5;

    while (1)
    {
        int x = width / 2;
        int y = height / 2;

        for (int i = 0; i < steps; ++i)
        {
            const Direction direction = static_cast<Direction>(rand() % NUM_DIRECTIONS);

            if (direction == Direction::Up) y--;
            else if (direction == Direction::Down) y++;
            else if (direction == Direction::Left) x--;
            else x++;

            // Clamp to reasonable bounds
            x = std::max(std::min(x, width-1), 0);
            y = std::max(std::min(y, height-1), 0);
            rooms[y * width + x].present = true;
        }

        // If number of set rooms exceeds minimum, continuue
        int count = 0;
        for (int i = 0; i < width*height; ++i)
            if (rooms[i].present) count++;
        
        if (count >= numRooms) break;
    }

    // Add exit
    portals[GetStartingCell()] = parentPortal;

    // Load descriptions if need be
    if (descriptions.size() > 0) return;

    std::ifstream file("../data/caves.txt");

    if (file.fail())
        std::cerr << "Could not open cave descriptions (caves.txt)!" << std::endl;

    std::string buffer;
    while (std::getline(file, buffer))
        descriptions.push_back(buffer);
}

void Cave::LoadAreas()
{
    // Add weapons to rooms
    const int weaponChance = 30;
    const int enemyChance = 10;

    for (int i = 0; i < width*height; ++i)
    {
        if (rooms[i].present && rand() % 100 <= weaponChance)
            rooms[i].items.emplace_back
            (
                ItemStack 
                {
                    GetWeightedRandomItem(Game::Get().weapons),
                    1
                }
            );

        if (rooms[i].present && rand() % 100 <= enemyChance)
            enemies[i] = { Enemy::Enemies::Gremlin, Game::Get().enemies[Enemy::Enemies::Gremlin].maxHealth };
    }
}

void Cave::Render() const
{
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
            if (rooms[y * width + x].present) std::cout << "X";
            else std::cout << " ";
        }

        std::cout << "#" << std::endl;
    }

    DrawEdge();
}

Cell Cave::GetStartingCell() const
{
    // Return first room we find
    for (int i = 0; i < width*height; ++i)
        if (rooms[i].present)
            return i;

    return 0;
}

void Cave::Look(Player& player) const
{
    player << "You stand in a cave:\n"; 

    // Check enough descriptions exist
    const size_t numDescriptions = 2;
    if (descriptions.size() < numDescriptions)
    {
        player << "It appears this place is simply indescribable...\n";
        return;
    }
    
    // Print exit (if need be)
    if (GetPortal(player.cell).has_value())
        player << "- You see a crack of light eminating from a crevice - the way out is here...\n";

    // Pick seed for tile then display some random descriptions
    srand(player.cell);
    std::set<size_t> usedIndexes;
    for (size_t i = 0; i < numDescriptions; ++i)
    {
        // Chose *unique* index
        size_t index = rand() % descriptions.size();
        while (usedIndexes.count(index))
            index = rand() % descriptions.size();

        player << "- " << descriptions[index] << "\n";
        usedIndexes.insert(index);
    }

    // Print paths
    player << "\nWays forward:\n";

    const int worldX = player.cell % width;
    const int worldY = player.cell / width;
    if (IsValid(worldX,   worldY-1)) player << "- W\n";
    if (IsValid(worldX-1, worldY  )) player << "- A\n";
    if (IsValid(worldX,   worldY+1)) player << "- S\n";
    if (IsValid(worldX+1, worldY  )) player << "- D\n";
}

void Cave::Move(Player& player, const Direction direction, const int distance) const
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

std::vector<ItemStack>& Cave::GetItems(const Cell cell) const
{
    return rooms[cell].items;
}

bool Cave::IsValid(const int x, const int y) const
{
    return x >= 0 && y >= 0 && x < width && y < height && rooms[y * width + x].present;
}

Cave::~Cave()
{
    delete[] rooms;
}