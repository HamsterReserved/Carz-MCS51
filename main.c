#include <AT89X52.H>
#include "config.h"
#include "motor_pwm.h"
#include "bt_serial.h"
#include "timers_setup.h"
#include "distance.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/*--主函数--*/
void main(void)
{
#ifdef DEBUG
	char msg_buf[15];
#endif
	t0_setup();
	t1_setup();
	t2_setup(); // 初始化用到的定时器
	EA = 1;
	P1 = 0; // 程序以P1=0为始态

	while (1)
	{
		distance_loop();
	}
}