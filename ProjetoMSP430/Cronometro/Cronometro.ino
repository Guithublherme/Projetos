#include <msp430g2553.h>
#include <legacymsp430.h>
#define BTN BIT3
#define SEG_A BIT0 //  AAAA
#define SEG_B BIT1 // F    B
#define SEG_C BIT2 // F    B
#define SEG_D BIT7 //  GGGG
#define SEG_E BIT6 // E    C
#define SEG_F BIT4 // E    C
#define SEG_G BIT5 //  DDDD

// Patterns for hexadecimal characters

#define zero SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F  // '0'
#define um SEG_B | SEG_C // '1'
#define dois SEG_A | SEG_B | SEG_D | SEG_E | SEG_G  // '2'
#define tres SEG_A | SEG_B | SEG_C | SEG_D | SEG_G  // '3'
#define quatro SEG_B | SEG_C | SEG_F | SEG_G  // '4'
#define cinco SEG_A | SEG_C | SEG_D | SEG_F | SEG_G  // '5'
#define seis SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G  // '6'
#define sete SEG_A | SEG_B | SEG_C  // '7'
#define oito SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G  // '8'
#define nove SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G  // '9'
#define reinicia  P2OUT=P1OUT &= ~(BIT0+BIT1+BIT2+BIT4+BIT5+BIT6+BIT7);


// VARIAVEIS GLOBAIS
 volatile int DECSEG=0;
 volatile  int SEG_UNI=0;
 volatile int SEG_DEZ=0;
 volatile int SEG_CEN=0;
 volatile int i=0;
  int mostra;

void DELAY()
{
volatile unsigned long int i;
for(i=0;i<10000;i++) { }
}
int DISPLAY( int SEGs){
  
int display;
switch(SEGs)   //Bits correspondetes a cada nuemro
{
case 0 :
display = (BIT0 + BIT1 + BIT2 + BIT4 + BIT5 + BIT6);
break;
case 1 :
display = (BIT1 + BIT2);
break;
case 2:
display = (BIT0 +BIT1 + BIT7 + BIT5 +BIT4);
break;
case 3:
display = (BIT0 + BIT1 + BIT7 + BIT2 + BIT4);
break;
case 4:
display = (BIT6 + BIT7 + BIT1 + BIT2);
break;
case 5:
display = (BIT0 + BIT6 + BIT7 + BIT2 + BIT4 );
break;
case 6:
display = (BIT0 + BIT6 + BIT2 + BIT4 + BIT5 + BIT7);
break;
case 7:
display = (BIT0 + BIT1 + BIT2);
break;
case 8:
display = (BIT0 + BIT1 + BIT2 + BIT4 + BIT5 + BIT6 + BIT7);
break;
case 9:
display = (BIT0 + BIT1 + BIT2 + BIT6 + BIT7 + BIT4 );
break;
}
return display;


       
}

int main(){
  
  
     WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
        P1SEL  = 0; // Utilizar pinos para a porta P1
        P1SEL2 = 0; // Utilizar pinos para a porta P1
        P1REN  |= BTN; // Habilitar resistor de pull-up/down no pino do botão
        P1OUT  |= BTN; 
    P1DIR = BIT0+BIT1+BIT2+BIT4+BIT5+BIT6+BIT7; //SAÍDAS
    P2DIR = BIT0+BIT1+BIT2+BIT4; //SAÍDAS
  

    BCSCTL1=CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ; // DEFINE MCLK E SMCLK COMO 1 MHZ
 
    TA1CCR0= 6250; //  6250*2*8uS= 0,1s 
    TA1CTL = TASSEL_2 + MC_3 + ID_3; // SMCLK  ,  UP/DOWN MODE, DIV8
    //TA1CCR0= 62500; //  62500*2*8uS= 1s 
    //TA1CTL = TASSEL_2 + MC_3 + ID_3+ TAIE;
    
    TA1CCR0 = 2500-1;  // 10000*1uS= 0,010S = 10 mS= 100Hz
    TA1CTL = TASSEL_2 + ID_0 + MC_1 + TAIE; // SMCLK  ,  UP MODE, DIV1, habilita interrupção

  
   //_BIS_SR(LPM4_bits+GIE); //DEIXAR EM MODO DE BAIXO CONSUMO ENQUANTO NÃO LIGAR O CRONÔMETRO
           int i=0;
        
        for(;;){       
           _BIS_SR(GIE);
           
           if ((P1IN&BIT3)==0)  { 
                       
           if((i%2)==0){ TA1CTL = TASSEL_2 + MC_3 + ID_3+ TAIE;}
           if ((i%2)!=0){TA1CTL = TASSEL_2 + MC_0 + ID_3+ TAIE; }
           i++;
           DELAY();}
                 } 
        return 0;  
 
}

