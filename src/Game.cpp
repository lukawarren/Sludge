#include "Game.h"
#include "World.h"
#include "Cave.h"

Game::Game()
{
    const auto ReadFile = [](const std::string& filename)
    {
        std::ifstream file("../data/" + filename);

        if (file.fail())
            std::cerr << "Could not open " << filename << std::endl;

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    };

    // Load variables
    motd = ReadFile("motd.txt");

    // Create items
    items.emplace_back(Item("Cave Mushroom", "a mushroom found amongst shadows and stone"));

    // Weapons
    const std::vector<std::pair<std::string, std::string>> descriptions =
    {
        { "Broken", "pitiable if nothing else" },
        { "Rusty", "tetanus might do the trick" },
        { "Grimy", "in need of a good clean" },
        { "Grim", "the more you look the worse it becomes" },
        { "Flimsy", "just don't drop it" },
        { "Inoffensive", "not likely to inspire much fear" },
        { "Passable", "good enough to do the job" },
        { "Pleasing", "rather nice-looking" },
        { "Gleaming", "surprisingly welll maintained" },
        { "Refined", "a more sensible option" },
        { "Sharp", "looking to do some serious business" },
        { "Threatening", "more than just a suggestion" },
        { "Terrifying", "the envy of your foes" },
        { "Deadly", "the nightmare of all who disagree with you" }
    };

    const std::vector<std::string> nouns =
    {
        "Dagger",
        "Shortsword",
        "Longsword",
        "Axe",
        "Mace"
    };

    for (size_t description = 0; description < descriptions.size(); ++description)
    {
        std::vector<ItemID> descriptionItems;
        descriptionItems.reserve(nouns.size());

        for (size_t noun = 0; noun < nouns.size(); ++noun)
        {
            // Weapons are added, description-wise, in order of rarity
            items.emplace_back
            (
                descriptions[description].first + " " + nouns[noun],
                descriptions[description].second,
                noun * 10 + description
            );

            descriptionItems.emplace_back(items.size()-1);
        }

        weapons.emplace_back(descriptionItems);
    }

    // Create enemies
    enemies.emplace_back
    (
        std::vector<Enemy::Attack>
        {
            {
                { "slashes", "moves to eviscerate", "lunges at", "scratches", "brushes" },
                "claws"
            }
        }, 
        "Gremlin", 30, 80
    );

}

Player* Game::AddPlayer(const std::string& name)
{
    if (players.count(name)) return nullptr;
    players.emplace(name, Player(name, 0, areas[0]->GetStartingCell()));

    return &players.at(name);
}

Player* Game::GetPlayer(const std::string& name)
{
    if (!players.count(name)) return nullptr;
    return &players.at(name);
}

AreaID Game::AddArea(Area* area)
{
    areas.emplace_back(area);
    area->LoadAreas();
    return areas.size()-1;
}

AreaID Game::GetAreaID(Area* area)
{
    for (size_t i = 0; i < areas.size(); ++i)
        if (areas[i] == area)
            return i;
    
    return 0;
}

void Game::LoadAreas()
{
    AddArea(new World(64, 32, 123));
    areas[0]->Render();
    std::cout << "Game loaded" << std::endl;
}

bool Game::OnCommand(const std::string& string, Player& player)
{
    // Check for non-empty message
    if (string.size() == 0)
    {
        player << "Unknown or invalid command\n";
        return true;
    }

    // Parse command
    const char* data = string.data();
    std::string command;
    unsigned int i = 0;
    while (data[i] != '\0' && data[i] != ' ')
    {
        command += data[i];
        i++;
    }

    // Space remains for arguments; treat as int
    std::optional<int> arg;
    if (i+1 < string.size())
    {
        if (std::all_of(string.begin()+i+1, string.end(), ::isdigit))
            arg = std::stoi(data+i+1);
    }

    if (command == "help")
    {
        player << "- attack [enemy] - assault a poor soul\n";
        player << "- wield [item] - equip item for combat\n";
        player << "- look - see surrounding area\n";
        player << "- items - view nearby items\n";
        player << "- get [item] - pickup item\n";
        player << "- info - view player stats\n";
        player << "- move - move to new area\n";
        player << "- quit - disconnect\n";
        player << "- help - list commands\n";
        player << "- w - move up\n";
        player << "- s - move down\n";
        player << "- a - move left\n";
        player << "- d - move right\n";
    }

    else if (command == "attack")
    {
        OnCombat(player, enemies[0]);
    }

    else if (command == "wield" && arg.has_value())
    {
        // Check item ID is valid
        if ((unsigned int) arg.value() > player.items.size()) player << "You have no such item\n";
        else
        {
            // Find Nth item and equip
            int i = 1;
            for (const auto &[key, value] : player.items)
            {
                if (i == arg.value()) player.weapon = key;
                i++;
            }
        }
    }

    else if (command == "look")
    {
        areas[player.area]->Look(player);
        PrintItems(player, false);
    }

    else if (command == "items")
    {
        PrintItems(player, true);
    }

    else if (command == "get" && arg.has_value())
    {
        auto& itemStacks = areas[player.area]->GetItems(player.cell);

        // Check item ID and args exists
        if ((unsigned int)arg.value() <= itemStacks.size())
        {
            // Give item to player
            const auto itemID = itemStacks[arg.value()-1].item;
            const auto amount = itemStacks[arg.value()-1].number;
            player.items[itemID] += amount;
            player << "You pick up a " << items[itemID].name << " (x" << amount << ")\n";

            // Remove item from area
            itemStacks.erase(itemStacks.begin() + arg.value()-1);
        }
        else
            player << "That item does not exist here\n";
    }

    else if (command == "info")
    {
        player << "Name: " << player.name << "\n";
        player << "Level: " << player.level << "\n\n";
        player << "Inventory: \n";

        int i = 1;
        for (const auto &[key, value] : player.items)
        {
            player << "- " << i++ << " - " << items[key].name << ": " << items[key].description << " (x" << value << ")\n";

            if (items[key].attack > 1)
                player << "----> Attack: " << items[key].attack << "\n";

            if (player.weapon.has_value() && player.weapon.value() == key)
                player << "----> You are wielding this\n";
        }

        if (player.items.size() == 0) player << "- You have no items\n";

    }

    else if (command == "move")
    {
        const auto portal = areas[player.area]->GetPortal(player.cell);
        if (portal.has_value())
        {
            const auto area = portal.value().area;
            player.area = area;
            player.cell = portal.value().cell.value_or(areas[area]->GetStartingCell());

            areas[player.area]->Look(player);
            PrintItems(player, false);
        }

        else
            player << "You'll find no enterance or exit here!\n";
    }

    else if (command == "quit")
    {
        player << "Farewell!\n";
        return false;
    }

    else if (command == "w") { areas[player.area]->Move(player, Area::Direction::Up,    arg.value_or(1)); PrintItems(player, false); }
    else if (command == "s") { areas[player.area]->Move(player, Area::Direction::Down,  arg.value_or(1)); PrintItems(player, false); }
    else if (command == "a") { areas[player.area]->Move(player, Area::Direction::Left,  arg.value_or(1)); PrintItems(player, false); }
    else if (command == "d") { areas[player.area]->Move(player, Area::Direction::Right, arg.value_or(1)); PrintItems(player, false); }

    else player << "Unknown or invalid command\n";

    return true;
}

