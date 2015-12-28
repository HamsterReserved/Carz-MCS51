#include <AT89X52.h>
#include "config.h"
#include "motor_pwm.h"

/* ���ڽ��ջ����� */
unsigned char buf[5];

/* ����Ϣ�����־ */
bit is_new_msg = 0;

void serial_int()
#ifndef WIN32
interrupt 4	  //�����ж�
#endif
{
	unsigned char recv_buf = 0;
	static unsigned char bytes_received = 0; // ��0�����ѿ�ʼ����

	if (RI) //�Ƿ�����ж�
	{
		RI = 0;
		recv_buf = SBUF;
		if (recv_buf == 'H' && (bytes_received == 0)) //�������ݵ�һ֡
		{
			buf[bytes_received] = recv_buf;
			bytes_received++;
		}
		else
		{
			if (bytes_received)
			{
				buf[bytes_received] = recv_buf;
				if (bytes_received >= 3) // bytes_received��0��ʼ�����������3�Ļ�˵���ѽ���4�ֽ�
				{
					bytes_received = 0;
					is_new_msg = 1;
					return;
				}  // ֹͣ����
				bytes_received++;
			}
		}
	}
}

/* ���ڳ�ʼ������ */
void serial_init()
{
	/* ʹ��T2�������ʷ�������11.0592MHz��9600 */
	RCLK = 1;
	TCLK = 1;
	RCAP2H = 0xFF;
	RCAP2L = 0xDC;
	/* ���ڷ�ʽ1��8-N-1 */
	SCON = 0x50;
	PCON = 0;
	/* ���������жϣ�T2��ʼ���� */
	ES = 1;
	TR2 = 1;
}

/* �൱��main��loop�Ĳ��֣�ѭ��ִ�� */
void serial_loop()
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