#include "http_message.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <locale>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

std::string HttpMessage::TrimInvisibleCharacters(std::string s)
{
    return std::string(s.begin(),
                       std::find_if_not(s.begin(), s.end(), ::isgraph));
}

const std::vector<std::string> HttpMessage::ParseRequestPath()
{
    std::vector<std::string> result;
    std::string              copy_path = request_status_line.path;
    copy_path.erase(0, copy_path.find_first_not_of('/'));

    std::istringstream iss(copy_path);
    std::string        single_path;

    while (std::getline(iss, single_path, '/')) result.push_back(single_path);

    return result;
}

HttpMessage::HttpMessage(const std::string & buffer)
{
    ParseRequest(buffer);
}

void HttpMessage::ParseRequest(const std::string & buffer)
{
    std::istringstream iss(buffer);
    std::string        temp;

    std::getline(iss, temp);
    request_status_line = RequestStatusLine(temp);

    while (std::getline(iss, temp) && temp != "\r")
    {
        size_t colon_position                        = temp.find(':');
        header_lines[temp.substr(0, colon_position)] = TrimInvisibleCharacters(
            temp.substr(colon_position + 1, temp.size()));
    }

    body = std::string(std::istreambuf_iterator<char>(iss),
                       std::istreambuf_iterator<char>());
    return;
}

void HttpMessage::SetResponseStatusLine(const std::string & hv)
{
    int status_code = 200;

    if (!fs::exists(fs::current_path().string() + request_status_line.path))
        status_code = 404;

    response_status_line =
        ResponseStatusLine(hv, status_code, HTTP_CODE_PHRASE.at(status_code));

    return;
}

void HttpMessage::SetHeaderLine(const std::string & key,
                                const std::string & value)
{
    header_lines[key] = value;

    return;
}

void HttpMessage::SetBody()
{
    std::vector<std::string> parsed_path = ParseRequestPath();
    parsed_path.push_back("");

    if (parsed_path[0] == "echo")
        SetBody(parsed_path[1]);
    else if (parsed_path[0] == "User-Agent")
        SetBody(header_lines["User-Agent"]);

    return;
}

std::string HttpMessage::MakeResponseStatusLine()
{
    return "HTTP/" + response_status_line.http_version + " " +
           std::to_string(response_status_line.status_code) + " " +
           response_status_line.option_phrase + "\r\n";
}

std::string HttpMessage::MakeResponse()
{
    SetResponseStatusLine();
    std::string response = MakeResponseStatusLine();

    SetContentLength();

    for (const auto & [key, value] : header_lines)
        response.append(key + ": " + value + "\r\n");

    response.append("\r\n");
    response.append(body);

    return response;
}

void HttpMessage::SetContentLength()
{
    SetHeaderLine("Content-Length", std::to_string(body.size()));
}

void HttpMessage::Show()
{
}

HttpMessage::RequestStatusLine::RequestStatusLine(const std::string & rsl)
{
    std::istringstream iss(rsl);
    std::string        temp;

    iss >> temp;
    this->method = HTTP_METHODS.at(temp);

    iss >> temp;
    this->path = temp;

    iss >> temp;
    this->http_version = std::string(
        std::find_if(temp.begin(), temp.end(), ::isdigit), temp.end());
}
