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
    items[Item::Items::caveMushroom] =
        Item("Cave Mushroom", "a mushroom found amongst shadows and stone", [](Player& player)
    {
        player << "Bob";
    });
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

    else if (command == "look")
    {
        areas[player.area]->Look(player);
        player << "\n";
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
        player << "Level: " << player.level << "\n";
        player << "Inventory: \n";

        for (const auto &[key, value] : player.items)
        {
            player << "- " << items[key].name << " x " << value << "\n";
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
            player << "\n";
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

    else if (command == "w") { areas[player.area]->Move(player, Area::Direction::Up,    arg.value_or(1)); player << "\n"; PrintItems(player, false); }
    else if (command == "s") { areas[player.area]->Move(player, Area::Direction::Down,  arg.value_or(1)); player << "\n"; PrintItems(player, false); }
    else if (command == "a") { areas[player.area]->Move(player, Area::Direction::Left,  arg.value_or(1)); player << "\n"; PrintItems(player, false); }
    else if (command == "d") { areas[player.area]->Move(player, Area::Direction::Right, arg.value_or(1)); player << "\n"; PrintItems(player, false); }

    else player << "Unknown or invalid command\n";

    return true;
}

void Game::PrintItems(Player& player, bool showIfEmpty)
{
    const auto& itemIDs = areas[player.area]->GetItems(player.cell);

    if (itemIDs.size() == 0 && !showIfEmpty)
    {
        player << "There are no items here\n";
        return;
    }

    player << "Items:\n";

    for (size_t i = 0; i < itemIDs.size(); ++i)
        player << "- " << i+1 << " - " << items[itemIDs[i].item].name << ": " << items[itemIDs[i].item].description << " x " << itemIDs[i].number << "\n";
}

Game::~Game()
{
    for (Area* area : areas) delete area;
}