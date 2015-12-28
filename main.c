#include <AT89X52.H>
#include "config.h"
#include "motor_pwm.h"
#include "bt_serial.h"
#include "timers_setup.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/*--������--*/
void main(void)
{
#ifdef DEBUG
	char msg_buf[15];
#endif
	t0_setup();
	t1_setup();
	t2_setup(); // ��ʼ���õ��Ķ�ʱ��
	EA = 1;
	P1 = 0; // ������P1=0Ϊʼ̬

	while (1)
	{
		serial_loop();
	}
}