/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2/19/2023
Author  : 
Company : 
Comments: 


Chip type               : ATmega32A
Program type            : Application
AVR Core Clock frequency: 16.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32a.h>
#include <delay.h>
#include <spi.h>

#define PWMADDRESS  0xFAAF
#define SNSADDRESS  0xFBBF
#define CMDADDRESS  0xFCCF


#define READCMD        0xFD
#define WRITECMD       0xFE
#define USERCMD        0xCD
#define SNDATA        0xDA
#define TURNON         0xF1
#define TURNOFF        0xF0
#define ACK            0xAC  
#define NAC            0xEC
#define EOD            0xED
#define GET            0xFF  
#define Yes         1
#define No          0
#define ON          1
#define OFF         0
#define LED         PORTB.0
#define CRCC           0xCC
#define clear(x)    x=0
eeprom float f1=1280,f2=35.7393055,f3=50.9835041;

float temperature,pressure;

struct location{

    float   lat;
    float   lon;
    float   alt;
    float   speed;
    long    timeStamp; 

};
struct location home, target;

union FLOAT2CHAR{
float value;
unsigned char B[4];
};

union FLOAT2CHAR tempData;


// Declare your global variables here


//// Timer1 overflow interrupt service routine
//char Xntr=1;
//interrupt [TIM1_OVF] void timer1_ovf_isr(void)
//{
//// Reinitialize Timer1 value
//TCNT1H=0x85EE >> 8;
//TCNT1L=0x85EE & 0xff;
//// Place your code here
//Xntr++;
//if(Xntr==9)
//    Xntr=1;
//}
// Timer1 overflow interrupt service routine
char Xntr;
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
// Reinitialize Timer1 value
TCNT1H=0x85EE >> 8;
TCNT1L=0x85EE & 0xff;
// Place your code here
Xntr++;
if(Xntr==7)
    Xntr=1;
}


// SPI interrupt service routine
unsigned char dataIn[20],dataOut[20];//// ,index=0;

//interrupt [SPI_STC] void spi_isr(void)
//{
//data[index]=SPDR;
//// Place your code here
//if(index<9)
//index++;
//}


char masterRead(unsigned int Address, unsigned char Qtt){

unsigned char i,TMP,CRC,temp;
TMP=Address>>8;
spi( TMP );
delay_us(10);
TMP=Address & 0xFF;
spi(TMP);
delay_us(10);
for(i=0,CRC=0;i<Qtt;i++){
    dataIn[i]=spi(TMP);
    //CRC+=dataIn[i];
    delay_us(10);
    }     
//temp=spi(TMP);              /////// reading CRC
//delay_us(50);
spi(EOD);
delay_us(30);
//if(CRC==temp)
    if(spi(ACK)==ACK)
        return 1; 
//else
//    spi(NAC);    
return 0;            
}




char masterWrite(unsigned int Address, unsigned char Qtt){

unsigned char i,TMP,CRC;
CRC=0;
delay_us(10);
TMP=Address>>8;
spi( TMP );
delay_us(10);
TMP=Address & 0xFF;
spi( TMP );
delay_us(10);

for(i=0;i<Qtt;i++){
    //CRC+=dataOut[i];
    spi(dataOut[i]);            
    delay_us(10);
    }
//if((CRC!=EOD)&&(CRC!=ACK)&&(CRC!=TMP))
//    spi( CRC );
//else
//    spi( CRCC );    
//delay_us(50);
spi(EOD);        /// i=Qtt+1
delay_us(30);               //// End of Data
if(spi(ACK)==ACK)
    return 1;
return 0;

}

