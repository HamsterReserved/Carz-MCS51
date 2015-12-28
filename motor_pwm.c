#include <AT89X52.h>
#include "motor_pwm.h"
#include "config.h"

unsigned char pwm_left_front;
unsigned char pwm_left_rear;
unsigned char pwm_right_front;
unsigned char pwm_right_rear;

/*
设置电机运行状态。

将会设置上脚的电平。下脚电平由PWM产生，此处仅作PWM参数设定。

motor_id： 要控制的电机编号。右前1，右后2，左前3，左后4
speed_pwm：速度。不做折算，故要求输入数值在-127~127（PWM_VALUE_MAX）内。负数表示反向
*/
void set_motor_state(motor_type motor_id, char speed_pwm)
{
	/* 实际PWM占空比，总是正数 */
	char real_pwm_state;
	/* 方向，将写入上脚，0反1正 */
	bit direction;

#ifdef DEBUG
	char msg_buf[15];
	sprintf(msg_buf, "SST %x %x\r\n", motor_id, speed_pwm);
	send_str(msg_buf);

	dump_pins();
#endif

	/*
	0在这里是个例外，不能用下面的流程来计算速度
	*/
	if (speed_pwm == 0) {
		real_pwm_state = 0;
		direction = 0;
		goto zero_rule;
	}

	real_pwm_state = PWM_VALUE_MAX - speed_pwm & 0x7f;
	direction = !(speed_pwm & 0x80);

zero_rule:
#ifdef DEBUG
	sprintf(msg_buf, "SSTC %x %x\r\n", direction, real_pwm_state);
	send_str(msg_buf);
#endif
	switch (motor_id)
	{
	case MOTOR_RIGHT_FRONT:
		pwm_right_front = real_pwm_state;
		MOTOR_RIGHT_FRONT_PIN1 = direction;
		break;
	case MOTOR_RIGHT_REAR:
		pwm_right_rear = real_pwm_state;
		MOTOR_RIGHT_REAR_PIN1 = direction;
		break;
	case MOTOR_LEFT_FRONT:
		pwm_left_front = real_pwm_state;
		MOTOR_LEFT_FRONT_PIN1 = direction;
		break;
	case MOTOR_LEFT_REAR:
		pwm_left_rear = real_pwm_state;
		MOTOR_LEFT_REAR_PIN1 = direction;
		break;
	}
}

/*
定时器0中断，用于PWM模拟。
定为156us，方式2
6410次中断/秒，tick 127次为一个PWM循环
*/
void t0_int()
#ifndef WIN32
interrupt 1
#endif
{
	static int current_pwm_tick = 0;

	if (current_pwm_tick > PWM_VALUE_MAX)
		current_pwm_tick = 0;

	if (current_pwm_tick < pwm_right_front)
		MOTOR_RIGHT_FRONT_PIN2 = 1;
	else
		MOTOR_RIGHT_FRONT_PIN2 = 0;
	if (current_pwm_tick < pwm_right_rear)
		MOTOR_RIGHT_REAR_PIN2 = 1;
	else
		MOTOR_RIGHT_REAR_PIN2 = 0;
	if (current_pwm_tick < pwm_left_front)
		MOTOR_LEFT_FRONT_PIN2 = 1;
	else
		MOTOR_LEFT_FRONT_PIN2 = 0;
	if (current_pwm_tick < pwm_left_rear)
		MOTOR_LEFT_REAR_PIN2 = 1;
	else
		MOTOR_LEFT_REAR_PIN2 = 0;
	current_pwm_tick++;
}

/* PWM时钟初始化函数 */
void pwm_init()
{
	/* 使用T0作为PWM时钟，方式2，自动重装0x9c（156us，6410次中断/秒） */
	TMOD = (TMOD & T1_MASK_) | 0x02; // T1保留
	TH0 = 0x9c;
	TL0 = 0x9c;
	/* 允许中断，开始工作 */
	ET0 = 1;
	TR0 = 1;
}