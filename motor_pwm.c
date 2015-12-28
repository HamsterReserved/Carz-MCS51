/* ���������PWM */

#include <AT89X52.h>
#include "motor_pwm.h"
#include "config.h"

unsigned char pwm_left_front;
unsigned char pwm_left_rear;
unsigned char pwm_right_front;
unsigned char pwm_right_rear;

/*
���õ������״̬��

���������Ͻŵĵ�ƽ���½ŵ�ƽ��PWM�������˴�����PWM�����趨��

motor_id�� Ҫ���Ƶĵ����š���ǰ1���Һ�2����ǰ3�����4
speed_pwm���ٶȡ��������㣬��Ҫ��������ֵ��-127~127��PWM_VALUE_MAX���ڡ�������ʾ����
*/
void set_motor_state(motor_type motor_id, char speed_pwm)
{
	/* ʵ��PWMռ�ձȣ��������� */
	char real_pwm_state;
	/* ���򣬽�д���Ͻţ�0��1�� */
	bit direction;

#ifdef DEBUG
	char msg_buf[15];
	sprintf(msg_buf, "SST %x %x\r\n", motor_id, speed_pwm);
	send_str(msg_buf);

	dump_pins();
#endif

	/*
	0�������Ǹ����⣬����������������������ٶ�
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
��ʱ��0�жϣ�����PWMģ�⡣
��Ϊ156us����ʽ2
6410���ж�/�룬tick 127��Ϊһ��PWMѭ��
*/
void t0_pwm_interrupt()
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

/* PWMʱ�ӳ�ʼ������ */
void pwm_init()
{
	/* ʹ��T0��ΪPWMʱ�ӣ���ʽ2���Զ���װ0x9c��156us��6410���ж�/�룩 */
	TMOD = (TMOD & T1_MASK_) | 0x02; // T1����
	TH0 = 0x9c;
	TL0 = 0x9c;
	/* �����жϣ���ʼ���� */
	ET0 = 1;
	TR0 = 1;
}