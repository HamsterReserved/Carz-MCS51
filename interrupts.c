/* �жϷ������ */

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
interrupt 4	  //�����ж�
#endif
{
	bt_serial_recv_interrupt();
}