void main(void)
{
// Declare your local variables here
char X,USERDATA,CMDDATA,SNSDATA,PWMDATA,i;
// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=Out Bit6=In Bit5=Out Bit4=Out Bit3=In Bit2=In Bit1=In Bit0=Out 
DDRB=(1<<DDB7) | (0<<DDB6) | (1<<DDB5) | (1<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (1<<DDB0);
// State: Bit7=0 Bit6=T Bit5=0 Bit4=0 Bit3=T Bit2=T Bit1=T Bit0=0 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 15.625 kHz
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 2 s
// Timer1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);
TCNT1H=0x85;
TCNT1L=0xEE;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
//TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (1<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);

// USART initialization
// USART disabled
UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (0<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
SFIOR=(0<<ACME);

// ADC initialization
// ADC disabled
ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

//// SPI initialization
//// SPI Type: Master
//// SPI Clock Rate: 1000.000 kHz
//// SPI Clock Phase: Cycle Start
//// SPI Clock Polarity: Low
//// SPI Data Order: MSB First
//SPCR=(1<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (1<<SPR0);
//SPSR=(0<<SPI2X);
//
//// Clear the SPI interrupt flag
//#asm
//    in   r30,spsr
//    in   r30,spdr
//#endasm



//// SPI initialization
//// SPI Type: Master
//// SPI Clock Rate: 250.000 kHz// 2*250.000 kHz
//// SPI Clock Phase: Cycle Start
//// SPI Clock Polarity: Low
//// SPI Data Order: MSB First
//SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (0<<SPR0);
//SPSR=(0<<SPI2X); /////1


// SPI initialization
// SPI Type: Master
// SPI Clock Rate: 1000.000 kHz
// SPI Clock Phase: Cycle Start
// SPI Clock Polarity: Low
// SPI Data Order: MSB First
SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (1<<SPR0);
SPSR=(0<<SPI2X);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Global enable interrupts
#asm("sei")



home.alt=f1;
home.lat=f2;
home.lon=f3;

Xntr=1;


    
while (1)
      {
      // Place your code here
////////////////////////////////////////////      
//        USERDATA=No;
//        if(masterRead(CMDADDRESS,13))
//            CMDDATA=Yes;
////////////////////////////////////////////

///////////////////////////////////////////        
      if(masterRead(SNSADDRESS,13))
            SNSDATA=Yes;  
//////////////////////////////////////////////            
//
        if(SNSDATA){
            clear(SNSDATA);            
            if(dataIn[12]==SNDATA){
                //LED=ON;
                //////////////////////////

                for(i=0;i<4;i++)
                    tempData.B[i]=dataIn[i]; //2+i
                temperature=tempData.value;
                
                for(i=0;i<4;i++)
                    tempData.B[i]=dataIn[4+i];
                pressure=tempData.value;
                
                for(i=0;i<4;i++)
                    tempData.B[i]=dataIn[8+i]; //10+i
                target.alt=tempData.value;
                
                for(i=0;i<20;i++){
                    dataOut[i]=dataIn[i]; 
                    dataIn[i]=0;
                    }   
                //delay_ms(2000);
                //LED=OFF;
                } 
            }
////////////////////////////////////////    
//    tempData.value=home.lat;
//    for(i=0;i<4;i++)
//        dataOut[i]=tempData.B[i]; //2+i
//        
//    tempData.value=home.lon;
//    for(i=0;i<4;i++)
//        dataOut[i+4]=tempData.B[i];
//        
//    tempData.value=home.alt;
//    for(i=0;i<4;i++)
//        dataOut[i+8]=tempData.B[i]; 
///////////////////////////////////////
            
///////////////////////////////////////
//        if(CMDDATA){
//            CMDDATA=No;
//            for(i=0;i<4;i++)
//                tempData.B[i]=dataIn[i]; //2+i
//            target.lat=tempData.value;
//            
//            for(i=0;i<4;i++)
//                tempData.B[i]=dataIn[4+i]; //6+i
//            target.lon=tempData.value;
//            
//            for(i=0;i<4;i++)
//                tempData.B[i]=dataIn[8+i]; //10+i
//            target.alt=tempData.value;
//            
//            if(dataIn[12]==USERCMD){
//                LED=ON;
//                for(i=0;i<20;i++)
//                    dataIn[i]=0;    
//                f1=target.alt;
//                delay_us(10);
//                f2=target.lat;
//                delay_us(10);
//                f3=target.lon;
//                delay_us(10);
//                delay_ms(2000);
//                LED=OFF;
//                } 
//            }
//    
//        tempData.value=home.lat;
//        for(i=0;i<4;i++)
//            dataOut[i]=tempData.B[i]; //2+i
//        
//        tempData.value=home.lon;
//        for(i=0;i<4;i++)
//            dataOut[i+4]=tempData.B[i];
//        
//        tempData.value=home.alt;
//        for(i=0;i<4;i++)
//            dataOut[i+8]=tempData.B[i];
///////////////////////////////////////            
            
            
//////////////////////////////////////////        
//        for(i=0;i<15;i++)
//            dataOut[i]='A'+i; 
/////////////////////////////////////////

//                temperature=21.5;
//                pressure=120.5;
//                target.alt=1200;
//                
//                tempData.value=temperature;
//                for(i=0;i<4;i++)
//                    dataOut[i]=tempData.B[i];
//                    //tempData.B[i]=dataIn[i]; //2+i
//                //temperature=tempData.value;
//                tempData.value=pressure;
//                for(i=0;i<4;i++)
//                    dataOut[4+i]=tempData.B[i];
//                    //tempData.B[i]=dataIn[4+i]; //6+i
//                //pressure=tempData.value;
//                
//                tempData.value=target.alt;
//                for(i=0;i<4;i++)
//                    dataOut[8+i]=tempData.B[i];
//                    //tempData.B[i]=dataIn[8+i]; //10+i
//                //target.alt=tempData.value;
                
   

        if(masterWrite(CMDADDRESS,12)){
            //USERDATA=Yes;
            LED=ON;
            delay_ms(100);
            LED=OFF; 
        }
               
            
        delay_ms(50);
        //LED = OFF;    
            //}
        
        }  
      
      
}
