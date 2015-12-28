#ifndef UTIL_H
#define UTIL_H

#include "config.h"

void delay(unsigned int k);
#ifdef DEBUG
void send_str(unsigned char* str_send);
#endif

#endif // !UTIL_H
