#include <AT89X52.h>
#include "config.h"

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
	/* �������жϣ�T2��ʼ���� */
	ES = 1;
	TR2 = 1;
}