#pragma once
#include "Area.h"
#include "Item.h"
#include "Player.h"
#include "Enemy.h"

class Game
{
public:

    static Game& Get()
    {
        // Guaranteed to be instantiated and destroyed by the compiler
        static Game game;
        return game;
    }

    Game(Game const&) = delete;
    void operator=(Game const&) = delete;

private:
    Game();
    ~Game();

public:
    std::unordered_map<std::string, Player> players;
    std::vector<Area*> areas;

    std::string motd;
    unsigned int seed;
    std::vector<std::string> vendorNames;

    std::vector<std::vector<ItemID>> weapons;
    std::vector<std::vector<ItemID>> armours;
    std::vector<ItemID> drinks;
    std::vector<ItemID> foods;
    std::vector<Enemy> enemies;
    std::vector<Item> items;
    
    std::vector<std::string> attackVerbs;
    std::vector<std::string> attackMinimalDamageDescriptions;
    std::vector<std::string> attackModerateDamageDescriptons;
    std::vector<std::string> attackMajorDamageDescriptions;

    std::vector<std::string> combatPrefixes;
    std::vector<std::string> combatObjects;
    
    std::vector<std::string> defenceMinimalDamageDescriptions;
    std::vector<std::string> defenceModerateDamageDescriptons;
    std::vector<std::string> defenceMajorDamageDescriptions;

    Player* AddPlayer(const std::string& name);
    Player* GetPlayer(const std::string& name);

    AreaID AddArea(Area* area);
    AreaID GetAreaID(Area* area);

    // Add areas in separate method as they use Game::Get(), and
    // we can't be referencing ourselves when we're still in the
    // process of getting created - we'll just get into an infinite
    // initialisation loop!
    void LoadAreas();

    bool OnCommand(const std::string& string, Player& player);
    void PrintItems(Player& player, bool showIfEmpty);
    void PrintItems(Player& player, const std::vector<ItemStack>& itemStacks);
    bool OnCombat(Player& player, EnemyInstance& enemy);
};