

#include "hyTypes.h"
#include "socket_api.h"



int gettimeofday (struct timeval *tv, void *tz)
{
  unsigned  timemillis = rawtime(NULL);
  
  tv->tv_sec  = timemillis/100;
  tv->tv_usec = (timemillis - (tv->tv_sec*100)) * 1000*10;
  
  return 0;
}

