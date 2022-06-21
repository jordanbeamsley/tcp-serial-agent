#include <thread>
#include <unistd.h>
#include <atomic>
#include <sys/epoll.h>

#include "tcp_server.h"
#include "serial.h"

#define MAX_EVENTS 10

int main(int argc, char const *argv[])
{
    //Initialize epoll structures
    int ev_count;
    fd nfds, epoll_fd;
    struct epoll_event ev, events[MAX_EVENTS];

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    //Initialize tcp server
    Tcp_Server *server = new Tcp_Server();
    fd sock_fd = server->init();

    ev.events = EPOLLIN;
    ev.data.fd = sock_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) < 0)
    {
        fprintf(stderr, "error assigning socked fd\n");
        return 1;
    }

    //Initialize serial port
    Serial *serial = new Serial();
    fd ser_fd = serial->init();

    printf("%d\n", ser_fd);

    ev.data.fd = ser_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ser_fd, &ev) < 0)
    {
        fprintf(stderr, "error assigning serial fd\n");
        return 1;
    }

    char read_buffer[MAX_SER_BUF] = "";
    size_t bytes_read;
    fd ev_fd;

    //Main loop
    while(1)
    {   
        ev_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        printf("%d ready events\n", ev_count);
        for (int i = 0; i < ev_count; i++)
        {   
            ev_fd = events[i].data.fd;

            //New connection to server
            if (ev_fd == sock_fd)
            {
                fd client_fd = server->accept_clients();
                printf("client accepted - '%d'\n", client_fd);

                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
                {
                    fprintf(stderr, "error epoll set insertion: '%d\n'", client_fd);
                }
            }

            else if (ev_fd == ser_fd)
            {
                serial->read(read_buffer);
                printf("recv from '%d' - %s\n", ev_fd, read_buffer);
                server->send_all_clients(read_buffer, sizeof(read_buffer));
                memset(read_buffer, 0, MAX_SER_BUF);
            }

            //Message from client
            else
            {
                read(ev_fd, read_buffer, 10);
                printf("recv from '%d' - %s\n", ev_fd, read_buffer);
                memset(read_buffer, 0, MAX_SER_BUF);
            }
        }
    }
    

    if (close(epoll_fd))
    {
        perror("close epoll");
    }

    return 0;
}
