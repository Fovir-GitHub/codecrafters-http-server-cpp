#include "http_server.h"
#include "http_server_exception.h"
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

HttpServer::HttpServer(const int pt) : PORT(pt)
{
    try
    {
        buffer.resize(HTTP_SERVER_CONSTANT::BUFFER_SIZE);
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }

    std::cout << "Logs from your program will appear here!\n";

    try
    {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw HttpServerException("Failed to create server socket");
    }
    catch (const HttpServerException & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }

    try
    {
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &REUSE,
                       sizeof(REUSE)) < 0)
            throw HttpServerException("setsockopt failed");
    }
    catch (const HttpServerException & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }

    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port        = htons(PORT);

    try
    {
        if (bind(server_fd, (sockaddr *) &server_address,
                 sizeof(server_address)) != 0)
            throw HttpServerException("Failed to bind to port " +
                                      std::to_string(PORT));
    }
    catch (const HttpServerException & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }
}

HttpServer::~HttpServer()
{
    close(server_fd);
    close(client_fd);
}

void HttpServer::Listen()
{
    try
    {
        if (listen(server_fd, HTTP_SERVER_CONSTANT::MAX_CONNECT_CLIENT) != 0)
            throw HttpServerException("listen failed");
    }
    catch (const HttpServerException & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }

    std::cout << "Waiting for a client to connect...\n";

    return;
}

int HttpServer::SetClient()
{
    int client_address_length = sizeof(client_address);

    try
    {
        if ((client_fd = accept(server_fd, (sockaddr *) &client_address,
                                (socklen_t *) &client_address_length)) < 0)
            throw HttpServerException("accept failed");
    }
    catch (const HttpServerException & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }

    return client_fd;
}

void HttpServer::HandleClient(int client)
{
    int backup_client_fd = client_fd;
    client_fd            = client;

    while (true)
    {
        if (!this->Receive())
            break;

        this->Send();
    }

    client_fd = backup_client_fd;

    return;
}

void HttpServer::Send(const std::string & message)
{
    http_message.SetBody(message);
    try
    {
        if (send(client_fd, http_message.MakeResponse().c_str(),
                 http_message.MakeResponse().size(), 0) < 0)
            throw HttpServerException("Send failed");
    }
    catch (const HttpServerException & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }
}

void HttpServer::Send(const std::vector<std::string> & message_lines)
{
    std::string total_message("");

    for (const std::string & msg : message_lines)
        total_message.append(msg + "\r\n");

    this->Send(total_message);

    return;
}

bool HttpServer::Receive()
{
    ssize_t receive_bytes;

    try
    {
        if ((receive_bytes = recv(client_fd, &buffer[0], buffer.size(), 0)) < 0)
            throw HttpServerException("receive failed");
    }
    catch (const HttpServerException & e)
    {
        std::cerr << e.what() << '\n';
        std::exit(HTTP_SERVER_ERROR_CODE);
    }

    http_message = HttpMessage(buffer);

    return receive_bytes != 0;
}
