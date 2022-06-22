#include "tcp_server.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 3333
#define MAX_CONN 10 

Tcp::Err Tcp_Server::init(fd *sock_fd)
{
    memset(&_server_addr, 0, sizeof(_server_addr));

    if((_sock_fd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
        return Tcp::ERR_CREATE_SOCK;

    const int enable = 1;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        return Tcp::ERR_SET_SOCK_OPT;

    _server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(SERVER_PORT);

    if (bind(_sock_fd, (struct sockaddr*) &_server_addr, sizeof(_server_addr)) < 0)
        return Tcp::ERR_BIND_SOCK;
    
    if (listen(_sock_fd, MAX_CONN) == -1)
        return Tcp::ERR_SOCK_LISTEN;

    *sock_fd = _sock_fd;
    return Tcp::NO_ERR;
}

Tcp::Err Tcp_Server::accept_client(Client **client)
{
    fd client_fd;
    socklen_t sock_size = sizeof(_client_addr);

    client_fd = accept(_sock_fd, (struct sockaddr *)&_client_addr, &sock_size);    
    if (client_fd == -1)
    {
        return Tcp::ERR_ACCEPT;
    }
    
    //If no errors, construct new client and append to list
    char new_ip[INET_ADDRSTRLEN];  
    inet_ntop(AF_INET, &_client_addr.sin_addr, new_ip, sizeof(new_ip));

    auto new_client = new Client(client_fd);
    new_client->ip = std::string(new_ip);
    new_client->port = htons(_client_addr.sin_port);

    _clients.insert(std::make_pair(client_fd, new_client));
    
    *client = new_client;
    return Tcp::NO_ERR;
}

Tcp::Err Tcp_Server::recv(fd client_fd, char *buf, size_t size)
{
    auto itr = _clients.find(client_fd);

    if (itr == _clients.end())
    {
        return Tcp::ERR_CLIENT_NOT_FOUND;
    }

    if (itr->second->recv(buf, size) <= 0)
    {
        return Tcp::ERR_RECV;
    }

    return Tcp::NO_ERR;
}

Tcp::Err Tcp_Server::send_all_clients(const char *buf, size_t size)
{
    Tcp::Err err = Tcp::NO_ERR;

    for (const auto client : _clients)
    {
        if (client.second->send(buf, size) < 0)
        {
            err = Tcp::ERR_SEND;
        }
    }

    return err;
}

void Tcp_Server::remove_client(fd client_fd)
{
    auto itr = _clients.find(client_fd); 

    if (itr != _clients.end())
    {
        itr->second->close();
        delete(itr->second);
        _clients.erase(itr);
    }
}