#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <arpa/inet.h>
#include <memory>
#include <string>
#include <sys/socket.h>

class HttpServer
{
private:
    enum HTTP_SERVER_CONSTANT {
        BUFFER_SIZE        = 1024,
        MAX_CONNECT_CLIENT = 5,
    };

    const int HTTP_SERVER_ERROR_CODE = 1;
    const int REUSE                  = 1;
    const int PORT;

    sockaddr_in server_address;
    sockaddr_in client_address;
    std::string buffer;
    int         server_fd;
    int         client_fd;

public:
    HttpServer(const int pt = 4221);
    ~HttpServer();

    void Listen();

    void SetClient();

    void Send(const std::string & message);

    bool Receive();
};

#endif // !_HTTP_SERVER_H_