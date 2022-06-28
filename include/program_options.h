#if !defined(PROGRAM_OPTIONS_H)
#define PROGRAM_OPTIONS_H

#include <map>
#include <string>

class Program_Options
{
private:
    void usage();
    int int_arg(const char *s);
    std::string string_arg();

public:
    void load_defaults();
    void parse(int argc, char* argv[]);

    bool verbose;
    std::string serial_device;
    int rs485; 
};

#endif // PROGRAM_OPTIONS_H
