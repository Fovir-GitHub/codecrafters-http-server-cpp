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
    const int PORT = 4221;

    int server_fd;

public:
    Server(int port) : PORT(port) {}

    int InitializeSocket();
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