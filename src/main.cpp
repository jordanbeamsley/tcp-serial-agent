#include <thread>
#include <unistd.h>
#include <atomic>
#include <sys/epoll.h>

#include "tcp_server.h"
#include "serial.h"
#include "error.h"
#include "program_options.h"

#define MAX_EVENTS 10

Epol::Err epol_assign(fd epoll_fd, fd new_fd, epoll_event *ev)
{
    ev->data.fd = new_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, ev) < 0)
    {
        return Epol::ERR_ASSIGN;
    }

    return Epol::NO_ERR;
}

int main(int argc, char *argv[])
{
    Program_Options *prog_opts = new Program_Options();
    prog_opts->load_defaults();
    prog_opts->parse(argc, argv);

    /* ================== Initialize epoll structures ================== */
    int ev_count;
    fd epoll_fd;
    struct epoll_event ev, events[MAX_EVENTS];
    Epol::Err epol_err;

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        fprintf(stderr, "%s\n", Epol::map_error(Epol::ERR_CREATE));
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;

    /* ================== Initialize tcp server ================== */
    fd sock_fd;
    Tcp::Err tcp_err;
    Tcp_Server *server = new Tcp_Server();
    
    tcp_err = server->init(&sock_fd);
    if (tcp_err != Tcp::NO_ERR)
    {
        fprintf(stderr, "%s\n", Tcp::map_error(tcp_err));
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "tcp server created: %s:%d\n", DEFAULT_SERVER_ADDR, DEFAULT_SERVER_PORT);

    epol_err = epol_assign(epoll_fd, sock_fd, &ev);
    if (epol_err != Epol::NO_ERR)
    {
        fprintf(stderr, "%s - '%d'\n", Epol::map_error(epol_err), sock_fd);
        exit(EXIT_FAILURE);
    }

    /* ================== Initialize serial port ================== */
    fd ser_fd;
    Ser::Err ser_err;
    Serial *serial = new Serial();
    
    serial->tty_set_params(DEFAULT_SERIAL_ECHO, DEFAULT_SERIAL_RATE, prog_opts->rs485);
    ser_err = serial->tty_init(&ser_fd, prog_opts->serial_device.c_str());
    if (ser_err != Ser::NO_ERR)
    {
        fprintf(stderr, "%s\n", Ser::map_error(ser_err));
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "serial port opened: %s\n", prog_opts->serial_device.c_str());

    epol_err = epol_assign(epoll_fd, ser_fd, &ev);
    if (epol_err != Epol::NO_ERR)
    {
        fprintf(stderr, "%s - '%d'\n", Epol::map_error(epol_err), sock_fd);
        exit(EXIT_FAILURE);
    }

    /* ================== Main loop ================== */
    char buf[MAX_BUF] = "";
    fd ev_fd;
    int ser_conn_flag = 0;

    while(1)
    {  
        ev_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < ev_count; i++)
        {   
            ev_fd = events[i].data.fd;

            /* ================== Event from server ================== */
            if (ev_fd == sock_fd)
            {
                //Accept client
                Client *client;
                tcp_err = server->accept_client(&client);
                
                if (tcp_err != Tcp::NO_ERR)
                {
                    fprintf(stderr, "%s\n", Tcp::map_error(tcp_err));
                    continue;
                }
                
                fprintf(stdout, "client accepted - '%s:%d'\n", client->ip.c_str(), client->port);

                //Add client to epoll
                fd client_fd = client->client_fd;
                epol_err = epol_assign(epoll_fd, client_fd, &ev);
                if (epol_err != Epol::NO_ERR)
                {
                    fprintf(stderr, "%s - '%d'\n", Epol::map_error(epol_err), client_fd);
                    server->remove_client(client_fd);
                    continue;
                }
            }

            /* ================== Event from serial ================== */
            else if (ev_fd == ser_fd)
            {
                //Check if disconnected
                ser_err = serial->read(buf, MAX_BUF);
                if(ser_err != Ser::NO_ERR)
                {
                    fprintf(stderr, "%s\n", Ser::map_error(ser_err));
                    ser_conn_flag = -1;
                    serial->close();
                    break;
                }

                fprintf(stdout, "received %ld bytes from serial\n", strlen(buf));
                
                //Send serial message to clients
                tcp_err = server->send_all_clients(buf, strlen(buf));
                if (tcp_err != Tcp::NO_ERR)
                {
                    fprintf(stderr, "%s\n", Tcp::map_error(tcp_err));
                }
                memset(buf, 0, strlen(buf));
            }

            /* ================== Event from client ================== */
            else
            {
                tcp_err = server->recv(ev_fd, buf, MAX_BUF);

                //Disconnect client if read error occurred
                if(tcp_err != Tcp::NO_ERR)
                {
                    fprintf(stderr, "%s\n", Tcp::map_error(tcp_err));
                    fprintf(stdout, "disconneting client...\n");
                    server->remove_client(ev_fd);
                    continue;
                }

                fprintf(stdout, "received %ld bytes from client - fd: %d\n", strlen(buf), ev_fd);

                //Send client message to serial
                serial->write(buf, strlen(buf) -1);
                memset(buf, 0, strlen(buf));
            }
        }

        //If serial port is disconnected, exit program
        if (ser_conn_flag < 0)
        {
            break;
        }
    }
    
    /* ================== Close and deaollocate ================== */

    //Free serial
    serial->close();
    delete(serial);

    //Free server
    server->remove_all_clients();
    delete(serial);

    //Free epoll
    close(epoll_fd);

    return 0;
}
