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
		serial_loop();
	}
}