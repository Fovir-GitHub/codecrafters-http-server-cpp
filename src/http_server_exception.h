#ifndef _HTTP_SERVER_EXCEPTION_H_
#define _HTTP_SERVER_EXCEPTION_H_

#include <exception>
#include <stdexcept>
#include <string>

class HttpServerException : public std::exception
{
private:
    std::string message;

public:
    explicit HttpServerException(const std::string & msg) : message(msg) {}

    const char * what() const noexcept override { return message.c_str(); }
};

#endif // !_HTTP_SERVER_EXCEPTION_H_