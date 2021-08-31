#pragma once
#include "board.h"
#include <HardwareTimer.h>
#include "variable_queue.h"


class Board_Stm32f103rct6 : public Board{
public:
   Board_Stm32f103rct6();
   ~Board_Stm32f103rct6();

   void enable_irq();
   void disable_irq();
   void usart_debug_init();
   void usart_init(unsigned char num, unsigned long buad);
   Queue* usart_getDataQueue(unsigned char num);

   void usart_write(unsigned char num, unsigned char ch);
   void usart_write(unsigned char num, unsigned char* data, unsigned char len);

   void set_config(unsigned char* data, unsigned short len);
   void set_config(int data);

   void get_config(unsigned char* data, unsigned short len);
   int get_config();

   void pin_init(unsigned char pin, unsigned char mode);
   void pin_write(unsigned char pin, unsigned char level);
   unsigned char pin_read(unsigned char pin);

   unsigned char pin_analog_read(unsigned char pin);
   void pin_analog_write(unsigned char pin, unsigned char level);

   void pwm_init(unsigned char khz);
   void pwm_init(unsigned char pin, unsigned char hz);

   void pwm_output(unsigned char pin, unsigned short pwm);

   unsigned long get_tick_count();
private:
public:
   static Board_Stm32f103rct6 board;  

   VQueue<256> usart1_queue; 

};