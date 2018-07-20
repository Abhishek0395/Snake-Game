/*
 * Snake.c
 *
 * Created: 1/3/2018 7:09:04 PM
 * Author : Abhishek
 */ 
#define D4 eS_PORTC4
#define D5 eS_PORTC5
#define D6 eS_PORTC6
#define D7 eS_PORTC7
#define RS eS_PORTC2
#define EN eS_PORTC3
#include <avr/io.h>
#include <stdlib.h>
#define F_CPU 1000000 
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h" //Can be download from the bottom of this article
void drawCharGreen(unsigned char *);
unsigned char ROL(unsigned char, int);
unsigned char ROR(unsigned char, int);
void initializeINT(int);
void update();
void makefood();
unsigned char one[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b11110000,
	0b00000000,
	0b00000000,
	0b00000000
};
unsigned char end[] = {
	0b10000001,
	0b01000010,
	0b00100100,
	0b00011000,
	0b00011000,
	0b00100100,
	0b01000010,
	0b10000001
};
int size=4;
int dir=2;
int flag=0;
int foodx=9,foody=9;
int flags=0;
int row=5,col=3;
int score=0;
struct matrix{
	int r;
	int c;
};
struct matrix abhi[100];
ISR(INT1_vect){
	int a,b;
	GIFR &= ~(1<<INTF1);
	a=abhi[size-1].r-abhi[size-2].r;
	b=abhi[size-1].c-abhi[size-2].c;
	if(a==0)
	{
		dir=3;
	}
	if(b==0)
	{
		dir=4;
	}
	GIFR |= 1<<INTF1;
	
}

ISR(INT0_vect){
	int a,b;
	GIFR &= ~(1<<INTF0);
		a=abhi[size-1].r-abhi[size-2].r;
		b=abhi[size-1].c-abhi[size-2].c;
		if(a==0)
		{
			dir=1;
		}
		if(b==0)
		{
			dir=2;
		}
	GIFR |=1<<INTF0;
	
};
int main(void)
{
	DDRA=0xff;//positive row
	DDRB=0xff;//negative column green
	DDRD = 0x00;
	DDRC = 0xff;
	MCUCSR= (1<<JTD);
	MCUCSR= (1<<JTD);
	Lcd4_Init();
	int cnt = 0;
	MCUCR=0b11110000;
	initializeINT(0);
	initializeINT(1);
	int i;
	for(i=0;i<4;i++)
	{
		abhi[i].r=4;
		abhi[i].c=i;
	}
	makefood();
	while(1)
	{
		int i,a,b,c;
		update();
		if(flags==1)
		makefood();
		if(flag==1)
		{
			for (i=0; i<100; i++)
			drawCharGreen(end);
			return 0;
		}
		for(i=0;i<10;i++)
		drawCharGreen(one);
		_delay_ms(1000);
	}
}
void makefood()
{
	int a,b,c,x,y;
	unsigned char z = 0b00000001;
	unsigned char fix,fail;
	while(1)
	{
	
	b=row%8;
	c=col%8;
	foodx=b;foody=c;
	c=7-c;
	fix=z;
	fix=fix<<c;
	if((one[b]&fix)==0b00000000)
	break;
	row=row+3;
	col=col+2;
	if(row>100000)
	row=5;
	if(col>100000)
	col=3;
	}
	row=row+3;
	col=col+2;
	one[foodx]=one[foodx]+fix;
	flags=0;
}
void update()
{
	unsigned char z = 0b00000001;
	unsigned char fix,fail,fi;
	char p[100];
	int i,a,b,x,y,d;
	int firstx,firsty;
	x=abhi[0].r;
	y=abhi[0].c;
	firstx=x;firsty=y;
	y=7-y;
	fix=z;
	fix=fix<<y;
	fi=fix;
	one[x]=one[x]-fix;
	for(i=0;i<size-1;i++)
	{
		abhi[i]=abhi[i+1];
	}
	if(dir==2)
	{
		abhi[size-1].r=abhi[size-2].r;
		abhi[size-1].c=abhi[size-2].c+1;
	}
	if(dir==1)
	{
		abhi[size-1].r=abhi[size-2].r-1;
		abhi[size-1].c=abhi[size-2].c;
	}
	if(dir==3)
	{
		abhi[size-1].r=abhi[size-2].r+1;
		abhi[size-1].c=abhi[size-2].c;
	}
	if(dir==4)
	{
		abhi[size-1].r=abhi[size-2].r;
		abhi[size-1].c=abhi[size-2].c-1;
	}
	if(abhi[size-1].r>=8)
	flag=1;
	if(abhi[size-1].c>=8)
	flag=1;
	if(abhi[size-1].r<0)
	flag=1;
	if(abhi[size-1].c<0)
	flag=1;
	x=abhi[size-1].r;
	y=abhi[size-1].c;
	d=y;
	y=7-y;
	fix=z;
	fix=fix<<y;
	if((foodx==x)&&(foody==d))
	{
		score+=5;
		flags=1;
		size++;
		for(i=size-2;i>=0;i--)
		{
			abhi[i+1]=abhi[i];
		}
		abhi[0].r=firstx;
		abhi[0].c=firsty;
	}
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,1);
	dtostrf(score,2,2,p);
	//snprintf(p,20,"%d",score);
	Lcd4_Write_String(p);
	if(((one[x]&fix)!=0b00000000) && !(foodx==x && foody==d))
	flag=1;
	else if(flags==1)
	{
	 one[firstx]=one[firstx]+fi;	
	}
	else
	one[x]=one[x]+fix;
}
void drawCharGreen(unsigned char *character)
{
	long i;
	PORTD = 0xFF;
	for (i=0; i<10; i++)
	{
		int j;
		unsigned char row = 0b00000001;
		for (j=0; j<8; j++)
		{
			PORTA = ROL(row, j);
			PORTB = ~character[j];
			_delay_ms(1);
		}
		
	}
}
void initializeINT(int INT_type){

	switch (INT_type){
		case 0 :{
			GICR |=1<<INT0;					//enable INT0
			MCUCR |= 1<<ISC01 | 1<<ISC00;	 //enable rising edge
			
		}
		case 1 :{
			GICR|=1<<INT1;					//enable INT1
			MCUCR |= 1<<ISC11 | 1<<ISC10;   	//enable rising edge
			
		}
		
	}
	sei();
	
}
unsigned char ROL(unsigned char number, int shiftAmount)
{
	int i;
	for (i=0; i<shiftAmount; i++)
	{
		unsigned char MSB = number & 0b10000000;
		MSB = MSB >> 7;
		number = number << 1;
		number = number | MSB;
	}
	return number;
}
unsigned char ROR(unsigned char number, int shiftAmount)
{
	int i;
	for (i=0; i<shiftAmount; i++)
	{
		unsigned char LSB = number & 0b00000001;
		LSB = LSB << 7;
		number = number >> 1;
		number = number | LSB;
	}
	return number;
}

