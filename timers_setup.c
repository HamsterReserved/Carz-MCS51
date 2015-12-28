/* 定时器初始化入口 */
#include "config.h"
#include "motor_pwm.h"
#include "bt_serial.h"

void t0_setup()
{
	pwm_init();
}

void t1_setup()
{

}

void t2_setup()
{
	serial_init();
}