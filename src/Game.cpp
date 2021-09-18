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
        player << "- move - move to new area\n";
        player << "- quit - disconnect\n";
        player << "- help - list commands\n";
        player << "- w - move up\n";
        player << "- s - move down\n";
        player << "- a - move left\n";
        player << "- d - move right\n";
    }

    else if (command == "look") areas[player.area]->Look(player);

    else if (command == "move")
    {
        const auto portal = areas[player.area]->GetPortal(player.cell);
        if (portal.has_value())
        {
            const auto area = portal.value().area;
            player.area = area;
            player.cell = portal.value().cell.value_or(areas[area]->GetStartingCell());
            areas[player.area]->Look(player);
        }

        else
            player << "You'll find no enterance or exit here!\n";
    }

    else if (command == "quit")
    {
        player << "Farewell!\n";
        return false;
    }

    else if (command == "w") areas[player.area]->Move(player, Area::Direction::Up,    arg.value_or(1));
    else if (command == "s") areas[player.area]->Move(player, Area::Direction::Down,  arg.value_or(1));
    else if (command == "a") areas[player.area]->Move(player, Area::Direction::Left,  arg.value_or(1));
    else if (command == "d") areas[player.area]->Move(player, Area::Direction::Right, arg.value_or(1));

    else player << "Unknown or invalid command\n";

    return true;
}

Game::~Game()
{
    for (Area* area : areas) delete area;
}