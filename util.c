#include "config.h"

//延时函数	
void delay(unsigned int k)
{
	unsigned int x, y;
	for (x = 0; x < k; x++)
		for (y = 0; y < 2000; y++);
}

#ifdef DEBUG
/************************************************************************/
//字符串发送函数
void send_str(unsigned char* str_send) {
	unsigned char i = 0;
	while (str_send[i] != '\0')
	{
		SBUF = str_send[i];
		while (!TI);				// 等特数据传送
		TI = 0;					// 清除数据传送标志
		i++;					// 下一个字符
	}
}

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