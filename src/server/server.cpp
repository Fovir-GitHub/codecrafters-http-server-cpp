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
        server_fd = socket(AF_INET, SOCK_STREAM, 0); /* Set server_fd */
        if (server_fd < 0) /* Failed to create socket */
            throw server::ServerException("Failed to create server socket");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    // Allow to reuse the port
    int reuse = 1;
    try
    {
        // If failed to set the socket option
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                       sizeof(reuse)) < 0)
            throw server::ServerException("setsockopt failed");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    // Bind socket to the address
    sockaddr_in server_address;
    server_address.sin_family      = AF_INET;     /* Use IPV4 */
    server_address.sin_addr.s_addr = INADDR_ANY;  /* Use 0.0.0.0 */
    server_address.sin_port        = htons(PORT); /* Set port */

    try
    {
        // Fail to bind the port
        if (bind(server_fd, (sockaddr *) &server_address,
                 sizeof(server_address)) != 0)
            throw server::ServerException("Failed to bind to port 4221");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    return server_fd;
}

int server::Server::AcceptClient()
{
    // Set the client
    sockaddr_in client_address;
    int         client_address_length = sizeof(client_address);

    std::cout << "Waiting for a client to connect...\n";

    // Accept the connection from client
    int client_fd = accept(server_fd, (sockaddr *) &client_address,
                           (socklen_t *) &client_address_length);

    // Return the client_fd
    return client_fd;
}

std::string server::Server::Receive(int client_fd)
{
    // Received data
    std::string received(BUFFER_LENGTH, '\0');
    ssize_t     receive_bytes; /* Received bytes */

    try
    {
        // Fail to receive data
        if ((receive_bytes =
                 recv(client_fd, &received[0], received.size(), 0)) < 0)
            throw server::ServerException("receive failed");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
    }

    /*
    If the receive_bytes is 0, it means the connection is closed.
    By returning empty string, the main program can handle the situation.
    */
    return receive_bytes == 0 ? "" : received;
}
