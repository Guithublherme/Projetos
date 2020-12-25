//////////////////////////////////////PINAGEM////////////////////////////////////////////

    
/*             | 
          BIT0 v
           _____                                            
          |     |                                      
   BIT5-> |_____| <- BIT1                            
          |BIT6^| 
   BIT4-> |_____| <- BIT2                               
          
          BIT0 ^ 
               | 
 
 
 */

///////////////////////////////////////////////////////////////////////////////////////////

#include <msp430g2553.h>
#include <legacymsp430.h>
#define BTN  BIT3
#define DLY 19000
 
void DELAY(volatile unsigned long int t)
{
        volatile unsigned long int i;
        for(i=0;i<t;i++) { }
}

 

void numface(int num)
{ __asm__(         "numface:\n"   
                   "mov.w r15,r11");  // DE ACORDO COM A COMPILAÇÃO A VARIAVEL NUM RECEBE O VALOR DE R11
       
       // LÓGICA PARA LIGAR OS LEDS DEPENDENDO DO VALOR DA FACE DO DADO
                                                                                                                                                                                                          
      if (num==1){
                 P1OUT|= BIT1+ BIT2 ;  
                 DELAY(DLY);
                  __asm__("call #RETURN2");
                
      }

    
     if (num==2){
                 P1OUT|= BIT0 + BIT1 + BIT6 + BIT4 ;  
                 DELAY(DLY);
               __asm__("call #RETURN2");
               
     }
     
     if (num==3){
                 P1OUT|= BIT0 + BIT1 + BIT6 + BIT2;  
                 DELAY(DLY);
                __asm__("call #RETURN2");
                
     }

     if (num==4){
                 P1OUT|= BIT1+ BIT2 + BIT5 + BIT6;  
                 DELAY(DLY);
                 __asm__("call #RETURN2");
                  
     }
        
     if (num==5){  
                 P1OUT|= BIT0+ BIT2 + BIT5 + BIT6;  
                 DELAY(DLY);
                 __asm__("call #RETURN2");
                 
     }
  
     if (num==6){                 
                 P1OUT|= BIT0+ BIT2 + BIT4 + BIT5 + BIT6 ;  
                 DELAY(DLY);
                __asm__("call #RETURN2");  
                
     }  
  else __asm__("call #RETURN2");  

}


int main (void)
{
        WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
        P1SEL  = 0;                     // Utilizar pinos para a porta P1
        P1SEL2 = 0;                     // Utilizar pinos para a porta P1
        P1REN  = BTN;           // Habilitar resistor de pull-up/down no pino do botão
        P1OUT  =  BTN;            // LED2 on, Resistor do botão conectado a VCC
        P1DIR  = (BIT0 + BIT1 + BIT2 + BIT4 + BIT5 + BIT6);    // setar pinos > saídas
        P1DIR  = ~(BTN); // entrada
        P1IES = BTN; // Configurar interrupção no botão por borda de descida
        P1IE = BTN ; // Habilitar interrupção via botão
 
 
 
   _BIS_SR ( LPM4_bits + GIE ) ;
                
 
           	     
 __asm__( "RETURN:\n" 
	            
		//if (1)
	 ".BTN1: \n "
	"mov.b &__P1IN, r15 \n "
	 "and.b #8, r15 \n "
	 "jz .BTN2 \n "
	 "mov.b #1, r15 \n "
	 "call #numface \n "
		//if (2)
	".BTN2: \n "
	 "mov.b &__P1IN, r15 \n "
	 "and.b #8, r15 \n "
	 "jz .BTN3 \n "
	 "mov.b #2, r15 \n "
	 "call #numface \n "
		//if (3)
	".BTN3: \n "
	 "mov.b &__P1IN, r15 \n "
	 "and.b #8, r15 \n "
	 "jz .BTN4 \n "
	 "mov.b #3, r15 \n "
	 "call #numface \n "
		//if (4)
	".BTN4: \n "
	 "mov.b &__P1IN, r15 \n "
	 "and.b #8, r15 \n "
	 "jz .BTN5 \n "
	 "mov.b #4, r15 \n "
	 "call #numface \n "
		//if (5)
	".BTN5: \n "
	 "mov.b &__P1IN, r15 \n "
	 "and.b #8, r15 \n "
	 "jz .BTN6 \n "
	 "mov.b #5, r15 \n "
	 "call #numface \n "
		//if (6)
	".BTN6: \n "
	 "mov.b &__P1IN, r15 \n "
	 "and.b #8, r15 \n "
	 "jz .BTN1 \n "
	 "mov.b #6, r15 \n "
	 "call #numface \n "	
		//ELSE
	 "jmp .BTN1" ) ; 
               
       
    
      
        
}




interrupt ( PORT1_VECTOR ) P1_ISR ( void ) // Formato de interrupção para o gcc para MSP430
{ 
  __asm__("call #RETURN");
    __asm__("RETURN2:");

 P1OUT &= ~((BIT0+BIT1+BIT2+BIT4+BIT5+BIT6));
   
  P1IFG =0;// Apagar flag de interrupção
 
 }

