#ifndef MOTOR_H
#define MOTOR_H

#include "config.h"

/* PWMռ�ձȣ��κ������Խ��Խ�� */
#define PWM_VALUE_MAX 127

/* PIN1���Ͻţ�PIN2���½ţ�PIN2����PWM���� */
#define MOTOR_RIGHT_FRONT_PIN1 P1_0
#define MOTOR_RIGHT_FRONT_PIN2 P1_1
#define MOTOR_RIGHT_REAR_PIN1 P1_2
#define MOTOR_RIGHT_REAR_PIN2 P1_3
#define MOTOR_LEFT_FRONT_PIN1 P1_4
#define MOTOR_LEFT_FRONT_PIN2 P1_5
#define MOTOR_LEFT_REAR_PIN1 P1_6
#define MOTOR_LEFT_REAR_PIN2 P1_7

typedef enum {
	MOTOR_RIGHT_FRONT = 1,
	MOTOR_RIGHT_REAR,
	MOTOR_LEFT_FRONT,
	MOTOR_LEFT_REAR
} motor_type;

void set_motor_state(motor_type motor_id, char speed_pwm);
void t0_pwm_interrupt();
void pwm_init();

#endif