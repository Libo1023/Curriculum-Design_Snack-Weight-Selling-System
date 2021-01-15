#include "iom16v.h"
// Common Cathode 
unsigned char LED_Disbuf[12]={0X3F,0X06,0X5B,0X4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F,0X7C,0X77};       
// Selection Bit---Common Cathode 
unsigned char position[8]={0XFE,0XFD,0XFB,0XF7,0XEF,0XDF,0XBF,0X7F};                              

unsigned int second=0;      
unsigned int minute=55;     
unsigned int hour=20;         


unsigned long int adc_rel; 
unsigned long int adc_val;        

int flag_a=0;                    
int flag_b=0;                    
int spa=2;                        
int spb=4;                       
int tpa=0;                        
int tpb=0;                        
int adopter=0;


// 延时功能
void delayms(unsigned int i)       
{
	unsigned int j;
	while (i--)
	{
		for (j=0;j<665;j++);
	}
}



// 主程序 
void main()                        
{
	DDRA=0X7F;               
	PORTA=0X80;
	DDRB=0XFF;         
	PORTB=0X00;
	DDRC=0XFF;         
	                  
						
	PORTC=0X7F;    
	
	DDRD=0XE0;          
	PORTD=0XFF;
	
	ASSR=0X00;          
	                     //   -    -    -    -    AS2  TCN2UB  OCR2UB  TCR2UB
	                     //   0    0    0    0     0      0       0       0
	                     // 0X00
	
	TCCR2=0X0E;          // FOC2  WGM20  COM21  COM20  WGM21  CS22  CS21  CS20
	                     //  0      0      0      0      1      1     1     0
	                     // 0X0F
	                   
	
	OCR2=124;             // n=(f-clock-cpu)*(t)/(N)=(8MHz)*(1)/(256)=31250  31250/250=125=n
	                     // OCR2=n-1=125-1=124
	                    	                   


	TCCR0=0X0B;         
	                     // FOC0  WGM00  COM01  COM00  WGM01  CS02  CS01  CS00
	                     //   0     0      0      0      1      0     1     1 
	                     // 0X0B
	                    
	
	OCR0=124;            // n=(f-clock-cpu)*(t)/(N)=(8MHz)*(2ms)/(64)=250
	                     // OCR0=n-1=250-1=249
	                     
	TIMSK=0X82;         
	                     // OCIE2  TOIE2  TICIE1  OCIE1A  OCIE1B  TOIE1  OCIE0  TOIE0
	                     //   1      0      0       0       0       0      1      0 
	                     // 0X82
	                     
	SREG=0X80;          
	
	ADMUX=0X47;          //  REFS1  REFS0  ADLAR  MUX4  MUX3  MUX2  MUX1  MUX0
	                     //    0      1      0     0     0     1     1     1
	                     // 0X47
	                     // AVCC with ecternal capacitor at AREF pin 
	                     // Right Adjusted
	                     // Single Ended Input---ADC7
	
	ADCSRA=0XBD;         // ADEN  ADSC  ADATE  ADIF  ADIE  ADPS2  ADPS1  ADPS0
	                     //  1     0      1     1     1     1       0      1       
	                     // 0XBD
	                     // Enable the ADC
	                     // Auto Triggering
	                     // ADC Prescaler Select---Division Factor---32
	                     
	SFIOR=0X60;          // ADTS2  ADTS1  ADTS0  -  ACME  PUD  PSR2  PSR10
	                     //   0      1      1    0   0     0    0      0
	                     // ADC Auto Trigger Source---Timer/Counter0 Compare Match
	                     // 0X60

	
	while (1)
	{
	
			display();   
			
			keyscan();  
			

	}
}


#pragma interrupt_handler Timer2Time:4
void Timer2Time()
{   adopter=adopter+1;       
    if (adopter==125)     
   {
		if (++second==60)
		{
			second=0;
			minute=minute+1;
			if (minute>=60)
			{
			    minute=0;
				hour=hour+1;
				if (hour>=24)
				{
				    hour=0;
				}
			}
		}
    adopter=0;
   }
}



#pragma interrupt_handler Timer0_CTC:20
void Timer0_CTC ()
{
	
}


void Diode_Light()
{
	if ((adc_val>550)&&(adc_val)<=605)
	{
		PORTC=0XFE;    
		flag_b=0;
	}
	if ((adc_val>280)&&(adc_val<400))
	{
		PORTC=0XFD;    
		flag_b=1;
		
	}
	if (((adc_val>=0)&&(adc_val<=280))||((adc_val>=400)&&(adc_val<=550)))
	{
		PORTC=0XBF;    
		flag_b=2;
		
	}
}


#pragma interrupt_handler adc_isr:15
void adc_isr()
{
      adc_rel=ADCL;
      adc_rel|=(unsigned int)(ADCH<<8); 
      adc_val=(unsigned long)adc_rel*606/1024;   
      
}


