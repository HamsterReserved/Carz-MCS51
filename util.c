/* 工具函数 */

#include "config.h"

//延时函数	
void delay(unsigned int k)
{
	unsigned int x, y;
	for (x = 0; x < k; x++)
		for (y = 0; y < 2000; y++);
}

#ifdef DEBUG
#include "bt_serial.h"

/* 通过蓝牙发送P1（驱动引脚）的情况 */
void dump_pins()
{
	int P1_state = P1;
	char msg_buf[10];
	sprintf(msg_buf, "DMP1 %x\r\n", P1_state);
	send_str(msg_buf);
	sprintf(msg_buf, "%x %x %x %x\r\n", pwm_right_front, pwm_right_rear, pwm_left_front, pwm_left_rear);
	send_str(msg_buf);
}
#endif