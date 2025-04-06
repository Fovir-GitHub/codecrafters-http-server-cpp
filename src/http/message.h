#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#define BEGIN_MESSAGE_NAMESPACE \
    namespace message           \
    {
#define END_MESSAGE_NAMESPACE }

BEGIN_MESSAGE_NAMESPACE

class Message
{
private:
    class Request
    {
    private:
        struct StatusLine
        {
            std::string method;
            std::string path;
            std::string http_version;

            StatusLine() {}
        } status_line;

        std::vector<std::string>                     parsed_path;
        std::unordered_map<std::string, std::string> header_lines;
        std::string                                  body;

        /**
         *@brief Remove invisible characters from the begin and the end of the
         *string
         *
         * @param s the original string
         * @return std::string the string after trim
         */
        std::string TrimInvisibleCharacters(const std::string & s);

        /**
         * @brief Parse the request path
         */
        void ParsePath();

    public:
        Request(const std::string & original_request);
    };

    class Response
    {
    private:
        const static std::unordered_map<int, std::string> HTTP_STATUS_CODE;

        struct StatusLine
        {
            int         status_code;
            std::string http_version;

            StatusLine(int sc = 200, const std::string & hv = "1.1")
                : status_code(sc)
                , http_version(hv)
            {
            }
        } status_line;

        std::string                                  response;
        std::unordered_map<std::string, std::string> header_lines;

    public:
        Response(int st);

        /**
         * @brief Set one header line with key-value pair
         *
         * @param key the name of the header
         * @param value the value of the header
         */
        void SetHeaderLine(const std::string & key, const std::string & value);

        /**
         *@brief Clear the header lines
         */
        void ClearHeaderLine() { header_lines.clear(); }
    };

    std::unique_ptr<Request> request;

public:
    Message(const std::string & msg) : request(std::make_unique<Request>(msg))
    {
    }
};

END_MESSAGE_NAMESPACE

#endif // !_MESSAGE_H_