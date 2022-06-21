#if !defined(SERIAL_H)
#define SERIAL_H

#include <fcntl.h>
#include <termios.h>

#include <defs.h>

class Serial
{
private:
    void tty_set_params(int local_echo, unsigned int serial_rate, int enable_rs485);
    fd tty_init(const char *tty_dev);

    speed_t _tty_rate;
    int _local_echo_flag;
    int _rs485_mode;
    fd _ser_fd;

public:
    fd init();
    int write(const char *buf);
    int read(char *buf);
    void close();
};


#endif // SERIAL_H
