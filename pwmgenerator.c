#include <mega32a.h> ///// or any other MCU you want



#define CHANNELS    PORTD
#define High        0xff
#define Low         0x00
#define On          0xff
#define Off         0x00
#define true        1
#define false       0
#define clear(x)    x=0
#define received    0x55
#define start       0xAA


//////////////////////////////////////////////////////////////////////////////
//  for slaves addresses that do not appear in the data series should be used.
//  here a 16 bits addressing scheme is used.
//////////////////////////////////////////////////////////////////////////////
#define ADDRESSMSB  0xFB
#define ADDRESSLSB  0xBF


#define READCMD     0xFD        
#define TURNON      0xF1
#define TURNOFF     0xF0
#define DSBL_MISO   (DDRB.6=0)      ///// important part!
#define EN_MISO     (DDRB.6=1)      ///// important part!
#define ACK         0xAC  
#define Yes         1
#define No          0
#define EOD         0xED
#define CRCC        0xCC            /// no need 
#define NAC         0xEC

unsigned char channel[8]={0,150,20,100,250,10,15,200};              ///// test PWM output for simulation
unsigned char MASK[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

bit newData;


unsigned char DataIn[20],selected=0,index=0, chkNextForLSB=0;//MSB,OVF,
// SPI interrupt service routine
unsigned char CRC,i;

unsigned char data,dataAvailable,DataOut[20],addressValid,receiving;
bit END=0,freeTime;



// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Reinitialize Timer 0 value
// Place your code here
TCCR0=  0x00;

TIMSK=(1<<TOIE1);
TCNT0=0;
CHANNELS=Off;
//PWM=Off;
}


// Timer1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
// Reinitialize Timer1 value
TCNT1H=0xB1;//0x63C0 >> 8;
TCNT1L=0xE0;//0x63C0 & 0xff;

CHANNELS=High;
// Place your code here
TCCR0=  (1<<CS01) | (1<<CS00);

TCNT0=0x06; 
TIMSK =(1<<TOIE1) |(1<<TOIE0);

//PWM=On;


}

// SPI interrupt service routine
interrupt [SPI_STC] void spi_isr(void)
{
data=SPDR;

// Place your code here
if(chkNextForLSB)
    chkNextForLSB--;        
if((data==ADDRESSMSB)&&(freeTime))//&&(!selected)  ///////////// freeTime here: because of critical timing for my PWM project,
    chkNextForLSB=2;                               ///////////// I decided to read SPI only on freetime of the slave. it is not
if((data==ADDRESSLSB) && (chkNextForLSB)){         ///////////// a part of SPI communication. but part of my project.
    addressValid=Yes;
    index=0;
    selected=Yes;
    }
if((addressValid)){             ///// writing section
    if(data==ADDRESSLSB)        ///// while writing Master keeps writing slave's LSB address on SPI MOSI 
        {
        EN_MISO;
        receiving=No;
        SPDR=DataOut[index];

        }
    else{                       /////// reading section
        if((selected)){
            if((data==EOD)){    //  ||(index==20)
                index=0;
                selected=No;
                for(i=1;i<9;i++)                ////// 8 bytes store in PWM channels (or any number of data)
                    channel[i-1]=DataIn[i];
                SPDR=ACK;    
                }
            receiving=Yes;

            DataIn[index]=data;             
            index++;
            //PORTC.0=0;
            }
        if(data==ACK){
            DSBL_MISO;//END=Yes;
            addressValid=No;
            index=0;
            }
        }
    }
}






void main(void)
{
// Declare your local variables here
unsigned char i, FreeRunner;
// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=0x00;
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=Out Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (1<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=0 Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=0xff;
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 125.000 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
// Timer Period: 2 ms
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;//0x06;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 1000.000 kHz
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 20 ms
// Timer1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
TCNT1H=0xB1;
TCNT1L=0xE0;
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

// SPI initialization
// SPI Type: Slave
// SPI Clock Rate: 2000.000 kHz
// SPI Clock Phase: Cycle Start
// SPI Clock Polarity: Low
// SPI Data Order: MSB First
SPCR=(1<<SPIE) | (1<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
SPSR=(0<<SPI2X);

// Clear the SPI interrupt flag
#asm
    in   r30,spsr
    in   r30,spdr
#endasm


// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Global enable interrupts
#asm("sei")

while (1)
      {
      
      
 //////////// this PWM section is a 8 channels radio controller simulator for a drone. with duty cycle of 10% maximum and 50Hz
    while((TCNT0>130)) {
        freeTime=No;
        // &&(TCNT0<255)
        for(i=0;i<8;i++){
            FreeRunner=TCNT0-130;///  (131---255)  -130 => 1-125
            if(FreeRunner>=channel[i])
                if(CHANNELS&&MASK[i])
                    CHANNELS&=~MASK[i];
            }
        if(FreeRunner==125)
            CHANNELS=Off;    
          
        }
        
        
        freeTime=Yes;


    
        
        
           
        
      
      

      }
}
