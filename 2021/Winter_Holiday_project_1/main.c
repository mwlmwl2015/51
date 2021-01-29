#include <STC89C5xRC.H>
#include "lcd.h"

unsigned  int code num[]={'0','1','2','3','4','5','6','7','8','9','-','h','m','s',' '};
unsigned int hour,minute,second;
unsigned int hour2,minute2,second2;
unsigned int hour3,minute3,second3;
unsigned int k=1,l=1,m=0;//很奇怪，放这就行了

unsigned int mode_flag=2;//模式更新标志

void LcdDisplay();//lcd1602显示
int matrix_button();//4*4矩阵按键
void delayms(unsigned int ms);//毫秒延时
void buzzer();//蜂鸣器响
void t01();//t0_t1定时器
int date_process();//数据处理m、s
int date_process_hour();//数据处理h
void mode();//模式
	
void main()
{
	LcdInit();
	t01();
	mode_flag=0;
	while(1)
	{
		matrix_button();
		LcdDisplay();
		mode();
		if(m==1)
		{
			if(hour3==hour&&minute3==minute)
				m=2;
		}
		if(matrix_button()==8)
			m=0;
	}
}

void LcdDisplay()
{	
		LcdWriteCom(0X80+0x03);
		LcdWriteData(num[date_process_hour()/10]);
		LcdWriteData(num[date_process_hour()%10]);
		LcdWriteData(num[11]);
		LcdWriteData(num[date_process()/600%6]);
		LcdWriteData(num[date_process()/60%10]);
		LcdWriteData(num[12]);
		LcdWriteData(num[date_process()/10%6]);
		LcdWriteData(num[date_process()%10]);
		LcdWriteData(num[13]);
	
//	LcdWriteCom(0X01);清屏

}

int matrix_button()
{
	unsigned int x,key;
	x=0;
	P1=0x7f;//第一行为0
	if(P1!=0x7f)
	{
		delayms(5);
		if(P1!=0x7f)
		{
			key=P1&0x0f;
			switch(key)
			{
				case 0x07: mode_flag=1;LcdWriteCom(0X01);break;//S1
				case 0x0b: x=2;break;//S2
				case 0x0d: x=3;break;//S3
				case 0x0e: x=4;break;//S4
			}
		}
	}
	
	P1=0xbf;//第二行为0
	if(P1!=0xbf)
	{
		delayms(5);
		if(P1!=0xbf)
		{
			key=P1&0x0f;
			switch(key)
			{
				case 0x07: mode_flag=2;	LcdWriteCom(0X01);break;//S5
				case 0x0b: x=6;break;//S6
				case 0x0d: x=7;break;//S7
				case 0x0e: x=8;m=0;break;//S8
			}
		}
	}
	
	P1=0xdf;//第三行为0
	if(P1!=0xdf)
	{
		delayms(5);
		if(P1!=0xdf)
		{
			key=P1&0x0f;
			switch(key)
			{
				case 0x07: mode_flag=3;LcdWriteCom(0X01);break;//S9
				case 0x0b: x=10;break;//S10
				case 0x0d: x=11;break;//S11
				case 0x0e: x=12;break;//S12
			}
		}
	}
	
//	P1=0xef;//第四行为0
//	if(P1!=0xef)
//	{
//		delayms(5);
//		if(P1!=0xef)
//		{
//			key=P1&0x0f;
//			switch(key)
//			{
//				case 0xe07: x=13;break;//S13
//				case 0x0b: x=14;break;//S14
//				case 0x0d: x=15;break;//S15
//				case 0x0e: x=16;break;//S16
//			}
//		}
//	}
	
	return x;
}

void delayms(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
		for(j=0;j<123;j++);
}

void buzzer()
{
	unsigned int i;
	P1=(P1&0xdf)|0x00;//	P1^5,蜂鸣器
	for(i=2;i>0;i--);
	P1=(P1&0xdf)|0x20;
	for(i=2;i>0;i--);
}

void t01()
{
	TMOD&=0xFF;		//设置定时器模式
	TL0=(65536-2000)/256;		//设置定时初值  2ms
	TH0=(65536-2000)%256;		//设置定时初值
	TL1=(65536-50000)/256;		//设置定时初值  50ms
	TH1=(65536-50000)%256;		//设置定时初值
	EA=1;
	ET0=1;
	ET1=0;
	TR0=1;
	TR1=0;
}

