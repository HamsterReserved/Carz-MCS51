/* 定时器初始化入口 */
#include "config.h"
#include "motor_pwm.h"
#include "bt_serial.h"
#include "distance.h"

void t0_setup()
{
	pwm_init();
}

void t1_setup()
{
	distance_t1_init();
}

void t2_setup()
{
	serial_init();
}