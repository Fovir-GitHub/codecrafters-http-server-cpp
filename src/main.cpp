#include "server/server.h"
#include <filesystem>
#include <string>
#include <thread>

namespace fs = std::filesystem;

int main(int argc, char ** argv)
{
    if (argc == 3 && std::string(argv[1]) == "--directory")
        fs::current_path(argv[2]);

    server::Server http_server(4221);

    http_server.InitializeSocket();
    http_server.Listen();

    while (true)
    {
        int client_fd = http_server.AcceptClient();

        std::thread([&]() { http_server.HandleClient(client_fd); }).detach();
    }

    return 0;
}
