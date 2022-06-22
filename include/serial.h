#if !defined(SERIAL_H)
#define SERIAL_H

#include <fcntl.h>
#include <termios.h>

#include <defs.h>
#include "error.h"

class Serial
{
private:
    speed_t _tty_rate;
    int _local_echo_flag;
    int _rs485_mode;
    fd _ser_fd;

public:
    void tty_set_params(int, unsigned int, int);
    Ser::Err tty_init(fd *, const char *);

    Ser::Err write(const char *buf, size_t size);
    Ser::Err read(char *buf, size_t size);
    Ser::Err close();
};


#endif // SERIAL_H
