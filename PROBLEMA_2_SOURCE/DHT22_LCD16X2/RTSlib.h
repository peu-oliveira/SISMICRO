//=================================================================
/* DS1307 Real Time Clock Software           */
/* 2nd Dec 2014                              */
/* Copyright 2015 Circuits4You.com           */ 
/* WWW - http://blog.circuits4you.com        */
/* Email - info@circuits4you.com             */
 
/* LCD Pin-5(R/W) must be connected to ground*/
//=================================================================

//Note: 1. Define Clock in Configuration Opetions
//      2. Define RTC Connections in i2c.h
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
//#include "i2c.h"

#define E   PD7
#define RS   PB0

void display(char string[16]);
void displaybyte(char D);
void dispinit(void);
void epulse(void);
void delay_ms(unsigned int de);
void line1();
void line2();


void SetTime(char HH,char MM, char SS, char ampm);
char GetHH();
char GetMM();
char GetSS();

void SetDate(char DD,char MM, char YY);
char GetDD();
char GetMonth();
char GetYY();

void DisplayDateTime();

int Read_RTC(char add);
int Write_RTC(char add,char data1);


char mystr[8];
int temp;
//=================================================================
//        Main Function
//=================================================================


//=================================================================
//        LCD Display Initialization Function
//=================================================================
void dispinit(void)
{
 int count;
 char init[]={0x43,0x03,0x03,0x02,0x28,0x01,0x0C,0x06,0x02,0x02};
  
 PORTB &= ~(1<<RS);           // RS=0
 for (count = 0; count <= 9; count++)
  {
 displaybyte(init[count]);
  }
 PORTB |= 1<<RS;    //RS=1
}


//=================================================================
//        Enable Pulse Function
//=================================================================
void epulse(void)
{
 PORTD |= 1<<E;
  delay_ms(10); //Adjust delay if required
 PORTD &= ~(1<<E);
 delay_ms(10); //Adjust delay if required
}


//=================================================================
//        Send Single Byte to LCD Display Function
//=================================================================
void displaybyte(char D)
{
//D4=PD6
//D5=PD5
//D6=PB7
//D7=PB6
 //data is in Temp Register
  char K1;
  K1=D;
  K1=K1 & 0xF0;
  K1=K1 >> 4;  //Send MSB
  
  PORTD &= 0x9F;  //Clear data pins 
  PORTB &= 0x3F;
  
  if((K1 & 0x01)==0x01){PORTD |= (1<<PD6);}
  if((K1 & 0x02)==0x02){PORTD |= (1<<PD5);}
  if((K1 & 0x04)==0x04){PORTB |= (1<<PB7);}
  if((K1 & 0x08)==0x08){PORTB |= (1<<PB6);}

 epulse();

  K1=D;
  K1=K1 & 0x0F;  //Send LSB
  PORTD &= 0x9F;  //Clear data pins 
  PORTB &= 0x3F;

  if((K1 & 0x01)==0x01){PORTD |= (1<<PD6);}
  if((K1 & 0x02)==0x02){PORTD |= (1<<PD5);}
  if((K1 & 0x04)==0x04){PORTB |= (1<<PB7);}
  if((K1 & 0x08)==0x08){PORTB |= (1<<PB6);}
 epulse();
}

//=================================================================
//        Display Line on LCD at desired location Function
//=================================================================
void display(char string[16])
{
 int len,count;

 PORTB |= (1<<RS);           // RS=1 Data Mode
  len = strlen(string);

   for (count=0;count<len;count++)
  {
    displaybyte(string[count]);
 }
}

void line1()
{
        PORTB &= ~(1<<RS);           // RS=0 Command Mode
  displaybyte(0x80);  //Move Coursor to Line 1
  PORTB |= (1<<RS);           // RS=1 Data Mode
}
void line2()
{
        PORTB &= ~(1<<RS);           // RS=0 Command Mode
  displaybyte(0xC0);  //Move Coursor to Line 2
  PORTB |= (1<<RS);           // RS=1 Data Mode
}
//=================================================================
//        Delay Function
//=================================================================
void delay_ms(unsigned int de)
{
unsigned int rr,rr1;
   for (rr=0;rr<de;rr++)
   {
  
  for(rr1=0;rr1<30;rr1++)   //395
  {
   asm("nop");
  }
   
   }
}
//=================================================================
//                    RTC1307_READ_WRITE                         //
//=================================================================
int Read_RTC(char add)
{
 int temp1;
 I2C_START_TX(0b11010000);
 i2c_transmit(add);
 i2c_start();
 I2C_START_RX(0b11010000);
 temp1 = i2c_receive(0);
 i2c_stop();
 return(temp1);
}