void Game::PrintItems(Player& player, bool showIfEmpty)
{
    const auto& itemIDs = areas[player.area]->GetItems(player.cell);

    if (itemIDs.size() == 0 && !showIfEmpty) return;
    else if (itemIDs.size() == 0)
    {
        player << "There are no items here\n";
        return;
    }

    if (!showIfEmpty) player << "\n";
    player << "Items:\n";

    for (size_t i = 0; i < itemIDs.size(); ++i)
        player << "- " << i+1 << " - " << items[itemIDs[i].item].name << ": " << items[itemIDs[i].item].description << " (x" << itemIDs[i].number << ")\n";
}

void Game::OnCombat(Player& player, Enemy& enemy)
{
    const std::vector<std::string> attackPrefixes =
    {
        "With considerable effort,",
        "Surprisingly gracefully,"
        "With a sudden jolt",
        "Terrified",
        "Unfazed"
    };

    const std::vector<std::string> attackVerbs =
    {
        "bludgeon",
        "whack",
        "smack",
        "whallop",
        "batter",
        "bruise",
        "move to eviscerate",
    };

    const std::vector<std::string> attackObjects
    {
        "it",
        "the poor thing",
        "the sucker",
        "the creature",
    };

    const std::vector<std::string> minimalDamageDescriptions =
    {
        "It looks much the same as before.",
        "You hardly notice a difference.",
        "Did that do anything?"
    };

    const std::vector<std::string> moderateDamageDescriptons =
    {
        "It looks bruised now.",
        "The scoundrel whinces in pain!",
        "You see a few cuts here and there.",
        "Vengance shall be yours!",
        "It teeters slightly."
    };

    const std::vector<std::string> majorDamageDescriptions =
    {
        "You see its insides slop to the floor somewhat.",
        "A few limbs appear to have been misplaced.",
        "There isn't much more there than a grim oozing mess.",
        "It's hard to say where one slash began and and another ended..."
    };

    const float damageVariation = 0.2f;
    const int prefixChance = 50;
    const int objectChance = 50;
    bool playersTurn = true;

    if (player.weapon.has_value())
        player << "You ready your " << items[player.weapon.value()].name << "\n";
    else
        player << "Empty-handed, you stand there passively.\n";

    while (player.health > 0 && enemy.health > 0)
    {
        if (playersTurn && player.weapon.has_value())
        {
            player << "- ";

            // Prefix
            if (rand() % 100 <= prefixChance)
            {
                player << attackPrefixes[rand() % attackPrefixes.size()];
                player << " you ";
            }
            else player << "You ";
            
            // Verb
            player << attackVerbs[rand() % attackVerbs.size()] << " " ;

            // Object
            if (rand() % 100 <= objectChance)
                player << attackObjects[rand() % attackObjects.size()];
            else
                player << "the " << enemy.name;

            // Actually attack, plus or minus some variation
            const float variation =
                (
                    (float) rand() / (float) RAND_MAX //  0 to 1
                    * 2.0f - 1.0f                     // -1 to 1
                ) 
                * damageVariation + 1.0f;             // -whatever to +whatever
            
            enemy.health -= items[player.weapon.value()].attack * variation;
            player << ". ";
            
            // Work out how damaged the enemy is
            float enemyHealthPercent = (float)enemy.health / (float)enemy.maxHealth;

            if (enemyHealthPercent > 0.6f)
                player << minimalDamageDescriptions[rand() % minimalDamageDescriptions.size()];
            
            else if (enemyHealthPercent > 0.3f)
                player << moderateDamageDescriptons[rand() % moderateDamageDescriptons.size()];
                
            else
                player << majorDamageDescriptions[rand() % majorDamageDescriptions.size()];

            player << "\n";
        }
    }

    if (player.health < 0)
    { 
        // TODO: kill player and delete from server
    }

    else
    {
        player << "The " << enemy << " has been slaughtered!\n";
        // TODO: delete enemy
    }
}

Game::~Game()
{
    for (Area* area : areas) delete area;
}