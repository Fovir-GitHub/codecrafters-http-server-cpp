#ifndef _MESSAGE_H_
#define _MESSAGE_H_

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

        void ParsePath();

    public:
        Request(const std::string & original_request);
    };

    class Response
    {
    };
};

END_MESSAGE_NAMESPACE

#endif // !_MESSAGE_H_