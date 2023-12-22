void encrypt(void);
void myini(void);
char CNT0;    
char CNT1;   
char CNT2;  
 
char CSR0;       	
char CSR1;       
char CSR2;        
char CSR3;   

char KEY0;	
char KEY1;	
char KEY2;	
char KEY3;	
char KEY4;	
char KEY5;	
char KEY6;	
char KEY7;	

char MASK;

void main()
{
myini();
encrypt();
NOP();
NOP();
}
//Test Conditions: 
//Encryption Key (KEY7:0) - 0x0516FBE989074278
//Data To Be Encrypted (CSR3:0) - 0x04030201
//Results After Encryption - (CSR3:0) - 0x19772302
void myini(void)
{
//Encryption Key (KEY7:0)- 0x0516FBE989074278
KEY0 = 0x78;
KEY1 = 0x42;
KEY2 = 0x07;
KEY3 = 0x89;
KEY4 = 0xe9;
KEY5 = 0xfb;
KEY6 = 0x16;
KEY7 = 0x05;
//Data to Encrypt (CSR3:0) = 0x04030201
CSR0 = 0x01;
CSR1 = 0x02;
CSR2 = 0x03;
CSR3 = 0x04;	
}

void encrypt(void)
{
#asm
ENCRYPT
	MOVLW   0xC
	MOVWF   _CNT1	

ENCRYPT_OUTER
	MOVLW   0x30	
	MOVWF   _CNT2	

ENCRYPT_INNER
	CLRWDT
	MOVF	_CNT1,W
	XORLW   1	
	BTFSC 0x3, 0x2
	GOTO    RRF_KEY
							
	BCF 0x3, 0
	MOVLW   1
	BTFSC   _CSR2,4
	MOVLW   0x10
	MOVWF   _MASK

	BTFSS   _CSR1,1
	GOTO    $+3
	RLF     _MASK,F
	RLF     _MASK,F           

	BTFSC   _CSR0,1
	RLF     _MASK,F

NFL_TABLE
	BTFSC   _CSR3,7
		GOTO	TABLE2
	MOVLW   0x2E			
	BTFSC   _CSR3,2
	MOVLW   0x74			
		GOTO	TABLE_END
TABLE2
	MOVLW   0x5C			
	BTFSC   _CSR3,2
	MOVLW   0x3A			

TABLE_END
	ANDWF   _MASK,F		
	MOVLW   0			
	BTFSS 0x3, 0x2
	MOVLW   0x80		
                                 
	MOVWF   _MASK		
	RLF     _MASK,F		
	RLF     _MASK,F		
	MOVF	_MASK,W

	XORWF   _CSR0,W
	XORWF   _CSR2,W
	XORWF   _KEY0,W

	MOVWF   _MASK	
	RRF     _MASK,F	

	RRF     _CSR3,F	
	RRF     _CSR2,F
	RRF     _CSR1,F
	RRF     _CSR0,F

RRF_KEY
	BCF 0x3, 0
	BTFSC   _KEY0,0		
	BSF 0x3, 0

	RRF     _KEY7,F	
	RRF     _KEY6,F
	RRF     _KEY5,F
	RRF     _KEY4,F
	RRF     _KEY3,F
	RRF     _KEY2,F
	RRF     _KEY1,F
	RRF     _KEY0,F        

	DECFSZ  _CNT2,F	
	GOTO    ENCRYPT_INNER	

	DECFSZ  _CNT1,F		
	GOTO    ENCRYPT_OUTER
#endasm
}