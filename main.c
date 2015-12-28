/****************************************************************************
 Ӳ������
 ������4���߶��壺
  VCC	 5V
  GND	 GND
  TDX	 TX
  RXD	 RD
 ���������뵥Ƭ��J2�ӿ���
****************************************************************************/

#include <AT89X52.H>
#include "config.h"
#include "motor_pwm.h"
#include "bt_serial.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/*--������--*/
void main(void)
{
#ifdef DEBUG
	char msg_buf[15];
#endif
	serial_init();
	pwm_init();
	EA = 1;
	P1 = 0; // ������P1=0Ϊʼ̬

	while (1)
	{
		serial_loop();
	}
}