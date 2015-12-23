/****************************************************************************
 硬件连接
 蓝牙有4条线定义：
  VCC	 5V
  GND	 GND
  TDX	 TX
  RXD	 RD
 把蓝牙插入单片机J2接口中
****************************************************************************/

//#define DEBUG

#include <AT89x51.H>
#ifdef DEBUG
#include <stdio.h>
#endif

bit  is_new_msg = 0; // 是否已将此次的信息传递给马达，当已经传送时不再处理buf。在接收后会再次设为0

/* 串口接收缓冲区 */
unsigned char buf[5];

/* PWM占空比，注意在上脚为0时此值越大表示速度越快（倒车），上脚为1时此值越大越慢 */
#define PWM_VALUE_MAX 127
unsigned char pwm_left_front;
unsigned char pwm_left_rear;
unsigned char pwm_right_front;
unsigned char pwm_right_rear;

/* PIN1是上脚，PIN2是下脚，PIN2可用PWM调速 */
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
	0在这里是个例外：速度0时direction和pwm都要是0，正向旋转的其他时候都是1、取反
	故把0速度归为0x80，使其采用负数计算方法来简化逻辑
	*/
	if (speed_pwm == 0)
		speed_pwm = 0x80;

	/* 正转时PWM值越大速度越慢，反转时PWM越大速度越快 */
	if (speed_pwm & 0x80) // 取符号位
	{
		// 符号位为1，反向旋转（或（纠正后的）停止）
		real_pwm_state = speed_pwm & 0x7f; // 去掉符号
		direction = 0;
	}
	else
	{
		// 符号位为0， 正向旋转
		real_pwm_state = PWM_VALUE_MAX - speed_pwm;
		direction = 1;
	}
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

/************************************************************************/
void serial_int()
#ifndef WIN32
interrupt 4	  //串口中断
#endif
{
	unsigned int recv_buf = 0;
	static unsigned int bytes_received = 0; // 非0表明已开始接收

	if (RI) //是否接收中断
	{
		RI = 0;
		recv_buf = SBUF;
		if (recv_buf == 'H' && (bytes_received == 0)) //接收数据第一帧
		{
			buf[bytes_received] = recv_buf;
			bytes_received++;
		}
		else
		{
			if (bytes_received)
			{
				buf[bytes_received] = recv_buf;
				if (bytes_received >= 3) // bytes_received从0开始，到这里等于3的话说明已接收4字节
				{
					bytes_received = 0;
					is_new_msg = 1;
					return;
				}  // 停止接收
				bytes_received++;
			}
		}
	}
}
/*********************************************************************/
/*--主函数--*/
void main(void)
{
#ifdef DEBUG
	char msg_buf[15];
#endif

	TMOD = 0x22; // T1方式2，自动重装0xfd，用于串口波特率。T0方式2，自动重装0x9c（156us，6410次中断/秒）用于PWM模拟
	TH0 = 0x9c;
	TL0 = 0x9c;
	TH1 = 0xfd; //11.0592M晶振，9600波特率
	TL1 = 0xfd;
	SCON = 0x50; // 串口方式2，9600-8-1Start-1Programmable-1Stop
	PCON = 0x00; // 波特率不加倍
	TR1 = 1;
	TR0 = 1;
	ES = 1;
	ET0 = 1;
	EA = 1;

	P1 = 0;

	while (1)
	{
		if (is_new_msg == 1)
		{
			is_new_msg = 0;
			if (buf[0] == 'H' && buf[1] == 'S')	//第一个字节为H，第二个字节为S，后两个字节为控制码
			{
				// 暂时把一侧的电机设置为同一个PWM值
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