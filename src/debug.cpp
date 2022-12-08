#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "debug.h"

#ifdef __cplusplus
}
#endif


void debug(const char *msg)
{
	SerialUSB.print(msg);
}

void debug_num(const unsigned int msg)
{
	SerialUSB.print(msg);
}
