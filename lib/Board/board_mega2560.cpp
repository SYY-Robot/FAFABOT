#include "board_mega2560.h"
#include "Arduino.h"
#include "EEPROM.h"
#include "TimerOne.h"

Board_Mega2560 Board_Mega2560::board;

#define CONFIG_EEPROM_BASE 0

Board* Board::get()
{
	return &Board_Mega2560::board;
}


Board_Mega2560::Board_Mega2560()
{

}

Board_Mega2560::~Board_Mega2560()
{

}

void Board_Mega2560::enable_irq()
{
	interrupts();
}

void Board_Mega2560::disable_irq()
{
	noInterrupts();
}

void Board_Mega2560::usart_debug_init()
{
	usart_init(USART_3, 115200);
}

void Board_Mega2560::usart_init(unsigned char num, unsigned long buad)
{
	if (num == (unsigned char)USART_0) {
		Serial.begin(buad);
	}
	else if (num == (unsigned char)USART_1) {
		Serial1.begin(buad);
	}
	else if (num == (unsigned char)USART_2) {
		Serial2.begin(buad);
	}
	else if (num == (unsigned char)USART_3) {
		Serial3.begin(buad);
	}
}


Queue* Board_Mega2560::usart_getDataQueue(unsigned char num)
{
	if (num == (unsigned char)USART_0) {
		return &usart1_queue;
	}
	else if(num == (unsigned char)USART_1){
		return &usart2_queue;
	}
	else if(num == (unsigned char)USART_2){
		return &usart3_queue;
	}
	else if(num == (unsigned char)USART_3){
		return &usart4_queue;
	}

	return 0;
}

void Board_Mega2560::usart_write(unsigned char num, unsigned char ch)
{
	if (num == (unsigned char)USART_0) {
		Serial.write(ch);
	}
	else if(num == (unsigned char)USART_1){
		Serial1.write(ch);
	}
	else if(num == (unsigned char)USART_2){
		Serial2.write(ch);
	}
	else if(num == (unsigned char)USART_3){
		Serial3.write(ch);
	}
}

void Board_Mega2560::usart_write(unsigned char num, unsigned char* data, unsigned char len)
{
	if (num == (unsigned char)USART_0) {
		Serial.write((char*)data, len);
	}
	else if (num == (unsigned char)USART_1) {
		Serial1.write((char*)data, len);
	}
	else if (num == (unsigned char)USART_2) {
		Serial2.write((char*)data, len);
	}
	else if (num == (unsigned char)USART_3) {
		Serial3.write((char*)data, len);
	}
}


void serialEvent()
{
	if (Serial.available()) {
		if (!Board::get()->usart_getDataQueue(USART_0)->put(Serial.read())) {
			//err
		}
	}
}

void serialEvent1(){
	if (Serial1.available()) {
		if (!Board::get()->usart_getDataQueue(USART_1)->put(Serial1.read())) {
			//err
		}
	}
}

void serialEvent2(){
	/*if (Serial2.available()) {
		if (!Board::get()->usart_getDataQueue(USART_2)->put(Serial2.read())) {
			//err
		}
	}*/
}

void serialEvent3(){
	if (Serial3.available()) {
		if (!Board::get()->usart_getDataQueue(USART_3)->put(Serial3.read())) {
			//err
		}
	}
}

void Board_Mega2560::set_config(unsigned char* data, unsigned short len)
{
	for (unsigned short i = 0; i < len;i++) {
		EEPROM.write(CONFIG_EEPROM_BASE + i, data[i]);
		delayMicroseconds(2);
	}
}

void Board_Mega2560::get_config(unsigned char* data, unsigned short len)
{
	for (unsigned short i = 0; i < len;i++) {
		data[i] = EEPROM.read(CONFIG_EEPROM_BASE + i);
		delayMicroseconds(2);
	}
}

void Board_Mega2560::pin_init(unsigned char pin, unsigned char mode)
{
	pinMode(pin, mode);
}

void Board_Mega2560::pin_write(unsigned char pin, unsigned char level)
{
	digitalWrite(pin, level);
}

unsigned char Board_Mega2560::pin_read(unsigned char pin)
{
	return digitalRead(pin);
}

unsigned short Board_Mega2560::pin_analogread(unsigned char pin)
{
	return analogRead(pin);
}

void Board_Mega2560::pwm_init(unsigned char khz)
{
	//Timer1.initialize(1000.0 / khz);
}

void Board_Mega2560::pwm_output(unsigned char pin, unsigned short value)
{
	/*if (value == 0)
		Timer1.disablePwm(pin);
	Timer1.pwm(pin, value);*/
	analogWrite(pin,value);
}

unsigned long Board_Mega2560::get_tick_count()
{
	return millis();
}