/****************************************************************************
 硬件连接
 蓝牙有4条线定义：
  VCC	 5V
  GND	 GND
  TDX	 TX
  RXD	 RD
 把蓝牙插入单片机J2接口中
****************************************************************************/

#include <AT89X52.H>
#include "config.h"
#include "motor_pwm.h"
#include "bt_serial.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/* bt_serial.c */
/* 串口接收缓冲区 */
extern unsigned char buf[5];
/* 新信息到达标志 */
extern bit is_new_msg;

/*--主函数--*/
void main(void)
{
#ifdef DEBUG
	char msg_buf[15];
#endif
	serial_init();
	pwm_init();
	EA = 1;
	P1 = 0; // 程序以P1=0为始态

	while (1)
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
}