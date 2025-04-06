#ifndef _SERVER_H_
#define _SERVER_H_

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#define BEGIN_SERVER_NAMESPACE \
    namespace server           \
    {
#define END_SERVER_NAMESPACE }

BEGIN_SERVER_NAMESPACE

class Server
{
private:
    enum { BUFFER_LENGTH = 1024 };

    const int PORT = 4221;

    static int server_fd;

public:
    Server(int port) : PORT(port) {}

    /**
     *@brief Initialize the socket by setting the socket options
     * and bind it to the port
     *
     * @return int server_fd
     */
    int InitializeSocket();

    /**
     *@brief Accept the connection from client
     *
     * @return int client_fd
     */
    int AcceptClient();

    /**
     *@brief Receive data from client
     *
     * @param client_fd the client
     * @return std::string the data received from client
     */
    std::string Receive(int client_fd);

    /**
     * @brief Send message to the client
     *
     * @param client_fd the client
     * @param message the message to be sent
     */
    void Send(int client_fd, const std::string & message);
};

class ServerException : std::exception
{
private:
    std::string message;

public:
    explicit ServerException(const std::string & msg) : message(msg) {}

    const char * what() const noexcept override { return message.c_str(); }
};

END_SERVER_NAMESPACE

#endif // !_SERVER_H_