/*
// interrupção tira do modo de baixo consumo e começa a contagem

interrupt(PORT1_VECTOR)P1_ISR( void ) // Formato de interrupção para o gcc para MSP430
{ 
   //LPM4_EXIT;   // AO RETORNAR PARA O main(), NÃO HABILITAR LPM4, E INICIAR A CONTAGEM
  P1OUT^=0xFF;
   P1IFG =0;
 
 }


 
// INTERRUPÇÃO QUE PAUSA A CONTAGEM UTILIZANDO BIT5
interrupt(PORT2_VECTOR) P2_ISR(void)
{      
  for(;;)
  {
  TA0CTL =  MC_0;  //STOP MODE
  DELAY(DLY); // DEBOUNCE
    if ((P1IN&BIT3)==0)    P2IFG =0;  // SE O BOTÃO FOR PRESSIONADO NOVAMENTE SAI DA INTERRUPÇÃO E DO STOP MODE
    
  }
   
}*/
 
 
 
 
 // interrupção a cada 0,1 segundos 
 interrupt(TIMER1_A1_VECTOR) TA1_ISR(void)     // verificar se é TIMER0_A0 ou como estava no exemplo  TIMER0_A1
{  
            if(DECSEG<10) DECSEG++; // INCREMENTA O DÉCIMO DE SEGUNDO A CADA CONTAGEM
                        
            if(DECSEG==10)  //CASO DÉCIMO DE SEG =10 ZERA  E INCREMENTA UNIDADE
            { 
              DECSEG=0;
              SEG_UNI++;
            
            if(SEG_UNI==10)  //CASO UNIDADE =10 ZERA UNIDADE E INCREMENTA DEZENA
            { 
              SEG_UNI=0;
              SEG_DEZ++;
            
             if(SEG_DEZ==10)  //CASO DEZENA =10 ZERA DEZENA E INCREMENTA CENTENA
            { 
              SEG_DEZ=0;
              SEG_CEN++;
            }}}
    
  
  TA1CTL &= ~TAIFG;
}

 
 
 
// Interrupção chamada a cada 10 ms=100Hz mostra os leds 
interrupt(TIMER0_A1_VECTOR) TA0_ISR(void)
{  
   
                    //quando timer contar 1 / 100 vezes, temos 0.01s(100Hz)/1s(1Hz)
                  //com clock a 1MHz
                  //basta colocar 100 depois do % para 1 Hz
                  //e 1 para 100 Hz
         
  
               
   if (mostra==0){
  P2OUT=0x0E; // 1110 -> 14 -> E SELECIONA O PRIMEIRO DISPLAY
  
  P1OUT= DISPLAY(DECSEG);
  
  
  }
  if(mostra==1){
  P2OUT=0x0D; //1101->13->D SELECIONA O SEGUNDO DISPLAY
  P1OUT= DISPLAY(SEG_UNI);
  ;
}
  if (mostra==2){
  P2OUT=0x0B; //1011->11->B SELECIONA O TERCEIRO DISPLAY
  P1OUT= DISPLAY(SEG_DEZ);
  }
  
//  if (mostra==3){
//  P2OUT=0x07; //0111->7 SELECIONA O QUARTO DISPLAY
 // P1OUT= DISPLAY(SEG_CEN); 
 // }
  
  mostra++;
          if (mostra==4){
                    mostra = 0;
                 }    
               
  
  TA0CTL &= ~TAIFG;
    
}
  
