#include <AT89x52.H>		//���������ļ�
#include <intrins.h>
#include "bt_serial.h"
#define  RX  P2_4
#define  TX  P2_5

unsigned int elapsed_ticks = 0;
unsigned int signal_start_tick = 0;
unsigned int signal_end_tick = 0;

unsigned char const discode[] = { 0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xff/*-*/ };
unsigned char const position[4] = { 0xfe,0xfd,0xfb,0xf7 };
unsigned char disbuff[4] = { 0,0,0,0, };
/********************************************************/
void Display(void)
#ifndef WIN32
using 1			 //ɨ�������
#endif
{
	static unsigned char posit = 0;
	if (posit == 0)
	{
		P0 = (discode[disbuff[posit]]) & 0x7f;
	}
	else
	{
		P0 = discode[disbuff[posit]];
	}
	P2 = position[posit];
	if (++posit >= 4)
		posit = 0;
}
/********************************************************/
void Conut(void)	  //���㺯��
{
	unsigned int signal_last_time = 0;
	unsigned int distance = 0;
	unsigned char bt_msg[4] = {'D', 0, 0, 0};
	
	signal_last_time = signal_end_tick - signal_start_tick;
	signal_end_tick = 0;
	signal_start_tick = 0;

	distance = signal_last_time*17;     //�������CM
	if ((distance >= 400)) //����������Χ��ʾ��-��
	{
		//flag = 0;
		bt_msg[1] = disbuff[0] = 10;	   //��-��
		bt_msg[2] = disbuff[1] = 10;	   //��-��
		bt_msg[3] = disbuff[2] = 10;	   //��-��
	}
	else
	{
		bt_msg[1] = disbuff[0] = distance % 1000 / 100;
		bt_msg[2] = disbuff[1] = distance % 1000 % 100 / 10;
		bt_msg[3] = disbuff[2] = distance % 1000 % 10 % 10;
	}
	send_data(bt_msg, 4);
}

void  distance_t1_interrupt()
#ifndef WIN32
interrupt 3 		 //T1�ж�����ɨ������ܺͼ�800MS����ģ��
using 1
#endif
{
	unsigned int eight_hundreds = 0;
	static unsigned int last_eight_hundreds = 0;
	TH1 = 0xfb;
	TL1 = 0xae;
	Display();
	elapsed_ticks++; // �ȴ��Զ������Լ1Сʱ
	eight_hundreds = elapsed_ticks / 800;
	if (last_eight_hundreds != eight_hundreds)
	{
		last_eight_hundreds = eight_hundreds;
		TX = 1;			                //800MS  ����һ��ģ��
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		TX = 0;
	}
}
/*********************************************************/
void distance_t1_init()
{
	TMOD = (TMOD & T0_MASK_) | 0x10; //��T1Ϊ��ʽ1��
	TH1 = 0xfb; //100us��ʱ
	TL1 = 0xae;
	ET1 = 1; //����T1�ж�
	TR1 = 1; //������ʱ��
}

void distance_loop()
{
	while (!RX);		//��RXΪ��ʱ�ȴ�    
	signal_start_tick = elapsed_ticks; //��������
	while (RX);			//��RXΪ1�������ȴ�			
	signal_end_tick = elapsed_ticks; //�رռ���
	Conut();			//����
}