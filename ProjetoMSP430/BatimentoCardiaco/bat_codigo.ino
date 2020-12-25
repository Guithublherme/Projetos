#include <msp430g2553.h>
#include <legacymsp430.h>
#include <stdio.h>
#define TX BIT2
int x=0;
int milisegundos;
int decsegundos=0;
int segundos=30;
int minutos=47;
int horas=15;

int batimentos;
long sensor;
long sensor1;
long sensor2;
char txbuf[256];

void uart_send(int len) {
  int i;
  for(i = 0; i < len; i++) {
    while (UCA0STAT & UCBUSY);
    UCA0TXBUF = txbuf[i];
  }
}


int main(void)
{
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;                     // clock 1mhz
  WDTCTL = WDTPW + WDTHOLD;                 // para WDT
  
  CCTL0 = CCIE;                             // CCR0 interrupt habilitada
  TACCR0 = 1000;				    // 1ms 
  TA0CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
////HORAS////
  TA1CCR0= 6250; //  6250*2*8uS= 0,1s 
  TA1CTL = TASSEL_2 + MC_3 + ID_3 +TAIE; // SMCLK  ,  UP/DOWN MODE, /8
/////////////
  P1IFG  = 0x00;			    //limpa flags de interrupção
  P1DIR |= 0x01;                            // P1.0 como saida pro LED
  P1OUT &= ~0x01;                           // apaga o led
  
  _BIS_SR(GIE);                 	    // global interrupt habilitada
 
 while(1){
        P1SEL2 = P1SEL = TX;
	P1IE &= ~0x01;			// desabilita interupt
	P1DIR |= 0x02;			// trigger como output
	P1OUT |= 0x02;			// gera pulse
//////////////////////////////////////////
        // UART: desabilitar paridade, transmitir um byte
        // na ordem LSB->MSB, um bit de stop
        UCA0CTL0 = 0;
        // UART: utilizar SMCLK
        UCA0CTL1 = UCSSEL_2;
        // Baud rate de 9600 bps, utilizando oversampling
        UCA0BR0 = 6;
        UCA0BR1 = 0;
        UCA0MCTL = UCBRF_8 + UCOS16;


	P1OUT &= ~0x02;                 // para pulso
  	P1DIR &= ~0x08;			// torna o pin P1.3 entrada 
        P1IFG = 0x00;                   // limpa flag se nada acontecer antes
	P1IE |= 0x08;			// habilita interupção   
	P1IES &= ~0x08;			// na borda de subida
        
       
_BIS_SR ( LPM0_bits ) ; // modo de baixo consumo 0 ativado     
        
        
       if(sensor>=2)
       {   
           P1OUT |= 0x01;  //liga o led indicando situação de perigo
           uart_send(sprintf(txbuf," Enviar Ambulância"));
          if(sensor1>=2)  uart_send(sprintf(txbuf," BATIMENTOS:%d - BATIMENTO ACELERADO  %.2d:%.2d:%.2d\n",sensor1,horas, minutos,segundos));
          else   uart_send(sprintf(txbuf," BATIMENTOS:%d - BATIMENTO LENTO %.2d:%.2d:%.2d\n",sensor2,horas, minutos,segundos));
    
       
       }
   
      else P1OUT &= ~0x01;
        
       
     

        
 }
}
 //interrupção chamada a cada borda de subida detectada, cada pico do sinal
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IFG&0x08)  //verifica se tem alguma interrupção pendente
        {
          if(!(P1IES&0x08)) // verifica se é borda de subida
          {	
            batimentos++;
          }
          
          }
	P1IFG &= ~0x08;				//limpa flag
	
}

// INTERRUPÇÃO QUE CONTA A CADA 0,1 SEGUNDOS 
interrupt(TIMER1_A1_VECTOR) TA1_ISR(void)
{
 
  decsegundos++;
  
      if(decsegundos>=10) {
      decsegundos=0;
      segundos++;}
 
            if(segundos>=60) {
              minutos++; // INCREMENTA OS MINUTOS
                // verifica o batimento e zera a variável para continuar fazendo a contagem
                
                if (batimentos>130)
                {
                sensor++; // indica batimentos instáveis 
                sensor1++; // indica batimento acelerado
                }
                if (batimentos<50)
               {
               sensor++; // indica batimentos instáveis 
               sensor2++;// indica fracos batimentos
               }
                
                else
                {
                 sensor=0;
                }
                
              segundos=0;  
              batimentos=0; // a cada 1 minuto zera a variável 
          }
            if(minutos>=60)  // INCREMENTA AS HORAS
            { 
              minutos=0;
              horas++;
            }
            if(horas>=24)  //CASO UNIDADE =10 ZERA UNIDADE E INCREMENTA DEZENA
            { 
              horas=0;
              
            }
    
  
  TA1CTL &= ~TAIFG;
}

	


