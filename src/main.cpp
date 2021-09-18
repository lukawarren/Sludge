#include "Connection.h"
#include "Common.h"
#include "Game.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 4000
#define BACKLOG 128

int main()
{
    // Create socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
    {
        perror("Unable to create socket");
        exit(-1);
    }

    // Tell the kernel that, in cases where the socket has been recently used 
    // (i.e. we just crashed and restarted), we don't care for waiting the 30
    // or so seconds to make a new one, we're happy to just ditch the old and start afresh.
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("Unable to reuse sockets");
        exit(-1);
    }

    // Bind to port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("Unable to bind to port");
        exit(-1);
    }

    // Mark the socket as a "passive socket"; i.e we want to listen
    if (listen(serverFd, BACKLOG) < 0)
    {
        perror("Unable to listen with socket");
        exit(-1);
    }

    std::cout << "Sludge running on port " << PORT << std::endl;

    // Load game
    Game::Get().LoadAreas();

    while (1)
    {
        // Wait until connection
        sockaddr_in clientAddress;
        unsigned int clientAddressSize = sizeof(clientAddress);
        int clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);
       
        if (clientFd < 0) perror("Unable to accept client connection");

        // Get IP (internally stored per-thread, so copy)
        std::string address = inet_ntoa(clientAddress.sin_addr);
        std::cout << "New connection received from " << address << std::endl;

        // Make new connection on new thread
        std::thread([&]()
        {
            Connection(clientFd, address);
        }).detach();
    }
    
    close(serverFd);
    return 0;
}
