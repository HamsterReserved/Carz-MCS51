#ifndef BT_SERIAL_H
#define BT_SERIAL_H

#include "config.h"

void bt_serial_recv_interrupt();
void serial_init();
void send_str(unsigned char* str_send);
void send_data(unsigned char* array_data, unsigned char len);
void serial_process();
#endif
