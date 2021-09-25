#pragma once
#include "Player.h"
#include "Item.h"
#include "Enemy.h"
#include "Vendor.h"

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

    Area(const unsigned int seed) : seed(seed)
    {
        srand(seed);
        generator.seed(seed);
    }

    virtual ~Area() {}
    virtual void LoadAreas() = 0;
    
    virtual Cell GetStartingCell() const = 0;
    virtual void Look(Player& player) const = 0;
    virtual void Move(Player& player, const Direction direction, const int distance) const = 0;
    virtual std::vector<ItemStack>& GetItems(const Cell cell) = 0;
    virtual std::string GetPortalText() const = 0;

    template<typename F>
    void Move(Player& player, const Direction direction, const int distance, const int width, const int height, F IsValid) const
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

    std::optional<Portal> GetPortal(const Cell cell) const
    {
        if (portals.count(cell)) return portals.at(cell);
        return {};
    }

    Vendor* GetVendor(const Cell cell)
    {
        if (vendors.count(cell)) return &vendors.at(cell);
        return {};
    }

    EnemyInstance* GetEnemy(const Cell cell)
    {
        if (enemies.count(cell)) return &enemies.at(cell);
        return nullptr;
    }

    void DestroyEnemy(const Cell cell)
    {
        enemies.erase(cell);
    }

    ItemID GetWeightedRandomItem(const std::vector<std::vector<ItemID>>& list)
    {
        // Items near the start of the list should be more common and to this end
        // pick a random number acccording to a gaussian distribution
        std::normal_distribution<float> distribution(list.size() / 2 - 1, 2);

        // Prefix chosen; chose item
        const auto prefix = list[distribution(generator)];
        return prefix[rand() % prefix.size()];
    }

    unsigned int seed;

protected:
    std::unordered_map<Cell, Portal> portals;
    std::unordered_map<Cell, Vendor> vendors;
    std::unordered_map<Cell, EnemyInstance> enemies;
    std::default_random_engine generator;
};