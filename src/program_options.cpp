#include "program_options.h"

#include <stdexcept>
#include <getopt.h>
#include "defs.h"

#define SHORT_OPTS "d:hvr"

void Program_Options::load_defaults()
{
    verbose = DEFAULT_VERBOSE;
    rs485 = DEFAULT_SERIAL_RS485;
    serial_device = DEFAULT_SERIAL_DEVICE;
}

void Program_Options::parse(int argc, char* argv[])
{
    int iarg;
    int opt_index = 0;

    static struct option long_opts[] = 
    {
        {"help",            no_argument,        0, 'h'},
        {"verbose",         no_argument,        0, 'v'},
        {"rs485",           no_argument,        0, 'r'},
        {"serial-device",   required_argument,  0, 'd'},
        {0,0,0,0}
    };

    while (1)
    {
        iarg = getopt_long(argc, argv, SHORT_OPTS, long_opts, &opt_index);

        if (iarg == -1)
        {
            break;
        }

        switch(iarg)
        {
            case 'h':
                usage();
                exit(0);

            case 'v':
                verbose = true;
                break;

            case 'd':
                serial_device = string_arg();
                break;

            case 'r':
                rs485 = 1;
                break;

            default:
                usage();
                exit(1);
        }
    }
}

int Program_Options::int_arg(const char *s)
{
    char *p;
    int num = strtol(s, &p, 10);

    if (*p != 0)
    {
        usage();
        exit(1);
    }

    return num;
}

std::string Program_Options::string_arg()
{
    //argument not supplied
    if (!optarg)
    {
        usage();
        exit(1);
    }

    return std::string(optarg);
}

void Program_Options::usage()
{
    printf("Usage: sio-bridge [OPTION]... \n\n");

    printf(
        "  -h, --help               display this help and exit\n"
        "  -v, --verbose            enable verbose output\n"
        "  -d, --serial-device      set serial device port\n"
        "  -r, --rs485              enable rs485 output mode\n"
    );   
}