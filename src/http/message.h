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
        Request() {}

        const std::vector<std::string> & GetParsedPath() const
        {
            return parsed_path;
        }

        const std::unordered_map<std::string, std::string> &
        GetHeaderLines() const
        {
            return header_lines;
        }

        /**
         *@brief Get the header line by given key
         *
         * @param key the key of the header line
         * @return const std::string& the value of the header line
         */
        const std::string & GetHeaderLines(const std::string & key) const;

        /**
         *@brief Get the request body
         *
         * @return const std::string& the request body
         */
        const std::string & GetBody() const { return body; }

        /**
         *@brief Get the original path of the request
         *
         * @return const std::string& the original path
         */
        const std::string & GetOriginalPath() const { return status_line.path; }

        /**
         *@brief Get the absolute path of the request path
         *
         * @return const std::string the full path of the file
         */
        const std::string GetFullPath() const;

        /**
         * @brief Get the http method from the status line
         *
         * @return const std::string& the method
         */
        const std::string & GetHttpMethod() const { return status_line.method; }

        /**
         * @brief Get the types of compression
         *
         * @return const std::vector<std::string> the types of compressions
         */
        const std::vector<std::string> GetCompressionOptions();
    };

    class Response
    {
    private:
        const static std::unordered_map<int, std::string> HTTP_STATUS_CODE;

        struct StatusLine
        {
            int         status_code  = 200;
            std::string http_version = "1.1";
        } status_line;

        std::string                                  response;
        std::unordered_map<std::string, std::string> header_lines;
        std::string                                  body;

    public:
        Response() {}

        /**
         * @brief Set one header line with key-value pair
         *
         * @param key the name of the header
         * @param value the value of the header
         */
        void SetHeaderLine(const std::string & key, const std::string & value);

        // Some `Set-` method
        void SetBody(const std::string & b) { body = b; }
        void SetStatusCode(const int sc) { status_line.status_code = sc; }
        void SetHttpVersion(const std::string & hv)
        {
            status_line.http_version = hv;
        }

        /**
         *@brief Clear the header lines
         */
        void ClearHeaderLine() { header_lines.clear(); }

        /**
         *@brief Clear the response body
         */
        void ClearBody() { body.clear(); }

        /**
         *@brief Clear the response
         */
        void Clear()
        {
            ClearHeaderLine();
            ClearBody();
        }

        /**
         *@brief Construct `response` member value
         */
        void MakeResponse();

        const std::string & GetResponse() const { return response; }

        /**
         *@brief Get the response body
         *
         * @return std::string the body
         */
        std::string GetBody() const { return body; }
    };

    std::unique_ptr<Request>  request;
    std::unique_ptr<Response> response;

public:
    Message(const std::string & msg)
        : request(std::make_unique<Request>(msg))
        , response(std::make_unique<Response>())
    {
    }

    Message()
        : request(std::make_unique<Request>())
        , response(std::make_unique<Response>())
    {
    }

    void SetRequest(const std::string & msg)
    {
        request = std::make_unique<Request>(msg);
    }

    const std::unique_ptr<Request> & GetRequestPointer() const
    {
        return request;
    }

    std::unique_ptr<Response> & GetResponsePointer() { return response; }
};

END_MESSAGE_NAMESPACE

#endif // !_MESSAGE_H_