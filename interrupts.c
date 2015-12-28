/* 中断服务程序 */

#include "config.h"
#include "motor_pwm.h"
#include "bt_serial.h"

void t0_int()
#ifndef WIN32
interrupt 1
#endif
{
	t0_pwm_interrupt();
}

void serial_int()
#ifndef WIN32
interrupt 4	  //串口中断
#endif
{
	bt_serial_recv_interrupt();
}