int Write_RTC(char add,char data1)
{
 I2C_START_TX(0b11010000); //device add. 
    i2c_transmit(add); //Reg. add.
 i2c_transmit(data1);
 i2c_stop();
return 0;
}

//=======================================================================
//                           SET TIME
//=======================================================================
void SetTime(char HH,char MM, char SS, char ampm)
{
 sprintf(mystr,"%03d",SS);
 Write_RTC(0x00,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));

 sprintf(mystr,"%03d",MM);
 Write_RTC(0x01,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));


 sprintf(mystr,"%03d",HH);
 if(ampm == 1)
 {
  Write_RTC(0x02,((((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30)) | 0x40) | 0x20);
    }
 else
 {
  Write_RTC(0x02,((((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30)) | 0x40));
 }
}
//=======================================================================
char GetHH()
{
 return Read_RTC(0x02);
}
//=======================================================================
char GetMM()
{
 return (Read_RTC(0x01) & 0x7F);
}
//=======================================================================
char GetSS()
{
 return Read_RTC(0x00);
}
//=======================================================================
//                   SET DATE
//=======================================================================
void SetDate(char DD,char MM, char YY)
{
 sprintf(mystr,"%03d",DD);
 Write_RTC(0x04,((mystr[1] - 0x30) << 4)  | (mystr[2] - 0x30));

 sprintf(mystr,"%03d",MM);
 Write_RTC(0x05,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));

 sprintf(mystr,"%03d",YY);
 Write_RTC(0x06,((mystr[1] - 0x30) << 4)  | (mystr[2] - 0x30));
}
//=======================================================================
char GetDD()
{
 return Read_RTC(0x04);
}
//=======================================================================
char GetMonth()
{
 char j;
 j=Read_RTC(0x05);
 j=(j & 0x0F) + ((j >> 4) * 10);
 return j; //12/11
}
//=======================================================================
char GetYY()
{
 char k;
 k=Read_RTC(0x06);
 k=(k & 0x0F) + ((k >> 4) * 10);
 return k;
}
//=======================================================================
//                    Display Date and Time
//=======================================================================
void DisplayDateTime()
{
  temp = Read_RTC(0x00);
  
  mystr[7]=48+(temp & 0b00001111);
  mystr[6]=48+((temp & 0b01110000)>>4);
  mystr[5]=':';
 
  temp = Read_RTC(0x01);
  
  mystr[4]=48+(temp & 0b00001111);
  mystr[3]=48+((temp & 0b01110000)>>4);
  mystr[2]=':';
 
  temp = Read_RTC(0x02);
  
  mystr[1]=48+(temp & 0b00001111);
  mystr[0]=48+((temp & 0b00010000)>>4);

  line1(); 
  display("Time:");
  
  displaybyte(mystr[0]);
  displaybyte(mystr[1]);
  displaybyte(mystr[2]);
  displaybyte(mystr[3]);
  displaybyte(mystr[4]);
  displaybyte(mystr[5]);
  displaybyte(mystr[6]);
  displaybyte(mystr[7]);

  temp = Read_RTC(0x02);
  temp = temp & 0x20;
  if(temp == 0x20)
  {
   display(" PM");
  }
  else
  {
   display(" AM");
  }
  temp = Read_RTC(0x06);
  
  mystr[7]=48+(temp & 0b00001111);
  mystr[6]=48+((temp & 0b01110000)>>4);
  mystr[5]=':';
 
  temp = Read_RTC(0x05);
  
  mystr[4]=48+(temp & 0b00001111);
  mystr[3]=48+((temp & 0b01110000)>>4);
  mystr[2]=':';
 
  temp = Read_RTC(0x04);
  
  mystr[1]=48+(temp & 0b00001111);
  mystr[0]=48+((temp & 0b00110000)>>4);
  
  line2();
  display("Date:");
  
  displaybyte(mystr[0]);
  displaybyte(mystr[1]);
  displaybyte(mystr[2]);
  displaybyte(mystr[3]);
  displaybyte(mystr[4]);
  displaybyte(mystr[5]);
  displaybyte(mystr[6]);
  displaybyte(mystr[7]);
}