void display()
{
	if (flag_a==0)   
	{
		PORTA=LED_Disbuf[second%10];
	    PORTB=position[0];
	    delayms(2);
	    PORTB=0XFF;
	
	    PORTA=LED_Disbuf[second/10];
 	    PORTB=position[1];
	    delayms(2);
	    PORTB=0XFF;
	    
	    PORTA=LED_Disbuf[minute%10];
 	    PORTB=position[4];
	    delayms(2);
	    PORTB=0XFF;
	    
	    PORTA=LED_Disbuf[minute/10];
 	    PORTB=position[5];
	    delayms(2);
	    PORTB=0XFF;
	    
	
	    PORTA=LED_Disbuf[hour%10];
	    PORTB=position[6];
	    delayms(2);
	    PORTB=0XFF;
	
	    PORTA=LED_Disbuf[hour/10];
	    PORTB=position[7];
	    delayms(2);
	    PORTB=0XFF;
	}
	
	
	if (flag_a==1)
	{
		PORTA=LED_Disbuf[11];
		PORTB=position[7];
		delayms(2);
		PORTB=0XFF;
		
		PORTA=LED_Disbuf[spa%10];
		PORTB=position[3];
		delayms(2);
		PORTB=0XFF;
		
		PORTA=LED_Disbuf[spa/10];
		PORTB=position[4];
		delayms(2);
		PORTB=0XFF;
		
		
		PORTA=LED_Disbuf[tpa%10];
		PORTB=position[0];
		delayms(2);
		PORTB=0XFF;
		
		PORTA=LED_Disbuf[tpa/10];
		PORTB=position[1];
		delayms(2);
		PORTB=0XFF;
			
	}
	
	
	
	if (flag_a==2)
	{
		PORTA=LED_Disbuf[10];
		PORTB=position[7];
		delayms(2);
     	PORTB=0XFF;
		
		
		PORTA=LED_Disbuf[spb%10];
		PORTB=position[3];
		delayms(2);
		PORTB=0XFF;
		
		PORTA=LED_Disbuf[spb/10];
		PORTB=position[4];
		delayms(2);
		PORTB=0XFF;
		
		
		PORTA=LED_Disbuf[tpb%10];
		PORTB=position[0];
		delayms(2);
		PORTB=0XFF;
		
		PORTA=LED_Disbuf[tpb/10];
		PORTB=position[1];
		delayms(2);
		PORTB=0XFF;		
	}
	
	
	
	if (flag_a==3)
	{
		
        Diode_Light();
		
		PORTA=LED_Disbuf[adc_val%10];
		PORTB=position[0];
		delayms(2);
		PORTB=0XFF;
		
		PORTA=LED_Disbuf[(adc_val/10)%10];
		PORTB=position[1];
		delayms(2);
		PORTB=0XFF;
		
		PORTA=LED_Disbuf[adc_val/100];
		PORTB=position[2];
		delayms(2);
		PORTB=0XFF;
		
                
			
	}
	
	
	if (flag_a==4)
	{
	    PORTC=0XFF;     
	
		PORTA=LED_Disbuf[11];
		PORTB=position[7];
		delayms(2);
     	PORTB=0XFF;
     	
     	PORTA=LED_Disbuf[10];
		PORTB=position[3];
		delayms(2);
     	PORTB=0XFF;
     	
     	PORTA=LED_Disbuf[tpa%10];
		PORTB=position[4];
		delayms(2);
     	PORTB=0XFF;
     	
     	PORTA=LED_Disbuf[tpa/10];
		PORTB=position[5];
		delayms(2);
     	PORTB=0XFF;
		
		PORTA=LED_Disbuf[tpb%10];
		PORTB=position[0];
		delayms(2);
     	PORTB=0XFF;
     	
     	PORTA=LED_Disbuf[tpb/10];
		PORTB=position[1];
		delayms(2);
     	PORTB=0XFF;
     	
     	
	}
	
	
	
		
}


void keyscan()
{
	if ((PIND&0XFF)!=0XFF)
	{
		delayms(20);               
		if ((PIND&0XFF)!=0XFF)
		{
			
		
		
			if (PIND==0XFE)          
			{
				flag_a=flag_a+1;
				if   (flag_a==5)
			       {
				     flag_a=0;
			       }
			}
			
			
			if (flag_a==0)
		{
			
			if (PIND==0XFD)       
			{
				if (++minute==60)
				{
				    minute=0;
					hour=hour+1;
					if(hour==24) hour=0;
				}
			}
			if (PIND==0XFB)      
			{
				if (++hour>=24) hour=0;
			}
	    }
			
			if (flag_a==1)
			{
				if (PIND==0XFD)  
				{
					spa=spa+2;
					if (spa==14) spa=2;
				}
				if (PIND==0XFB)  
				{
					tpa=0;
				}
			}
			
			if (flag_a==2)
			{
				if (PIND==0XFD)  
				{
					spb=spb+2;
					if (spb==16) spb=2;
				}
				if (PIND==0XFB) 
				{
					tpb=0;
				}
			}
			
			if ((flag_a==3)&&(flag_b==1))
			{
				
				
				if (PIND==0XFD) 
				{
					tpa=tpa+(spa/2);
					if (tpa>99) tpa=0;
				}

			    	
				if (PIND==0XFB) 
				{
					tpb=tpb+(spb/2);
					if (tpb>99) tpb=0;
				}
				
			}    
			    
			    if ((flag_a==3)&&(flag_b==0))
			{
				
				if (PIND==0XF7)  
			    	{
				    	tpa=tpa+spa;
				    	if (tpa>99) tpa=0;
				    }
			    if (PIND==0XEF)  
			    	{
				    	tpb=tpb+spb;
				    	if (tpb>99) tpb=0;
			    	}
			    	
			}
				
			
			if (flag_a==4)
			{
				if (PIND==0XFD)  
				{
					tpa=0;
				}
				if (PIND==0XFB)  
				{
					tpb=0;
				}
			}
		
		}
	}
	while ((PIND&0XFF)!=0XFF);
}










