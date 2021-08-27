/*
 * GccApplication1.c
 *
 * Created: 8/19/2021 5:07:04 PM
 * Author : Mohamed Elshafie
 */ 

#include <avr/io.h>
#define F_CPU 8000000ul            //speed of clock provided for the delay function in util/delay.h file
#include<util/delay.h>
#define FOSC 8000000              // Clock Speed
#define BAUD 9600                //Baud rate
#define MYUBRR FOSC/16/BAUD-1     //value of UBRR register


void UART_INIT()              //Initialization function for UART
{

	UBRRL = (unsigned char)(MYUBRR) ;         //send the low  8 bits of the UBRR
	UBRRH = (unsigned char)(MYUBRR>>8);       //send the high 4 bits of the UBRR
	UCSRB = ((1<<RXEN)|(1<<TXEN));            //Enable transmit and enable RXENn TXENn
	UCSRC = ((1<<UCSZ1)|(1<<UCSZ0)|(1<<URSEL));     //Frame with 8 bit data , 1 stop bit, no parity UCSZn1 UCSZn0 , Asynchornous mode
}

uint8_t UART_Read()                   //Function to read input from UART
{ while( !(UCSRA & (1<<RXC)) );       //wait until the receiving complete RXCn
	return UDR;                      //return data received by UART and saved in UDR register
}

void PWM_Timer0_Init()     //Initialization function for Timer0 to be used as PWM output for motor A
{
	
	TCNT0 = 0;            //Set Timer Initial value
	
	DDRB  = DDRB | (1<<PB3); //set PB3/OC0 as output pin 
	
	/* Configure timer control register
	 *  Fast PWM Mode WGM01=1 & WGM00=1
	 *  Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 *  clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}
void PWM_Timer1_Init()     //Initialization function for Timer1 to be used as PWM output for motor B
{
	TCNT1L = 0;      //Set Timer Initial value
	TCNT1H = 0;
	
	DDRD  = DDRD | (1<<PD5);   //set PD5/OC1A as output pin 
	
	/* Configure timer control register
	 *  Fast PWM Mode WGM01=1 & WGM00=1
	 *  Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 *  clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR1A = (1<<WGM10) | (1<<COM1A1);
	TCCR1B = (1<<WGM12) | (1<<CS11);
}
void PWM_Timer2_Init()        //Initialization function for Timer2 to be used as PWM output for motor C
{
	
	TCNT2 = 0;        //Set Timer Initial value
	
	DDRD  = DDRD | (1<<PD7); //set PD7/OC2 as output pin 
	
	/* Configure timer control register
	 *  Fast PWM Mode WGM21=1 & WGM20=1
	 *  Clear OC0 when match occurs (non inverted mode) COM20=0 & COM21=1
	 *  clock = F_CPU/8 CS20=0 CS21=1 CS22=0
	 */
	TCCR2 = (1<<WGM20) | (1<<WGM21) | (1<<COM21) | (1<<CS21);
}


int main(void)
{
	
	DDRC |= 0xFF;              //make port C as output pins 
    
	
	PORTC &= 0x00;            // Motors are at stop at the beginning 
	
	UART_INIT();             //Calling UART initialization function
	
	PWM_Timer0_Init();      //Calling Timer0 initialization function
	
	PWM_Timer1_Init();      //Calling Timer0 initialization function
	
	PWM_Timer2_Init();      //Calling Timer0 initialization function
	
	uint8_t x;
	
	char per1,per2,per3;
	
	char frame[12];
	
	for(char i=0;i<12;i++){
		x = UART_Read();       //Reading one byte ASCII from UART and storing it in x variable
		frame[i]=x;              //Storing the whole frame 'AXXXBXXXCXXX' in an array
	}
	
	for(char i=0;i<12;i++){
		frame[i]-=48;           // Converting ASCII input to integer... example: input zero: ASCII=48 so integer = ASCII-48=zero
	}
	
	//combining the XXX of every motor in one variable to use it as a whole integer not separated characters:
	
	per1=frame[1]*100+frame[2]*10+frame[3];    //storing the percentage of the motor A
	per2=frame[5]*100+frame[6]*10+frame[7];    //storing the percentage of the motor B
	per3=frame[9]*100+frame[10]*10+frame[11];  //storing the percentage of the motor C
	
	
    while (1) 
    {
		
		_delay_ms(500);
		
		OCR0=(per1*255/100);             // multiply percentage of motorA's speed by the PWM value(255)... example: 100% : 100*255/100 = 255(max speed)
		//output zero on pin PC1 and one on pin PC0 to make motor rotate clockwise
		PORTC &= (~(1<<PC1));   
		PORTC |= (1<<PC0);
		
		OCR2=(per3*255/100);           // multiply percentage of motorC's speed by the PWM value(255)... example: 100% : 100*255/100 = 255(max speed)
		//output zero on pin PC3 and one on pin PC2 to make motor rotate clockwise
		PORTC &= (~(1<<PC3));
		PORTC |= (1<<PC2);
		
		OCR1AL=(per2*255/100);        // multiply percentage of motorB's speed by the PWM value(255)... example: 100% : 100*255/100 = 255(max speed)
		//output zero on pin PC5 and one on pin PC4 to make motor rotate clockwise
		PORTC &= (~(1<<PC5));
		PORTC |= (1<<PC4);
		    }
}