void t0_() interrupt 1
{
	unsigned int count;
	TL0=(65536-2000)/256;		//设置定时初值  2ms
	TH0=(65536-2000)%256;		//设置定时初值
	count++;
	
	if(m==2)
	{
		buzzer();
	}
	
	if(count==200)//大致1s
	{
		count=0;
		second++;
		if(second==60)//大致1m
		{
			second=0;
			minute++;
			if(minute==60)//大致1h
			{
				minute=0;
				hour++;
				if(hour==24)//大致1d
				{
					hour=0;
				}
			}
		}
	}
}

void t1_() interrupt 3
{
	unsigned int count;
	TL1=(65536-50000)/256;		//设置定时初值  50ms
	TH1=(65536-50000)%256;		//设置定时初值
	count++;
	
//	if(count==20)//大致1s
	if(count==400)//大致1s
	{
		count=0;
		second2++;
		if(second2==60)//大致1m
		{
			second2=0;
			minute2++;
			if(minute2==60)//大致1h
			{
				minute2=0;
				hour2++;
				if(hour2==24)//大致1d
				{
					hour2=0;
				}
			}
		}
	}
}
int date_process()//s、m
{
	unsigned int t;
	if(mode_flag==1||mode_flag==0)
	{
		if(minute>0)
			t=minute*60+second;
		else if(second>0)
			t=second;
		else t=0;
	}
	if(mode_flag==2)
	{
		if(minute2>0)
			t=minute2*60+second2;
		else if(second2>0)
			t=second2;
		else t=0;
	}
	if(mode_flag==3)
	{
		if(minute3>0)
			t=minute3*60+second3;
		else if(second3>0)
			t=second3;
		else t=0;
	}
	return t;
}

int date_process_hour()//h
{
	unsigned int j;
	if(mode_flag==1||mode_flag==0)
		j=hour;
	if(mode_flag==2)
		j=hour2;
	if(mode_flag==3)
		j=hour3;
	return j;
}

void mode()//模式
{
	unsigned int a;
	if(mode_flag==1)
	{
		LcdWriteCom(0X80+0x4E);
		LcdWriteData(num[mode_flag]);
				LcdWriteData(num[k+10]);
		if(matrix_button()==2)
		{
			k++;//k==1--h,k==2--m,k==3--s
			if(k>=4) k=1;
		}
		if(matrix_button()==3)
		{
			if(k==1) 
			{
				hour++;
				if(hour>24)
					hour=0;
			}
			if(k==2)
			{
				minute++;
				if(minute>60)
					minute=0;
			}
			if(k==3) 
			{
				second++;
				if(second>60)
					second=0;
			}
		}	
		if(matrix_button()==4)
		{
			mode_flag=0;
			LcdWriteCom(0X01);//清屏	
		}	
		
	}
	
	if(mode_flag==2)
	{
		LcdWriteCom(0X80+0x4E);
		LcdWriteData(num[mode_flag]);	
		LcdWriteData(num[l]);		
		if(matrix_button()==6)
		{
			l++;
			if(l>=3)
				l=1;
		}
		if(l==1)
		{
			ET1=0;
			TR1=0;
		}
		if(l==2)
		{
			ET1=1;
			TR1=1;
		}		
		if(matrix_button()==7)
			hour2=minute2=second2=0;
		if(matrix_button()==8)
		{
			mode_flag=0;
			LcdWriteCom(0X01);//清屏	
		}
	}
	
	if(mode_flag==3)
	{
		LcdWriteCom(0X80+0x4E);
		LcdWriteData(num[mode_flag]);
		LcdWriteData(num[a+11]);
		m=0;
		if(matrix_button()==10)
		{
			a++;
			if(a>=2)
				a=0;
		}
		if(matrix_button()==11)
		{
			if(a==0)
			{
				hour3++;
				if(hour3>24)
					hour3=0;
			}
			if(a==1)
			{
				minute3++;
			}
		}
		if(matrix_button()==12)
		{
			m=1;
			mode_flag=0;
			LcdWriteCom(0X01);//清屏	
		}
	}
}
