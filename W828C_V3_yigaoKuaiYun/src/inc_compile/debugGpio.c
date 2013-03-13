#include "debugGpio.h"

void debug_io_initial(int port, int bit)
{
     hyhwGpio_setAsGpio(port,bit);
     hyhwGpio_setOut(port,bit);
     hyhwGpio_setLow(port,bit);
}

void debug_io_set_high(int port, int bit)
{
    hyhwGpio_setHigh(port,bit);
}

void debug_io_set_low(int port, int bit)
{
     hyhwGpio_setLow(port,bit);
}
