/*
 * HCS200_decoder.c
 *
 * Created: 06.07.2018 22:53:34
 * Author : w666m
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/delay.h>


#define vect_VCC TCCR1B |= (1 << ICES1)			// ������������� ���������� �� ������������ ������ ��������
#define vect_GND TCCR1B &= ~(1 << ICES1)		// ������������� ���������� �� ���������� ������ ��������
#define vect_INVERT TCCR1B ^=(1 << ICES1)		// ����������� ������ ����������
#define detect_vect (TCCR1B&(1 << ICES1))		// ��������� ������� ���������� 0-high 1-low
#define HIGH_interupt 0  
//�������� ID  ���� ������� �� ������������              
#define control_ID 0x0003281
//#define control_ID 0xFFF9080
//#define control_ID 0xFFFF8A0

#define _9600_kbps 103
#define _19200_kbps 51
#define _38400_kbps 25
#define _76800_kbps 10
#define _250000_kbps 3

uint8_t RF_bufer[66];
uint16_t his_ICR1, new_ICR1, CycleCount;
uint8_t Preamble_count=0, button_code;
volatile uint8_t reciver_full;
uint8_t HCS_bit_counter;                // ������� ��������� ��� ������
uint32_t read_ID;

void UART_init(void)
{
	DDRD|=(1<<1);
	PORTD|=(1<<1)|(1<<0);
	UBRRH=0;
	UBRRL=_38400_kbps;				  //38400
	UCSRC|=(1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
	UCSRB|=(1<<TXEN);
}

void transmit_UART(unsigned char byte)
{
	while((UCSRA&(1<<UDRE))==0);
	UDR = byte;
}


void KEELOQ_init(void)
{
	TCCR1B |= (0 << ICNC1)|(1 << CS11)|(1 << ICES1); // ���������� ������� ���������� ����, ������������ �� 8, ������������� ���������� �� ������������ ������ ��������
	TIMSK |= (1 << TICIE1)|(1<<TOIE0); // ��������� ���������� �� �������// � ������������|(1 << TOIE1)
	sei();
}

// ���������� �� ������� �������/�������� 1
ISR(TIMER1_CAPT_vect)
{
	/*	���� �� 65535 16MHz �������� �� 2 = 1 count=0,5us max count=32767,5us	*/
	his_ICR1 = new_ICR1;
	new_ICR1 = ICR1;
	vect_INVERT;
	TCNT0 = 0;
	CycleCount = new_ICR1 - his_ICR1;
	/*Preamble detect*/
	if (Preamble_count<12)
	{
		if (detect_vect == HIGH_interupt)
		{
			if (((CycleCount > 500) && (CycleCount < 1000)) || Preamble_count == 0){} 
			else
			{
				Preamble_count = 0;
				return;
			}
		} 
		else
		{
			// ����� �������� ���������
			if((CycleCount > 500) && (CycleCount < 1000))
			{
				// ������� ������� ���������
				Preamble_count ++;
				if(Preamble_count == 12)
				{
					// ������� ���������
					HCS_bit_counter = 0;
					return;
				}
			}
			else
			{
				// ������� ����� �� �����
				Preamble_count = 0; // ���������� ������� ��������� ��������� ���������
				return;
			}
		}
	}
	/* �������� 64 bit ������ */
	if(Preamble_count == 12)
	{
		if(detect_vect == HIGH_interupt)
		{
			if(((CycleCount > 600) && (CycleCount < 1800)) || HCS_bit_counter == 0){}
			else
			{
				// ������������ ����� ����� ����������
				CycleCount = 0;
				return;
			}
		}
		else
		{
			// ����� �������� ������
			if((CycleCount > 600) && (CycleCount < 1800))
			{
				RF_bufer[65 - HCS_bit_counter] = (CycleCount > 1000) ? 0 : 1; // ������� ������, ��� �������� �� �� * 3 ������� 0, ����� 1
				HCS_bit_counter++;
				if(HCS_bit_counter == 66)
				{
					/* ������� ��� 66 ��� */
					reciver_full = 1;
					Preamble_count = 0; // ���������� ������� ���������
					//���������� ID �����������
					for(uint8_t i = 6; i < 34; i++)
					{
						if (RF_bufer[i] == 1)
						{
							read_ID |= (1<<(i-6));
						} 
						else
						{
							read_ID &=~(1<<(i-6));
						}
					}
					//������������� ������ �����������
					if ((read_ID&0xFFFFFFF) == (control_ID&0xFFFFFFF))
					{
						for (uint8_t i = 2; i < 6; i++)
						{
							if (RF_bufer[i] == 1)
							{
								button_code |= (1<<(i-2));
							}
							else
							{
								button_code &=~(1<<(i-2));
							}
						}
						PORTC = button_code;
					}
				}
			}
			else
			{
				/* ���� ������ */
				Preamble_count = 0;
				return;
			}
		}	
	}
}

int main(void)
{
    /* Replace with your application code */
	DDRC|=(1<<0)|(1<<1)|(1<<2)|(1<<3);//������ ����� ���� ������ � ���� PORTC0..3
	UART_init();
	KEELOQ_init();
    while (1) 
    {
		if (reciver_full == 1)
		{
			button_code = 0;
			transmit_UART((uint8_t)((read_ID>>24)&0x0F));
			transmit_UART((uint8_t)((read_ID>>16)&0xFF));
			transmit_UART((uint8_t)((read_ID>>8)&0xFF));
			transmit_UART((uint8_t)(read_ID&0xFF));
			read_ID = 0;
			reciver_full = 0;
		}
    }
}