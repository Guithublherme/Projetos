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

int distancia;
long sensor;
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
//////////////////////////////////////////
	__delay_cycles(10);             //  10us
	P1OUT &= ~0x02;                 // para pulso
  	P1DIR &= ~0x08;			// torna o pin P1.3 entrada (ECHO)
        P1IFG = 0x00;                   // limpa flag se nada acontecer antes
	P1IE |= 0x08;			// habilita interupção  pino ECHO 
	P1IES &= ~0x08;			// na borda de subida
        __delay_cycles(30000);          // delay por 30ms (after this time echo times out if there is no object detected)
        distancia = sensor/58;           // converte ECHO em cm
       
        while (x==0)  {  uart_send(sprintf(txbuf," %d mensagens\n",x) );x++;}
       
        if(distancia > 35 && distancia != 0){
        x=0;
         for(int j=0;j<150;j++) //aguarda 15 segundos depois q abriu a caixa
      {
      __delay_cycles(100000);
      }
        }
        
        
       if(distancia < 15 && distancia != 0)
       { P1OUT |= 0x01;  //liga o led indicando que chegou a carta
         
          if(x==1)  uart_send(sprintf(txbuf," %d mensagem recebida %.2d:%.2d:%.2d\n",x,horas, minutos,segundos));
          if(x==10) uart_send(sprintf(txbuf," Verificar correio\n"));
          if(x<10&&x!=1) uart_send(sprintf(txbuf," %d mensagens recebida %.2d:%.2d:%.2d\n",x,horas, minutos,segundos));
                    
    
          
      x++;
      
      for(int j=0;j<30;j++)
      {
      __delay_cycles(100000);
      }
       }
   
      else P1OUT &= ~0x01;
        
       
     

        
 }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IFG&0x08)  //verifica se tem alguma interrupção pendente
        {
          if(!(P1IES&0x08)) // verifica se é borda de subida
          {	
            TA0CTL|=TACLR;   // limpa o timer A0
            milisegundos = 0;
            P1IES |= 0x08;  //borda de descida
          }
          else
          {
            sensor = (long)milisegundos*1000 + (long)TAR;	//calcula distancia do ECHO

          }
	P1IFG &= ~0x08;				//limpa flag
	}
}

interrupt(TIMER0_A0_VECTOR) TA0_ISR(void)
{
  milisegundos++;
  
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
              segundos=0;  
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

	


