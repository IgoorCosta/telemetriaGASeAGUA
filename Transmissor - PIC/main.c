#include <16F627A.h>

#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(crystal=8MHz)
#use rs232(baud=1200,parity=N,xmit=PIN_B2,rcv=PIN_B1,bits=8,stream=PORT1)

#use FIXED_IO( A_outputs=PIN_A7,PIN_A6,PIN_A5,PIN_A4,PIN_A3,PIN_A2,PIN_A1,PIN_A0 )
#use FIXED_IO( B_outputs=PIN_B7,PIN_B6,PIN_B5,PIN_B4,PIN_B3,PIN_B2,PIN_B1 )


const char * ip = "M0001";
int consumo = 0;
char sistema = '1';
int high = 0;
int low = 0;

#INT_EXT
void  EXT_isr(void) 
{
   clear_interrupt(INT_EXT);
   enable_interrupts(INT_EXT);
}

void RF_Send(char* ip, int consumo, char sistema)
{ 
     printf("%sC%07xS%cF", ip, consumo, sistema);
     delay_ms(1);
     return;
}

void main()
{      
   ext_int_edge(L_TO_H);
   clear_interrupt(INT_EXT);
   enable_interrupts(INT_EXT);
   delay_ms(10);
   
   output_low(PIN_B0);
   output_low(PIN_B1);
   output_low(PIN_B2);
   output_low(PIN_B3);
   output_low(PIN_B4);
   output_low(PIN_B5);
   output_low(PIN_B6);
   output_low(PIN_B7);
   output_low(PIN_A0);
   output_low(PIN_A1);
   output_low(PIN_A2);
   output_low(PIN_A3);
   output_low(PIN_A4);
   output_low(PIN_A5);
   output_low(PIN_A6);
   output_low(PIN_A7);
   delay_ms(10);
   
   for(int x = 0; x < 75; x++) {
      RF_Send(ip, consumo, sistema);
   }
   
   while(TRUE)
   {  
      if(input_state(pin_B0) == 1) {
         high++;
      }
      else {
         low++;
      }
      
      if(high > 60 && low < 20) {
         consumo++;
         high = 0;
         low = 0;
         for(int x = 0; x < 75; x++) {
            RF_Send(ip, consumo, sistema);
         }
         RF_Send(ip, consumo, sistema);
         if(input_state(pin_B0) == 1) {
            ext_int_edge(H_TO_L);
         }
         else {
            ext_int_edge(L_TO_H);
         }
         clear_interrupt(INT_EXT);
         enable_interrupts(INT_EXT);
         sleep();
      }
      
      if(high > 80 && low > 20) {
         high = 0;
         low = 0;
         RF_Send(ip, consumo, sistema);
         ext_int_edge(H_TO_L);
         clear_interrupt(INT_EXT);
         enable_interrupts(INT_EXT);
         sleep();
      }
      
      if(low > 50) {
         high = 0;
         low = 0;
         RF_Send(ip, consumo, sistema);
         ext_int_edge(L_TO_H);
         clear_interrupt(INT_EXT);
         enable_interrupts(INT_EXT);
         sleep();
      }
      delay_ms(10);
   }
}
