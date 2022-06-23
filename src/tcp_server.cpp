#include "tcp_server.h"

Tcp::Err Tcp_Server::init(fd *sock_fd)
{
    //Initialize address struct
    memset(&_server_addr, 0, sizeof(_server_addr));

    //Create socket
    if((_sock_fd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
        return Tcp::ERR_CREATE_SOCK;

    //Set socket option reuse address
    const int enable = 1;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        return Tcp::ERR_SET_SOCK_OPT;

    //Create local address
    _server_addr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_ADDR);
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(DEFAULT_SERVER_PORT);

    //Bind local address
    if (bind(_sock_fd, (struct sockaddr*) &_server_addr, sizeof(_server_addr)) < 0)
        return Tcp::ERR_BIND_SOCK;
    
    //Set socket to accept connections
    if (listen(_sock_fd, DEFAULT_SERVER_MAX_CONN) == -1)
        return Tcp::ERR_SOCK_LISTEN;

    //Return file and no error
    *sock_fd = _sock_fd;
    return Tcp::NO_ERR;
}

Tcp::Err Tcp_Server::accept_client(Client **client)
{
    fd client_fd;
    socklen_t sock_size = sizeof(_client_addr);

    //Accept incoming connection
    client_fd = accept(_sock_fd, (struct sockaddr *)&_client_addr, &sock_size);    
    
    //Return error if accept fails
    if (client_fd == -1)
        return Tcp::ERR_ACCEPT;
    
    //Construct new client
    char new_ip[INET_ADDRSTRLEN];  
    inet_ntop(AF_INET, &_client_addr.sin_addr, new_ip, sizeof(new_ip));

    auto new_client = new Client(client_fd);
    new_client->ip = std::string(new_ip);
    new_client->port = htons(_client_addr.sin_port);

    //Append client to clients map
    _clients.insert(std::make_pair(client_fd, new_client));
    
    //Return client and no error
    *client = new_client;
    return Tcp::NO_ERR;
}

Tcp::Err Tcp_Server::recv(fd client_fd, char *buf, size_t size)
{
    //Find client in map from fd
    auto itr = _clients.find(client_fd);

    //Return error if not found
    if (itr == _clients.end())
        return Tcp::ERR_CLIENT_NOT_FOUND;

    //Return error if receive error
    if (itr->second->recv(buf, size) <= 0)
        return Tcp::ERR_RECV;

    //Return no error
    return Tcp::NO_ERR;
}

Tcp::Err Tcp_Server::send_all_clients(const char *buf, size_t size)
{
    //Initialize error code to no error
    Tcp::Err err = Tcp::NO_ERR;

    //Iterate through clients map
    for (const auto client : _clients)
    {
        //If any client send fails, set send error
        if (client.second->send(buf, size) < 0)
            err = Tcp::ERR_SEND;
    }

    //Return error code
    return err;
}

Tcp::Err Tcp_Server::remove_client(fd client_fd)
{
    //find client in map from fd
    auto itr = _clients.find(client_fd); 

    //Return error if not found
    if (itr == _clients.end())
        return Tcp::ERR_CLIENT_NOT_FOUND;

    //Return error if close error
    if (itr->second->close() < 0)
        return Tcp::ERR_CLOSE;

    //deallocate and remove client from list
    delete(itr->second);
    _clients.erase(itr);

    //Return no error
    return Tcp::NO_ERR;
}

Tcp::Err Tcp_Server::remove_all_clients()
{
    int err = 0;

    //Iterate through client map
    for (auto itr = _clients.begin(); itr != _clients.end(); ++itr)
    {
        //deallocate and remove client from list
        err += itr->second->close();
        delete(itr->second);
        _clients.erase(itr);
    }

    //Return error if close fails for one or more clients
    if (err < 0)
        return Tcp::ERR_CLOSE;

    //Return no error
    return Tcp::NO_ERR;
}