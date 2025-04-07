#include "server.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

namespace fs = std::filesystem;

/**
 *@brief Check whether the file exists in current directory
 *
 * @param file_name the name of the file to check
 * @param full_path whether the file name is the full path of the file
 * @return true the file exists
 * @return false the file does not exist
 */
static bool existFile(const std::string & file_name, bool full_path = false)
{
    return !full_path
               ? (fs::exists(fs::current_path().string() + "/" + file_name) ||
                  file_name == "/" || file_name.empty())
               : (fs::exists(file_name));
}

static void terminateProgram()
{
    std::exit(1);
}

int server::Server::InitializeSocket()
{
    try
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0); /* Set server_fd */
        if (server_fd < 0) /* Failed to create socket */
            throw server::ServerException("Failed to create server socket");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    // Allow to reuse the port
    int reuse = 1;
    try
    {
        // If failed to set the socket option
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                       sizeof(reuse)) < 0)
            throw server::ServerException("setsockopt failed");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    // Bind socket to the address
    sockaddr_in server_address;
    server_address.sin_family      = AF_INET;     /* Use IPV4 */
    server_address.sin_addr.s_addr = INADDR_ANY;  /* Use 0.0.0.0 */
    server_address.sin_port        = htons(PORT); /* Set port */

    try
    {
        // Fail to bind the port
        if (bind(server_fd, (sockaddr *) &server_address,
                 sizeof(server_address)) != 0)
            throw server::ServerException("Failed to bind to port 4221");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    return server_fd;
}

void server::Server::Listen()
{
    // The maximum number of connection
    int connection_backlog = 500;

    try
    {
        // Fail to listen
        if (listen(server_fd, connection_backlog) != 0)
            throw server::ServerException("listen failed");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        terminateProgram();
    }

    return;
}

int server::Server::AcceptClient()
{
    // Set the client
    sockaddr_in client_address;
    int         client_address_length = sizeof(client_address);

    std::cout << "Waiting for a client to connect...\n";

    // Accept the connection from client
    int client_fd = accept(server_fd, (sockaddr *) &client_address,
                           (socklen_t *) &client_address_length);

    // Return the client_fd
    return client_fd;
}

std::string server::Server::Receive(int client_fd)
{
    // Received data
    std::string received(BUFFER_LENGTH, '\0');
    ssize_t     receive_bytes; /* Received bytes */

    try
    {
        // Fail to receive data
        if ((receive_bytes =
                 recv(client_fd, &received[0], received.size(), 0)) < 0)
            throw server::ServerException("receive failed");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
    }

    // Resize the received data
    received.resize(receive_bytes);

    /*
    If the receive_bytes is 0, it means the connection is closed.
    By returning empty string, the main program can handle the situation.
    */
    return receive_bytes == 0 ? "" : received;
}

void server::Server::Send(int client_fd, const std::string & message)
{
    try
    {
        if (send(client_fd, message.c_str(), message.size(), 0) < 0)
            throw server::ServerException("send failed");
    }
    catch (const ServerException & e)
    {
        std::cerr << e.what() << '\n';
    }

    return;
}

void server::Server::HandleClient(int client_fd)
{
    std::string received_message;

    while (true)
    {
        // If the received message is empty, then the connection is closed
        if ((received_message = this->Receive(client_fd)) == "")
            break;

        http_message.SetRequest(received_message);

        // If the method is POST
        if (http_message.GetRequestPointer()->GetHttpMethod() == "POST")
            this->HandlePOSTMethod(
                client_fd, http_message.GetRequestPointer()->GetParsedPath());
        else /* By default, handle GET method */
            this->HandleGETMethod(client_fd);
    }

    std::cout << "Connection closed\n";

    return;
}

void server::Server::SetResponse()
{
    // Clear the response before setting
    http_message.GetResponsePointer()->Clear();

    const std::vector<std::string> & request_path =
        http_message.GetRequestPointer()->GetParsedPath();

    if (request_path.at(0) == "echo")
        HandleEcho(request_path);
    else if (request_path.at(0) == "user-agent")
        HandleUserAgent();
    else if (request_path.at(0) == "files")
        HandleFile(request_path);
    else
        HandleDefault();

    http_message.GetResponsePointer()->MakeResponse();

    return;
}

void server::Server::HandleEcho(const std::vector<std::string> & request_path)
{
    try
    {
        http_message.GetResponsePointer()->SetBody(request_path.at(1));
    }
    catch (const std::out_of_range & e)
    {
        std::cout << e.what() << '\n';
    }

    http_message.GetResponsePointer()->SetHeaderLine("Content-Type",
                                                     "text/plain");

    return;
}

void server::Server::HandleUserAgent()
{
    http_message.GetResponsePointer()->SetBody(
        http_message.GetRequestPointer()->GetHeaderLines("User-Agent"));

    http_message.GetResponsePointer()->SetHeaderLine("Content-Type",
                                                     "text/plain");

    return;
}

void server::Server::HandleFile(const std::vector<std::string> & request_path)
{
    // If the file exists
    if (existFile(request_path.at(1)))
    {
        http_message.GetResponsePointer()->SetStatusCode(200);
        http_message.GetResponsePointer()->SetHeaderLine(
            "Content-Type", "application/octet-stream");

        std::ifstream      fin;
        std::ostringstream oss;
        char               ch;

        try
        {
            // Open the file
            fin.open(fs::current_path().string() + "/" + request_path.at(1),
                     std::ios::binary);

            // If open failed
            if (fin.fail())
                throw server::ServerException("fail to open file");
        }
        catch (const server::ServerException & e)
        {
            std::cerr << e.what() << '\n';
        }

        // Read the file content
        while (fin.get(ch)) oss << ch;

        fin.close(); /* Close the file */
        http_message.GetResponsePointer()->SetBody(oss.str());
    }
    else
        http_message.GetResponsePointer()->SetStatusCode(404);

    return;
}

void server::Server::HandleDefault()
{
    http_message.GetResponsePointer()->SetStatusCode(
        existFile(http_message.GetRequestPointer()->GetFullPath(), true) ? 200
                                                                         : 404);

    return;
}

void server::Server::HandleGETMethod(int client_fd)
{
    this->SetResponse();
    this->Send(client_fd, http_message.GetResponsePointer()->GetResponse());
}

void server::Server::HandlePOSTMethod(
    int client_fd, const std::vector<std::string> & request_path)
{
    std::ofstream fout;

    try
    {
        fout.open(fs::current_path().string() + "/" + request_path.at(1),
                  std::ios::binary);

        // Fail to open the file
        if (fout.fail())
            throw server::ServerException("fail to write file");
    }
    catch (const server::ServerException & e)
    {
        std::cerr << e.what() << '\n';
        return;
    }

    // Write the file and close the file
    fout << http_message.GetRequestPointer()->GetBody();
    fout.close();

    // Set the response
    http_message.GetResponsePointer()->SetStatusCode(201);

    // Make the response and send it
    http_message.GetResponsePointer()->MakeResponse();
    this->Send(client_fd, http_message.GetResponsePointer()->GetResponse());

    return;
}
