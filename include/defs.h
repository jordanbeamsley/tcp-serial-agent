#if !defined(DEFS_H)
#define DEFS_H

//Global
typedef int fd;
#define MAX_BUF 100

//TCP
#define DEFAULT_SERVER_ADDR "127.0.0.1"
#define DEFAULT_SERVER_PORT 3333
#define DEFAULT_SERVER_MAX_CONN 10 
//Serial
#define DEFAULT_SERIAL_RATE 9600
#define DEFAULT_SERIAL_DEVICE "/dev/ttyUSB0"
#define DEFAULT_SERIAL_ECHO 0
#define DEFAULT_SERIAL_RS485 0

#endif // DEFS_H