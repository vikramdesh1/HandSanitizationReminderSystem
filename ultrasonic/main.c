#include <msp430g2553.h>

long counter=0;
//unsigned int counter2=0;
int flag=1;
int state=0,timer=0;

void clockset(void);
void UARTsendnumber(int value);
void UARTinit(void);

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    clockset();
    UARTinit();
	P1DIR|=BIT5+BIT6+BIT0;
	P2DIR&=~BIT2;
	P2IES|=BIT2;
	P2IE|=BIT2;
	P1OUT&=~BIT5;
	__delay_cycles(100000);
	_bis_SR_register(GIE);
	while(1)
	{
	__delay_cycles(1000);
	P1OUT|=BIT5;	// 1
	__delay_cycles(300);
	P1OUT&=~BIT5;	// 0
	long temp=0;
	flag=1;
	while(flag&&temp<1073741824)
	temp++;
	counter=temp;
	//UARTsendnumber(counter);
	__delay_cycles(2000000);
		if(state==0)
		{
			P1OUT&=~(BIT6+BIT0);
			if(counter<5500)
				{
					state=1;
					//P1OUT|=BIT6;
				}
		}
		else if(state==1)
		{
			P1OUT^=BIT0;
			if(counter<2000||timer>200)
				{
				state=2;
				timer=0;
				P1OUT&=~BIT0;
				}
			timer++;
		}
		else if(state==2)
		{
			P1OUT^=BIT6;
			if(counter>5500)
			{
				timer++;
				if(timer>=15)
				{
					timer=0;
					state=3;
					P1OUT|=BIT0+BIT6;
				}
			}
		}
		else
		{
			P1OUT^=(BIT0+BIT6);
			timer++;
			if(timer>=15)
			{
				state=0;
				timer=0;
				P1OUT&=~(BIT0+BIT6);
			}
		}
	}
}

void clockset(void)
{
	DCOCTL = CALDCO_16MHZ;   //DCO==16MHz  sine wave
	BCSCTL1 = CALBC1_16MHZ;
}
void UARTinit(void)
{
	UCA0CTL1 = UCSWRST;
	P1SEL |= BIT1 + BIT2;                      // P1.1 RXD, P1.2 TXD
	P1SEL2 |= BIT1 + BIT2;                     // P1.1  RXD, P1.2 TXD
	UCA0CTL1 |= UCSSEL_2;                     // select SMCLK
	UCA0BR0 = 0x82;                            // 16,000,000hz 115200 139
	UCA0BR1 = 6;
	UCA0CTL1 &= ~UCSWRST;                     // SWRST=1 then configure, SWRST=0 after configuration
	//IE2 |= UCA0RXIE;                          // enable interrupt
}
void UARTsendnumber(int value)
{
	char buffer[9];
	unsigned int i;
	if(value<0)
	{
		buffer[0]='-';
		value=(-1)*value;
	}
	else
		buffer[0]='+';
		buffer[1]=('0'+((value/100000)%10));
		buffer[2]=('0'+((value/10000)%10));
		buffer[3]=('0'+((value/1000)%10));
		buffer[4]=('0'+((value/100)%10));
		buffer[5]=('0'+((value/10)%10));
		buffer[6]=('0'+((value/1)%10));
		buffer[7]=10;
		buffer[8]=13;
	for(i=0;i<9;i++)
	{
		while (!(IFG2&UCA0TXIFG));
		UCA0TXBUF=buffer[i];
	}
}
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR(void)
{
	P2IFG=0;
	flag=0;
}
