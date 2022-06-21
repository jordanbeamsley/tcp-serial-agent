#if !defined(TCP_SERVER_H)
#define TCP_SERVER_H

#include <sys/select.h>
#include <mutex>
#include <vector>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string>

#include "defs.h"

class Tcp_Server
{

struct Client
{
    fd sock_fd;
    std::string ip;

    Client(fd new_fd) {sock_fd = new_fd;}

    size_t send(const char *msg, size_t size) const
    {
        const size_t num_sent = ::send(sock_fd, msg, size, 0);
        return num_sent;
    }
};

private:
    fd_set _rfds;
    fd _sock_fd;
    struct sockaddr_in _server_addr, _client_addr;
    
    std::mutex _clients_mtx;
    std::vector<Client*> _clients;

public:
    Tcp_Server(/* args */);
    ~Tcp_Server();
    int init();
    int send_all_clients(const char *msg, size_t size);
    fd accept_clients(/*std::string *ip*/);
};

#endif // TCP_SERVER_H