#ifndef _SERVER_H_
#define _SERVER_H_

#include "../http/message.h"
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

    int              server_fd;
    message::Message http_message;

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
     *@brief Listen the port for coming data
     */
    void Listen();

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

    /**
     *@brief Handle the client
     *
     * @param client_fd the client file description
     */
    void HandleClient(int client_fd);

    /**
     *@brief Set the response
     */
    void SetResponse();

    /**
     *@brief Set the response when the client calls `/echo/xxx` path
     *
     * @param request_path the parsed path
     */
    void HandleEcho(const std::vector<std::string> & request_path);

    /**
     *@brief Set the response when the client calls `/user-agent` path
     */
    void HandleUserAgent();

    /**
     *@brief Set the response when the client tries to access a file
     */
    void HandleFile(const std::vector<std::string> & request_path);

    /**
     *@brief Handle the default situation
     */
    void HandleDefault();

    /**
     *@brief Handle the GET http method
     *
     * @param client_fd the client file description
     */
    void HandleGETMethod(int client_fd);

    /**
     *@brief Handle the POST http method
     *
     * @param client_fd the client file description
     * @param request_path the parsed request path
     */
    void HandlePOSTMethod(int                              client_fd,
                          const std::vector<std::string> & request_path);

    /**
     *@brief Handle the compression operation
     */
    void HandleCompression();

    /**
     *@brief Gzip compression function
     *
     * @param data
     * @return std::string data after compression
     */
    std::string GzipCompression(const std::string & data);

    /**
     *@brief Set the `Connection' header in response
     */
    void HandleConnectionClose();
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