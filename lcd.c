/* This project is to transmit GPS location to predefined phone number */
 
#include<reg51.h>
#define port2 P2
sbit rs = P1^0;
sbit rw = P1^1;
sbit e = P1^2;
sbit button1 = P0^2;
char info[70];
char test[6]={"$GPGGA"};
char comma_position[15];
unsigned int check=0,i;
unsigned char a;
void receive_data();
void lcd_latitude();
void lcd_longitude();
void transmit();
unsigned char storelat[12];
unsigned char storelon[1];
void retrive();

int z;


 
//DELAY FUNCTION
void delay(unsigned int msec)
{
	int i,j ;
	for(i=0;i<msec;i++)
	for(j=0;j<1275;j++); 
}
 
// LCD COMMAND SENDING FUNCTION
void lcd_cmd(unsigned char item)
{
	port2 = item;
	rs= 0;
	rw=0;
	e=1;
	delay(1);
	e=0;
	return;
} 
 
// LCD DATA SENDING FUNCTION
void lcd_data(unsigned char item)
{
	port2 = item;
	rs= 1;
	rw=0;
	e=1;
	delay(1);
	e=0;			    
	return;
}
 
 // LCD STRING SENDING FUNCTION 
void lcd_string(unsigned char *str)
{
	int i=0;
	while(str[i]!='\0')
	{
        lcd_data(str[i]);
        i++;
        delay(10);
        }
        return;
}
 
// SERIAL PORT SETTING
void serial()
{
	TMOD=0x20;      //MODE=2
	TH1=0xFD;	     // 9600 BAUD
	SCON=0x50  ;    // SERIAL MODE 1 ,8- BIT DATA ,1 STOP BIT ,1 START BIT , RECEIVING ON
	TR1=1;	         //TIMER START
}
 
void find_comma()
{
	unsigned int i,count=0;
	for(i=0;i<70;i++)
	{	
		if(info[i]==',')
		{
			comma_position[count++]=i;
		}
         }
}

void transmit(unsigned char x)
{
	SBUF=x;
	while(TI==0);
	TI=0;
}
		
void compare()
{  
	IE=0x00;	       //Interrupt disable 
	find_comma();	  //Function to detect position of comma in the string
	retrive();
	
	if(button1==0)
		
	{
		
		transmit('A');
		transmit('T');
		transmit(0x0D);
		delay(300);
		transmit('A');
		transmit('T');
		transmit('+');
		transmit('C');
		transmit('M');
		transmit('G');
		transmit('F');
		transmit('=');
		transmit('1');
		transmit(0x0D);
		transmit('A');
		transmit('T');
		transmit('+');
		transmit('C');
		transmit('M');
		transmit('G');
		transmit('S');
		transmit('=');
		transmit('"');
		transmit('0');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('9');
		transmit('"');
		transmit(0x0D);
		delay(300);
		
		transmit('L');
		transmit('A');
		transmit('T');
		
		
		
		
		for(z=0;z<12;z++)
		{
		transmit(storelat[z]);
		}
		transmit(' ');
		delay(100);
		transmit('L');
		transmit('O');
		transmit('N');
		for(z=0;z<12;z++)
		{
			transmit(storelon[z]);
		}
		
		transmit(0x1A);
	}
	check=0;
	IE=0x90;		   //Interrupt enable
}
void receive_data()	   	 interrupt 4					  
{
	info[check++]=SBUF;	  //Read SBUF
	if(check<7)	         //Condition to check the required data
        {
		if(info[check-1]!=test[check-1])
		check=0;
        }
	RI=0;
}
void lcd_shape()	      //Function to create shape of degree
{
	lcd_cmd(64);
	lcd_data(10);
	lcd_data(17);
	lcd_data(17);
	lcd_data(10);
	lcd_data(0);
	lcd_data(0);
	lcd_data(0);
	lcd_data(0);
}
 
void lcd_latitude()		 //Function to display Latitude
{
	unsigned int c2=comma_position[1]; //Position of second comma
	lcd_shape();
	lcd_cmd(0x01);	         // Clear LCD display
	lcd_cmd(0x80);	         //Move cursor to position 6 of line 1
	lcd_string("LAT");	 //Showing Latitude
	 
	lcd_data(info[c2+1]);	
	lcd_data(info[c2+2]);
	lcd_data(0);			 //Degree symbol
	lcd_data(info[c2+3]);	
	lcd_data(info[c2+4]);
	lcd_data(info[c2+5]);
	lcd_data(info[c2+6]);
	lcd_data(info[c2+7]);
	lcd_data(info[c2+8]);
	lcd_data(info[c2+9]);
	lcd_data(0x27);          //ASCII of minute sign(')
	lcd_data(info[c2+10]);
	lcd_data(info[c2+11]);
	
} 

void retrive()
{
	unsigned int c2=comma_position[1];
  unsigned int c4=comma_position[3];	
	for (z=0;z<12;z++)
	{
		storelat[z]=info[c2+z];
		storelon[z]=info[c4+z];
}
}
 
void lcd_longitude()
{
	unsigned int c4=comma_position[3];
	
	lcd_cmd(0xC0);			    //Begining of second line  
	lcd_string("LON");		//Showing Longitude
	
	
	lcd_data(info[c4+1]);
	lcd_data(info[c4+2]);
	lcd_data(info[c4+3]);
	lcd_data(0);
	lcd_data(info[c4+4]);
	lcd_data(info[c4+5]);
	lcd_data(info[c4+6]);
	lcd_data(info[c4+7]);
	lcd_data(info[c4+8]);
	lcd_data(info[c4+9]);
	lcd_data(info[c4+10]);
	lcd_data(0x27);               //ASCII of minute sign(')
	lcd_data(info[c4+11]);
	lcd_data(info[c4+12]);
	delay(250);
}

void main()
{
	serial();
	lcd_cmd(0x38);	        //2 LINE, 5X7 MATRIX
	lcd_cmd(0x0e);         //DISPLAY ON, CURSOR BLINKING
	IE=0x90;
	while(1)
	{
		if(check==69)
		compare();
	}
}