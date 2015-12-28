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

/* bt_serial.c */
/* ���ڽ��ջ����� */
extern unsigned char buf[5];
/* ����Ϣ�����־ */
extern bit is_new_msg;

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
		if (is_new_msg == 1)
		{
			is_new_msg = 0;
			if (buf[0] == 'H' && buf[1] == 'S')	//��һ���ֽ�ΪH���ڶ����ֽ�ΪS���������ֽ�Ϊ������
			{
				// ��ʱ��һ��ĵ������Ϊͬһ��PWMֵ
#ifdef DEBUG
				sprintf(msg_buf, "\r\nMSG %x %x\r\n", buf[2], buf[3]);
				send_str(msg_buf);
#endif
				set_motor_state(MOTOR_LEFT_FRONT, buf[2]);
				set_motor_state(MOTOR_LEFT_REAR, buf[2]);
				set_motor_state(MOTOR_RIGHT_FRONT, buf[3]);
				set_motor_state(MOTOR_RIGHT_REAR, buf[3]);
			}
		}
	}
}