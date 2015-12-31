#include <AT89x52.H>		//器件配置文件
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
using 1			 //扫描数码管
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
void Conut(void)	  //计算函数
{
	unsigned int signal_last_time = 0;
	unsigned int distance = 0;
	unsigned char bt_msg[4] = {'D', 0, 0, 0};
	
	signal_last_time = signal_end_tick - signal_start_tick;
	signal_end_tick = 0;
	signal_start_tick = 0;

	distance = signal_last_time*17;     //算出来是CM
	if ((distance >= 400)) //超出测量范围显示“-”
	{
		//flag = 0;
		bt_msg[1] = disbuff[0] = 10;	   //“-”
		bt_msg[2] = disbuff[1] = 10;	   //“-”
		bt_msg[3] = disbuff[2] = 10;	   //“-”
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
interrupt 3 		 //T1中断用来扫描数码管和计800MS启动模块
using 1
#endif
{
	unsigned int eight_hundreds = 0;
	static unsigned int last_eight_hundreds = 0;
	TH1 = 0xfb;
	TL1 = 0xae;
	Display();
	elapsed_ticks++; // 等待自动溢出，约1小时
	eight_hundreds = elapsed_ticks / 800;
	if (last_eight_hundreds != eight_hundreds)
	{
		last_eight_hundreds = eight_hundreds;
		TX = 1;			                //800MS  启动一次模块
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
	TMOD = (TMOD & T0_MASK_) | 0x10; //设T1为方式1；
	TH1 = 0xfb; //100us定时
	TL1 = 0xae;
	ET1 = 1; //允许T1中断
	TR1 = 1; //开启定时器
}

void distance_loop()
{
	while (!RX);		//当RX为零时等待    
	signal_start_tick = elapsed_ticks; //开启计数
	while (RX);			//当RX为1计数并等待			
	signal_end_tick = elapsed_ticks; //关闭计数
	Conut();			//计算
}