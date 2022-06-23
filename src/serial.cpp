#include "serial.h"

#include <linux/serial.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

void Serial::tty_set_params(int local_echo, unsigned int serial_rate, int enable_rs485)
{
    static const struct {
        unsigned int as_uint; 
        speed_t as_speed;
    } speedTable[] = {
        {   1200,   B1200 },
        {   9600,   B9600 },
        {  19200,  B19200 },
        {  38400,  B38400 },
        {  57600,  B57600 },
        { 115200, B115200 },
        { 230400, B230400 }
    };

    //Assign unix compliant baud rate from speed table
    unsigned i;
    for (i = 0 ; i < (sizeof(speedTable) / sizeof(speedTable[0])) ; i++) {
        if (speedTable[i].as_uint == serial_rate) {
            _tty_rate = speedTable[i].as_speed;
            break;
        }
    }

    _local_echo_flag = local_echo;
    _rs485_mode = enable_rs485;
}

Ser::Err Serial::tty_init(fd *ser_fd, const char *tty_dev)
{
    *ser_fd = -1;

    //Rs485 struct holds config info for rs485 settings
    struct serial_rs485 rs485_conf;

    //Termios struct holds config info for port
    struct termios tio;

    memset(&tio, 0, sizeof(tio));       // Set all bits in termios config to 0
    tio.c_iflag = 0;                    //input processing; parity, flow control, etc.
    tio.c_oflag = 0;                    //output parameters; translation, padding, etc.
    tio.c_cflag = CS8 | CREAD | CLOCAL; //control parameters: 8b/B, can be read from, attached locally
    tio.c_lflag = ICANON;                    //local modes; echoing, signals, etc.
    tio.c_cc[VMIN] = 1;                 //min number of bytes required for *read* to return
    tio.c_cc[VTIME] = 5;                //how long to wait for input before returning (0.1s units)

    //Open file (port) with read/write access and assign file descriptor (fd)
    *ser_fd = open(tty_dev, O_RDWR);
    if (*ser_fd < 0) 
    {
        return Ser::ERR_OPEN_PORT;    
    }
    _ser_fd = *ser_fd; 

    if (_rs485_mode) {
        rs485_conf.flags |= SER_RS485_ENABLED;   //Enable RS-485 mode:
        rs485_conf.delay_rts_before_send = 0;    //Set rts/txen delay before send
        rs485_conf.delay_rts_after_send = 0;     //Set rts/txen delay after send

        //Write the RS-485 config to the open file descriptor with ioctl.
        if (ioctl (*ser_fd, TIOCSRS485, &rs485_conf) < 0) {
            return Ser::ERR_SET_CONFIG;
        }
    }

    cfsetospeed(&tio, _tty_rate); //set output baud rate
    cfsetispeed(&tio, _tty_rate); //set input baud rate

    /* save the termios config to the open file descriptor,
        * TCSAFLUSH flushes IO buffers and applies config */
    if (tcsetattr(*ser_fd, TCSAFLUSH, &tio) < 0) {
        return Ser::ERR_SET_ATTR;
    }

    return Ser::NO_ERR;
}

Ser::Err Serial::read(char *buf, size_t size)
{
    //Return if read error
    size_t read_size = ::read(_ser_fd, buf, size);
    if (read_size <= 0)
        return Ser::ERR_READ;

    //Return no error
    return Ser::NO_ERR; 
}

Ser::Err Serial::write(const char *buf, size_t size)
{
    //Return if error writing message
    if (::write(_ser_fd, buf, size) < 0)
        return Ser::ERR_WRITE;

    //Return if error writing terminator
    if (::write(_ser_fd, "\r\n", 2) < 0)
        return Ser::ERR_WRITE;

    //Return no error
    return Ser::NO_ERR;
}

Ser::Err Serial::close()
{
    //Return if error closing serial
    if (::close(_ser_fd) < 0)
        return Ser::ERR_CLOSE;

    //Return no error
    return Ser::NO_ERR;
}