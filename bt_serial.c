/* 蓝牙串口相关 */

#include <AT89X52.h>
#include "config.h"
#include "motor_pwm.h"

/* 串口接收缓冲区 */
unsigned char buf[5];

/* 新信息到达标志 */
bit is_new_msg = 0;

/* 串口中断服务程序 */
void bt_serial_recv_interrupt()
#ifndef WIN32
interrupt 4	  //串口中断
#endif
{
	unsigned char recv_buf = 0;
	static unsigned char bytes_received = 0; // 非0表明已开始接收

	if (RI) //是否接收中断
	{
		RI = 0;
		recv_buf = SBUF;
		if (recv_buf == 'H' && (bytes_received == 0)) //接收数据第一帧
		{
			buf[bytes_received] = recv_buf;
			bytes_received++;
		}
		else
		{
			if (bytes_received)
			{
				buf[bytes_received] = recv_buf;
				if (bytes_received >= 3) // bytes_received从0开始，到这里等于3的话说明已接收4字节
				{
					bytes_received = 0;
					is_new_msg = 1;
					return;
				}  // 停止接收
				bytes_received++;
			}
		}
	}
}

/* 串口初始化函数 */
void serial_init()
{
	/* 使用T2做波特率发生器，11.0592MHz，9600 */
	RCLK = 1;
	TCLK = 1;
	RCAP2H = 0xFF;
	RCAP2L = 0xDC;
	/* 串口方式1，8-N-1 */
	SCON = 0x50;
	PCON = 0;
	/* 允许串口中断，T2开始工作 */
	ES = 1;
	TR2 = 1;
}

/* 字符串发送函数 */
void send_str(unsigned char* str_send)
{
	unsigned char i = 0;
	while (str_send[i] != '\0')
	{
		SBUF = str_send[i];
		while (!TI);				// 等特数据传送
		TI = 0;					// 清除数据传送标志
		i++;					// 下一个字符
	}
}

/* 相当于main中loop的部分，循环执行 */
void serial_loop()
{
	if (is_new_msg == 1)
	{
		is_new_msg = 0;
		if (buf[0] == 'H' && buf[1] == 'S')	//第一个字节为H，第二个字节为S，后两个字节为控制字
		{
			// 暂时把一侧的电机设置为同一个PWM值
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