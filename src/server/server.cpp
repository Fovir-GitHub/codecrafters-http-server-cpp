#include "server.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>

static void terminateProgram()
{
    std::exit(1);
}

int server::Server::InitializeSocket()
{
    try
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0)
            throw server::ServerException("Failed to create server socket");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    int reuse = 1;
    try
    {
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                       sizeof(reuse)) < 0)
            throw server::ServerException("setsockopt failed");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    sockaddr_in server_address;
    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port        = htons(PORT);

    try
    {
        if (bind(server_fd, (sockaddr *) &server_address,
                 sizeof(server_address)) != 0)
            throw server::ServerException("Failed to bind to port 4221");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }
}