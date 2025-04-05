#ifndef _HTTP_MESSAGE_H_
#define _HTTP_MESSAGE_H_

#include <string>
#include <unordered_map>
#include <vector>

const std::unordered_map<std::string, int> HTTP_METHODS = {
    {"GET", 0},
    {"POST", 1},
};

const std::unordered_map<int, std::string> HTTP_CODE_PHRASE = {
    {200, "OK"},
    {404, "Not Found"},
};

class HttpMessage
{
private:
    struct RequestStatusLine
    {
        int         method;
        std::string path;
        std::string http_version;

        RequestStatusLine() {}
        RequestStatusLine(const std::string & rsl);
    } request_status_line;

    struct ResponseStatusLine
    {
        std::string http_version;
        int         status_code;
        std::string option_phrase;

        ResponseStatusLine(const std::string & hv = "1.1", int sc = 200,
                           const std::string & op = "OK")
            : http_version(hv)
            , status_code(sc)
            , option_phrase(op)
        {
        }
    } response_status_line;

    std::unordered_map<std::string, std::string> header_lines;
    std::string                                  body;
    std::vector<std::string>                     parsed_path;

    std::string                    TrimInvisibleCharacters(std::string s);
    const std::vector<std::string> ParseRequestPath();

public:
    HttpMessage() {}
    HttpMessage(const std::string & buffer);

    void ParseRequest(const std::string & buffer);

    void SetResponseStatusLine(const std::string & hv = "1.1");

    void ClearHeader() { header_lines.clear(); }

    void SetHeaderLine(const std::string & key, const std::string & value);
    void SetBody(const std::string & b) { body = b; }
    void SetBody();

    std::string MakeResponseStatusLine();
    std::string MakeResponse();
    void        HandleEcho();

    int  GetContentLength() const { return body.size(); }
    void SetContentLength();

    void Show();
};

#endif // !_HTTP_MESSAGE_H_