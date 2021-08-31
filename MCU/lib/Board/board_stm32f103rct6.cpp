#include <Arduino.h>
#include <EEPROM.h>
#include "board_stm32f103rct6.h"


Board_Stm32f103rct6 Board_Stm32f103rct6::board;

#define CONFIG_EEPROM_BASE 0

Board* Board::get(){
   return &Board_Stm32f103rct6::board;
}

Board_Stm32f103rct6::Board_Stm32f103rct6(){

}

Board_Stm32f103rct6::~Board_Stm32f103rct6(){

}

void Board_Stm32f103rct6::enable_irq(){
   interrupts();
}

void Board_Stm32f103rct6::disable_irq(){
   noInterrupts();
}

void Board_Stm32f103rct6::usart_debug_init(){

}

void Board_Stm32f103rct6::usart_init(unsigned char num, unsigned long buad){
   if(num == (unsigned char)USART_1)
      Serial1.begin(buad);
}

Queue* Board_Stm32f103rct6::usart_getDataQueue(unsigned char num){
   if(num == (unsigned char)USART_1){
      return &usart1_queue;
   }
   return 0;
}

void Board_Stm32f103rct6::usart_write(unsigned char num, unsigned char ch){
   if(num == (unsigned char)USART_1) 
      Serial1.write(ch);
}

void Board_Stm32f103rct6::usart_write(unsigned char num, unsigned char* data, unsigned char len){
   if (num == (unsigned char)USART_1) {
      Serial1.write((char*)data,len);
   }
}

void serialEvent1(){
   if(Serial1.available()){
      if(!Board::get()->usart_getDataQueue(USART_1)->put(Serial1.read())){
         //err
      }
   }
}

void Board_Stm32f103rct6::set_config(unsigned char* data,unsigned short len){
   for(unsigned short i=0;i<len;i++){
      EEPROM.write(CONFIG_EEPROM_BASE+i,data[i]);
      delayMicroseconds(2);
   }
}

void Board_Stm32f103rct6::set_config(int data){
   
   EEPROM.write(CONFIG_EEPROM_BASE,data);
   delayMicroseconds(2);
}

void Board_Stm32f103rct6::get_config(unsigned char* data,unsigned short len){
   for(unsigned short i=0;i<len;i++){
      data[i] = EEPROM.read(CONFIG_EEPROM_BASE+i);
      //delayMicroseconds(2);
   }
}

int Board_Stm32f103rct6::get_config(){
   return EEPROM.read(CONFIG_EEPROM_BASE);
}

void Board_Stm32f103rct6::pin_init(unsigned char pin, unsigned char mode){
   pinMode(pin,mode);
}

void Board_Stm32f103rct6::pin_write(unsigned char pin, unsigned char level){
   digitalWrite(pin, level);
}

unsigned char Board_Stm32f103rct6::pin_read(unsigned char pin){
   return digitalRead(pin);
}

unsigned char Board_Stm32f103rct6::pin_analog_read(unsigned char pin){
   return analogRead(pin);
}

void Board_Stm32f103rct6::pin_analog_write(unsigned char pin, unsigned char level){
   analogWrite(pin, level);
}

void Board_Stm32f103rct6::pwm_init(unsigned char khz){
   //null
}

void Board_Stm32f103rct6::pwm_init(unsigned char pin, unsigned char hz){
   //null
}

void Board_Stm32f103rct6::pwm_output(unsigned char pin, unsigned short value){
  //null
}

unsigned long Board_Stm32f103rct6::get_tick_count(){
   return millis();
}