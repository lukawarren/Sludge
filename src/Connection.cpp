#include "Connection.h"
#include "Game.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 128

Connection::Connection(int clientFd, const std::string address)
{
    bool alive = true;

    const auto Send = [&](const std::string& string)
    {
        send(clientFd, string.c_str(), string.size(), 0);
    };

    const auto Read = [&](const std::string& prompt = "")
    {
        if (prompt != "") Send(prompt);

        // Read data into (null terminated) buffer, cutting off messages that're too long
        char buffer[BUFFER_SIZE] = {};
        ssize_t bytes = read(clientFd, buffer, sizeof(buffer));

        if (bytes < 0)
        {
            perror("Unable to read client socket");
            alive = false;
            return std::string();
        }

        else if (bytes == 0)
        {
            // Connection closed
            alive = false;
            return std::string();
        }
        
        // Remove newline and return
        auto string = std::string(buffer);
        string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
        return string;
    };

    // Send motd
    send(clientFd, Game::Get().motd.c_str(), Game::Get().motd.size(), 0);

    // Get name
    std::string name = Read("What is to be your name? ");
    if (!alive) goto terminate;
    
    Player* player;
    player = Game::Get().AddPlayer(name);
    while (player == nullptr)
    {
        Send("Name already taken!\n");
        name = Read("What is to be your name? ");

        if (!alive) goto terminate;

        player = Game::Get().AddPlayer(name);
    }

    // Name chosen, proceed
    Send("Greetings, ");
    Send(name);
    Send("!\n\n");

    while (alive)
    {
        // Do command
        const auto command = Read("> ");
        alive = Game::Get().OnCommand(command, *player);

        // Clear screen
        Send("\e[1;1H\e[2J");

        // Return output
        player->outputBuffer += "\n";
        Send(player->outputBuffer);
        player->outputBuffer = "";
    }

terminate:
    std::cout << "Connection closed at address " << address << std::endl;
    close(clientFd);
}

Connection::~Connection() {}
