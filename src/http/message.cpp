#include "message.h"
#include <algorithm>
#include <cctype>
#include <sstream>

std::string
message::Message::Request::TrimInvisibleCharacters(const std::string & s)
{
    return std::string(
        std::find_if_not(s.begin(), s.end(), ::isspace),
        std::find_if_not(s.rbegin(), s.rend(), ::isspace).base());
}

void message::Message::Request::ParsePath()
{
    std::string copy_path = status_line.path;

    copy_path.erase(0, copy_path.find_first_not_of('/'));

    std::istringstream iss(copy_path);
    std::string        single_path;

    while (std::getline(iss, single_path, '/'))
        parsed_path.push_back(single_path);

    parsed_path.push_back("");

    return;
}

message::Message::Request::Request(const std::string & original_request)
{
    std::string        temp;
    std::istringstream iss(original_request);

    iss >> temp; /* Get the http method */
    status_line.method = temp;

    iss >> temp; /* Get the http path */
    status_line.path = temp;

    iss >> temp; /* Get the http version */
    status_line.http_version = std::string(
        std::find_if(temp.begin(), temp.end(), ::isdigit), temp.end());

    // Ignore empty lines
    while (std::getline(iss, temp) && temp == "\r");

    // Get headers
    while (std::getline(iss, temp) && temp != "\r")
    {
        size_t colon_position = temp.find(':'); /* Find colon's position */
        header_lines[TrimInvisibleCharacters(temp.substr(0, colon_position))] =
            TrimInvisibleCharacters(
                temp.substr(colon_position + 1, temp.size()));
    }

    // The remain part is body
    body = std::string(std::istreambuf_iterator<char>(iss),
                       std::istreambuf_iterator<char>());

    ParsePath(); /* Parse the request path */
}