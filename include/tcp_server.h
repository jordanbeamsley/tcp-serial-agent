#if !defined(TCP_SERVER_H)
#define TCP_SERVER_H

#include <sys/select.h>
#include <map>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

#include "defs.h"
#include "error.h"

struct Client
{
    fd client_fd;
    std::string ip;
    uint16_t port;

    Client(fd new_fd) {client_fd = new_fd;}

    size_t send(const char *buf, size_t size) const
    {
        return ::send(client_fd, buf, size, 0);
    }

    size_t recv(char *buf, size_t size) const
    {
        return ::recv(client_fd, buf, size, 0);
    }

    void close() const
    {
        ::close(client_fd);
    }
};

class Tcp_Server
{
private:
    fd_set _rfds;
    fd _sock_fd;
    struct sockaddr_in _server_addr, _client_addr;
    
    std::map<fd, Client*> _clients;

public:
    Tcp::Err init(fd*);
    Tcp::Err send_all_clients(const char*, size_t);
    Tcp::Err accept_client(Client**);
    Tcp::Err recv(fd, char*, size_t);
    void remove_client(fd);
};

#endif // TCP_SERVER_H