#if !defined(ERROR_H)
#define ERROR_H

#include <string>

namespace Epol
{
    enum Err {
        NO_ERR,
        ERR_CREATE,
        ERR_ASSIGN
    };

    const char* map_error(Err Err_code);
} // namespace EPOL


namespace Tcp
{
    enum Err {
        NO_ERR,
        ERR_CREATE_SOCK,
        ERR_SET_SOCK_OPT,
        ERR_BIND_SOCK,
        ERR_SOCK_LISTEN,
        ERR_ACCEPT,
        ERR_CLIENT_NOT_FOUND,
        ERR_RECV,
        ERR_SEND,
    };

    const char* map_error(Err err_code);
} // namespace TCP

namespace Ser
{
    enum Err {
        NO_ERR,
        ERR_OPEN_PORT,
        ERR_SET_CONFIG,
        ERR_SET_ATTR,
        ERR_READ,
        ERR_WRITE,
        ERR_CLOSE,
    };

    const char* map_error(Err err_code);
} // namespace SER


#endif // ERROR_H
