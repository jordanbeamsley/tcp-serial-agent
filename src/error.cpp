#include "error.h"

const char* Epol::map_error(Err err_code)
{
    switch(err_code)
    {
        case NO_ERR:            return "no error";
        case ERR_CREATE:        return "error creating epoll";
        case ERR_ASSIGN:        return "error epoll set insertion";
    }

    return "Unknown error";
}

const char* Tcp::map_error(Err err_code)
{
    switch(err_code)
    {
        case NO_ERR:                return "no error";
        case ERR_CREATE_SOCK:       return "error creating TCP socket";
        case ERR_SET_SOCK_OPT:      return "error setting TCP socket options";
        case ERR_BIND_SOCK:         return "error binding TCP socket to local address";
        case ERR_SOCK_LISTEN:       return "error setting TCP socket to accept connections";
        case ERR_ACCEPT:            return "error accepting TCP connection";
        case ERR_CLIENT_NOT_FOUND:  return "error TCP client not found in map";
        case ERR_RECV:              return "error receiving from TCP client";
        case ERR_SEND:              return "error sending to one or more TCP clients";
        case ERR_CLOSE:             return "error closing TCP socket";
    }

    return "Unknown error";
}

const char* Ser::map_error(Err err_code)
{
    switch (err_code)
    {
        case NO_ERR:            return "no error";
        case ERR_OPEN_PORT:     return "error opening serial port";
        case ERR_SET_CONFIG:    return "error setting serial RS485 config";
        case ERR_SET_ATTR:      return "error saving serial termios config";
        case ERR_READ:          return "error reading from serial port";        
        case ERR_WRITE:         return "error writing to serial port";
        case ERR_CLOSE:         return "error closing serial port";
    }

    return "Unknown error";
}