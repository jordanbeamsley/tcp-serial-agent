#include "tcp_server.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 3333
#define MAX_CONN 10 

Tcp_Server::Tcp_Server()
{

}

int Tcp_Server::init()
{
    memset(&_server_addr, 0, sizeof(_server_addr));

    if((_sock_fd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
        return -1;

    const int enable = 1;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        return -2;

    _server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(SERVER_PORT);

    if (bind(_sock_fd, (struct sockaddr*) &_server_addr, sizeof(_server_addr)) < 0)
        return -3;
    
    if (listen(_sock_fd, MAX_CONN) == -1)
        return -4;

    return _sock_fd;
}

fd Tcp_Server::accept_clients(/*std::string *ip*/)
{
    fd client_fd;
    socklen_t sock_size = sizeof(_client_addr);

    //Wait for new client request
    client_fd = accept(_sock_fd, (struct sockaddr *)&_client_addr, &sock_size);
    
    //Error connecting, skip to next loop iteration
    if (client_fd == -1)
    {
        return -1;
    }
    
    //If no errors, construct new client and append to list
    char new_ip[INET_ADDRSTRLEN];  
    inet_ntop(AF_INET, &_client_addr.sin_addr, new_ip, sizeof(new_ip));

    auto new_client = new Client(client_fd);
    new_client->ip = std::string(new_ip);

    std::lock_guard<std::mutex> lock(_clients_mtx);
    _clients.push_back(new_client);

    return client_fd;
}

int Tcp_Server::send_all_clients(const char *msg, size_t size)
{
    std::lock_guard<std::mutex> lock(_clients_mtx);

    for (Client *client :_clients)
    {
        client->send(msg, size);
    }

    return 1;
}