#include "message.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

const std::unordered_map<int, std::string>
    message::Message::Response::HTTP_STATUS_CODE = {
        {200, "OK"},
        {404, "Not Found"},
};

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

const std::string &
message::Message::Request::GetHeaderLines(const std::string & key) const
{
    static const std::string empty_string;

    try
    {
        return header_lines.at(key);
    }
    catch (const std::out_of_range &)
    {
        return empty_string;
    }
}

const std::string message::Message::Request::GetFullPath() const
{
    return fs::current_path().string() + status_line.path;
}

void message::Message::Response::SetHeaderLine(const std::string & key,
                                               const std::string & value)
{
    header_lines[key] = value;
}

void message::Message::Response::MakeResponse()
{
    // Set the status line
    response = "HTTP/" + status_line.http_version + " " +
               std::to_string(status_line.status_code) + " " +
               HTTP_STATUS_CODE.at(status_line.status_code) + "\r\n";

    this->SetHeaderLine("Content-Length", std::to_string(body.size()));
    for (const auto & [key, value] : header_lines)
        response.append(key + ": " + value + "\r\n");

    response.append("\r\n");
    response.append(body);
}
