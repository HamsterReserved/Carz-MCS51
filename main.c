/****************************************************************************
 Ӳ������
 ������4���߶��壺
  VCC	 5V
  GND	 GND
  TDX	 TX
  RXD	 RD
 ���������뵥Ƭ��J2�ӿ���
****************************************************************************/

//#define DEBUG

#include <AT89X52.H>
#ifdef DEBUG
#include <stdio.h>
#endif

bit  is_new_msg = 0; // �Ƿ��ѽ��˴ε���Ϣ���ݸ������Ѿ�����ʱ���ٴ���buf���ڽ��պ���ٴ���Ϊ0

/* ���ڽ��ջ����� */
unsigned char buf[5];

/* PWMռ�ձȣ��κ������Խ��Խ�� */
#define PWM_VALUE_MAX 127
unsigned char pwm_left_front;
unsigned char pwm_left_rear;
unsigned char pwm_right_front;
unsigned char pwm_right_rear;

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

/************************************************************************/
//��ʱ����	
void delay(unsigned int k)
{
	unsigned int x, y;
	for (x = 0; x < k; x++)
		for (y = 0; y < 2000; y++);
}

#ifdef DEBUG
/************************************************************************/
//�ַ������ͺ���
void send_str(unsigned char* str_send) {
	unsigned char i = 0;
	while (str_send[i] != '\0')
	{
		SBUF = str_send[i];
		while (!TI);				// �������ݴ���
		TI = 0;					// ������ݴ��ͱ�־
		i++;					// ��һ���ַ�
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
	0�������Ǹ����⣺�ٶ�0ʱdirection��pwm��Ҫ��0��������ת������ʱ����1��ȡ��
	�ʰ�0�ٶȹ�Ϊ0x80��ʹ����ø������㷽�������߼�
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

/************************************************************************/
void serial_int()
#ifndef WIN32
interrupt 4	  //�����ж�
#endif
{
	unsigned char recv_buf = 0;
	static unsigned char bytes_received = 0; // ��0�����ѿ�ʼ����

	if (RI) //�Ƿ�����ж�
	{
		RI = 0;
		recv_buf = SBUF;
		if (recv_buf == 'H' && (bytes_received == 0)) //�������ݵ�һ֡
		{
			buf[bytes_received] = recv_buf;
			bytes_received++;
		}
		else
		{
			if (bytes_received)
			{
				buf[bytes_received] = recv_buf;
				if (bytes_received >= 3) // bytes_received��0��ʼ�����������3�Ļ�˵���ѽ���4�ֽ�
				{
					bytes_received = 0;
					is_new_msg = 1;
					return;
				}  // ֹͣ����
				bytes_received++;
			}
		}
	}
}

/* ���ڳ�ʼ������ */
void serial_init()
{
	/* ʹ��T2�������ʷ�������11.0592MHz��9600 */
	RCLK = 1;
	TCLK = 1;
	RCAP2H = 0xFF;
	RCAP2L = 0xDC;
	/* ���ڷ�ʽ1��8-N-1 */
	SCON = 0x50;
	PCON = 0;
	/* �������жϣ�T2��ʼ���� */
	ES = 1;
	TR2 = 1;
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

/*********************************************************************/
/*--������--*/
void main(void)
{
#ifdef DEBUG
	char msg_buf[15];
#endif
	serial_init();
	pwm_init();
	EA = 1;
	P1 = 0; // ������P1=0Ϊʼ̬

	while (1)
	{
		if (is_new_msg == 1)
		{
			is_new_msg = 0;
			if (buf[0] == 'H' && buf[1] == 'S')	//��һ���ֽ�ΪH���ڶ����ֽ�ΪS���������ֽ�Ϊ������
			{
				// ��ʱ��һ��ĵ������Ϊͬһ��PWMֵ
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