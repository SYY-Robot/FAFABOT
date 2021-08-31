/* Encoder Library, for measuring quadrature encoded signals
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 * Copyright (c) 2011,2013 PJRC.COM, LLC - Paul Stoffregen <paul@pjrc.com>
 *
 * Version 1.2 - fix -2 bug in C-only code
 * Version 1.1 - expand to support boards with up to 60 interrupts
 * Version 1.0 - initial release
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef Stm32_Encoder_h_
#define Stm32_Encoder_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif

#if defined(__AVR__) || (defined(__arm__) && defined(CORE_TEENSY))
#define IO_REG_TYPE			uint8_t
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_PIN_READ(base, mask)     (((*(base)) & (mask)) ? 1 : 0)

#elif defined(ARDUINO_GENERIC_F103RCTX)
#define IO_REG_TYPE			uint32_t
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_PIN_READ(base, mask)     (((*(base)) & (mask)) ? 1 : 0)

#endif

#define ENCODER_ARGLIST_SIZE CORE_NUM_INTERRUPT

#include "stm32_config/stm32_pins.h"


// All the data needed by interrupts is consolidated into this ugly struct
// to facilitate assembly language optimizing of the speed critical update.
// The assembly code uses auto-incrementing addressing modes, so the struct
// must remain in exactly this order.
typedef struct {
	volatile IO_REG_TYPE * pin1_register;
	volatile IO_REG_TYPE * pin2_register;
	IO_REG_TYPE            pin1_bitmask;
	IO_REG_TYPE            pin2_bitmask;
	uint8_t					  pin1;
	uint8_t					  pin2;
	uint8_t                state;
	int32_t                position;
} Stm32_Encoder_internal_state_t;

class Stm32_Encoder
{
public:
	Stm32_Encoder(uint8_t pin1, uint8_t pin2) {
		#ifdef INPUT_PULLUP
		pinMode(pin1, INPUT_PULLUP);
		pinMode(pin2, INPUT_PULLUP);
		#else
		pinMode(pin1, INPUT);
		digitalWrite(pin1, HIGH);
		pinMode(pin2, INPUT);
		digitalWrite(pin2, HIGH);
		#endif
		encoder.pin1_register = PIN_TO_BASEREG(pin1);
		encoder.pin1_bitmask = PIN_TO_BITMASK(pin1);
		encoder.pin2_register = PIN_TO_BASEREG(pin2);
		encoder.pin2_bitmask = PIN_TO_BITMASK(pin2);
		encoder.pin1 = pin1;
		encoder.pin2 = pin2;
		encoder.position = 0;
		//delayMicroseconds(2000);
		uint8_t s = 0;

		if (DIRECT_PIN_READ(encoder.pin1_register, encoder.pin1_bitmask)) s |= 1;
		if (DIRECT_PIN_READ(encoder.pin2_register, encoder.pin2_bitmask)) s |= 2;
		encoder.state = s;

		nterrupts_in_use = attach_interrupt(pin1, &encoder);
		nterrupts_in_use += attach_interrupt(pin2, &encoder);

		//update_finishup();  // to force linker to include the code (does not work)
	}

	inline int32_t read() {
		if (nterrupts_in_use < 2) {
			update(&encoder);
		}
		int32_t ret = encoder.position;
		return ret;
	}
	inline void write(int32_t p) {
		encoder.position = p;
	}

private:
	Stm32_Encoder_internal_state_t encoder;
	uint8_t nterrupts_in_use;
public:
	static Stm32_Encoder_internal_state_t * interruptArgs[ENCODER_ARGLIST_SIZE];

//                           _______         _______       
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2

		//	new	new	old	old
		//	pin2	pin1	pin2	pin1	Result
		//	----	----	----	----	------
		//	0	0	0	0	no movement
		//	0	0	0	1	+1
		//	0	0	1	0	-1
		//	0	0	1	1	+2  (assume pin1 edges only)
		//	0	1	0	0	-1
		//	0	1	0	1	no movement
		//	0	1	1	0	-2  (assume pin1 edges only)
		//	0	1	1	1	+1
		//	1	0	0	0	+1
		//	1	0	0	1	-2  (assume pin1 edges only)
		//	1	0	1	0	no movement
		//	1	0	1	1	-1
		//	1	1	0	0	+2  (assume pin1 edges only)
		//	1	1	0	1	-1
		//	1	1	1	0	+1
		//	1	1	1	1	no movement
/*
	// Simple, easy-to-read "documentation" version :-)
	//
	void update(void) {
		uint8_t s = state & 3;
		if (digitalRead(pin1)) s |= 4;
		if (digitalRead(pin2)) s |= 8;
		switch (s) {
			case 0: case 5: case 10: case 15:
				break;
			case 1: case 7: case 8: case 14:
				position++; break;
			case 2: case 4: case 11: case 13:
				position--; break;
			case 3: case 12:
				position += 2; break;
			default:
				position -= 2; break;
		}
		state = (s >> 2);
	}
*/

public:
	static void update(Stm32_Encoder_internal_state_t *arg) {
		uint8_t p1val = digitalRead(arg->pin1);
		uint8_t p2val = digitalRead(arg->pin2);
		uint8_t state = arg->state & 3;
		if (p1val) state |= 4;
		if (p2val) state |= 8;
		arg->state = (state >> 2);
		switch (state) {
			case 1: case 7: case 8: case 14:
				arg->position++;
				return;
			case 2: case 4: case 11: case 13:
				arg->position--;
				return;
			case 3: case 12:
				arg->position += 2;
				return;
			case 6: case 9:
				arg->position -= 2;
				return;
		}
	}
private:
	static uint8_t attach_interrupt(uint8_t pin, Stm32_Encoder_internal_state_t *state) {
		switch (pin)
		{
			#ifdef CORE_INT0_PIN
				case CORE_INT0_PIN:
					interruptArgs[0] = state;
					attachInterrupt(pin,pcint_isr0,CHANGE);
					break;
			#endif
			#ifdef CORE_INT1_PIN
				case CORE_INT1_PIN:
					interruptArgs[1] = state;
					attachInterrupt(pin,pcint_isr1,CHANGE);
					break;
			#endif
			#ifdef CORE_INT2_PIN
				case CORE_INT2_PIN:
					interruptArgs[2] = state;
					attachInterrupt(pin,pcint_isr2,CHANGE);
					break;
			#endif
			#ifdef CORE_INT3_PIN
				case CORE_INT3_PIN:
					interruptArgs[3] = state;
					attachInterrupt(pin,pcint_isr3,CHANGE);
					break;
			#endif
			#ifdef CORE_INT4_PIN
				case CORE_INT4_PIN:
					interruptArgs[4] = state;
					attachInterrupt(pin,pcint_isr4,CHANGE);
					break;
			#endif
			#ifdef CORE_INT5_PIN
				case CORE_INT5_PIN:
					interruptArgs[5] = state;
					attachInterrupt(pin,pcint_isr5,CHANGE);
					break;
			#endif
			#ifdef CORE_INT6_PIN
				case CORE_INT6_PIN:
					interruptArgs[6] = state;
					attachInterrupt(pin,pcint_isr6,CHANGE);
					break;
			#endif
			#ifdef CORE_INT7_PIN
				case CORE_INT7_PIN:
					interruptArgs[7] = state;
					attachInterrupt(pin,pcint_isr7,CHANGE);
					break;
			#endif
			#ifdef CORE_INT8_PIN
				case CORE_INT8_PIN:
					interruptArgs[8] = state;
					attachInterrupt(pin,pcint_isr8,CHANGE);
					break;
			#endif
			#ifdef CORE_INT9_PIN
				case CORE_INT9_PIN:
					interruptArgs[9] = state;
					attachInterrupt(pin,pcint_isr9,CHANGE);
					break;
			#endif
			#ifdef CORE_INT10_PIN
				case CORE_INT10_PIN:
					interruptArgs[10] = state;
					attachInterrupt(pin,pcint_isr10,CHANGE);
					break;
			#endif
			#ifdef CORE_INT11_PIN
				case CORE_INT11_PIN:
					interruptArgs[11] = state;
					attachInterrupt(pin,pcint_isr11,CHANGE);
					break;
			#endif
			#ifdef CORE_INT12_PIN
				case CORE_INT12_PIN:
					interruptArgs[12] = state;
					attachInterrupt(pin,pcint_isr12,CHANGE);
					break;
			#endif
			#ifdef CORE_INT13_PIN
				case CORE_INT13_PIN:
					interruptArgs[13] = state;
					attachInterrupt(pin,pcint_isr13,CHANGE);
					break;
			#endif
			#ifdef CORE_INT14_PIN
				case CORE_INT14_PIN:
					interruptArgs[14] = state;
					attachInterrupt(pin,pcint_isr14,CHANGE);
					break;
			#endif
			#ifdef CORE_INT15_PIN
				case CORE_INT15_PIN:
					interruptArgs[15] = state;
					attachInterrupt(pin,pcint_isr15,CHANGE);
					break;
			#endif
			#ifdef CORE_INT16_PIN
				case CORE_INT16_PIN:
					interruptArgs[16] = state;
					attachInterrupt(pin,pcint_isr16,CHANGE);
					break;
			#endif
			#ifdef CORE_INT17_PIN
				case CORE_INT17_PIN:
					interruptArgs[17] = state;
					attachInterrupt(pin,pcint_isr17,CHANGE);
					break;
			#endif
			#ifdef CORE_INT18_PIN
				case CORE_INT18_PIN:
					interruptArgs[18] = state;
					attachInterrupt(pin,pcint_isr18,CHANGE);
					break;
			#endif
			#ifdef CORE_INT19_PIN
				case CORE_INT19_PIN:
					interruptArgs[19] = state;
					attachInterrupt(pin,pcint_isr19,CHANGE);
					break;
			#endif
			#ifdef CORE_INT20_PIN
				case CORE_INT20_PIN:
					interruptArgs[20] = state;
					attachInterrupt(pin,pcint_isr20,CHANGE);
					break;
			#endif
			#ifdef CORE_INT21_PIN
				case CORE_INT21_PIN:
					interruptArgs[21] = state;
					attachInterrupt(pin,pcint_isr12,CHANGE);
					break;
			#endif
			#ifdef CORE_INT22_PIN
				case CORE_INT22_PIN:
					interruptArgs[22] = state;
					attachInterrupt(pin,pcint_isr22,CHANGE);
					break;
			#endif
			#ifdef CORE_INT23_PIN
				case CORE_INT23_PIN:
					interruptArgs[23] = state;
					attachInterrupt(pin,pcint_isr23,CHANGE);
					break;
			#endif
			#ifdef CORE_INT24_PIN
				case CORE_INT24_PIN:
					interruptArgs[24] = state;
					attachInterrupt(pin,pcint_isr24,CHANGE);
					break;
			#endif
			#ifdef CORE_INT25_PIN
				case CORE_INT25_PIN:
					interruptArgs[25] = state;
					attachInterrupt(pin,pcint_isr25,CHANGE);
					break;
			#endif
			#ifdef CORE_INT26_PIN
				case CORE_INT26_PIN:
					interruptArgs[26] = state;
					attachInterrupt(pin,pcint_isr26,CHANGE);
					break;
			#endif
			#ifdef CORE_INT27_PIN
				case CORE_INT27_PIN:
					interruptArgs[27] = state;
					attachInterrupt(pin,pcint_isr27,CHANGE);
					break;
			#endif
			#ifdef CORE_INT28_PIN
				case CORE_INT28_PIN:
					interruptArgs[28] = state;
					attachInterrupt(pin,pcint_isr28,CHANGE);
					break;
			#endif
			#ifdef CORE_INT29_PIN
				case CORE_INT29_PIN:
					interruptArgs[29] = state;
					attachInterrupt(pin,pcint_isr29,CHANGE);
					break;
			#endif
			#ifdef CORE_INT30_PIN
				case CORE_INT30_PIN:
					interruptArgs[30] = state;
					attachInterrupt(pin,pcint_isr30,CHANGE);
					break;
			#endif
			#ifdef CORE_INT31_PIN
				case CORE_INT31_PIN:
					interruptArgs[31] = state;
					attachInterrupt(pin,pcint_isr31,CHANGE);
					break;
			#endif
			#ifdef CORE_INT32_PIN
				case CORE_INT32_PIN:
					interruptArgs[32] = state;
					attachInterrupt(pin,pcint_isr32,CHANGE);
					break;
			#endif
			#ifdef CORE_INT33_PIN
				case CORE_INT33_PIN:
					interruptArgs[33] = state;
					attachInterrupt(pin,pcint_isr33,CHANGE);
					break;
			#endif
			#ifdef CORE_INT34_PIN
				case CORE_INT34_PIN:
					interruptArgs[34] = state;
					attachInterrupt(pin,pcint_isr34,CHANGE);
					break;
			#endif
			#ifdef CORE_INT35_PIN
				case CORE_INT35_PIN:
					interruptArgs[35] = state;
					attachInterrupt(pin,pcint_isr35,CHANGE);
					break;
			#endif
			#ifdef CORE_INT36_PIN
				case CORE_INT36_PIN:
					interruptArgs[36] = state;
					attachInterrupt(pin,pcint_isr36,CHANGE);
					break;
			#endif
			#ifdef CORE_INT37_PIN
				case CORE_INT37_PIN:
					interruptArgs[37] = state;
					attachInterrupt(pin,pcint_isr37,CHANGE);
					break;
			#endif
			#ifdef CORE_INT38_PIN
				case CORE_INT38_PIN:
					interruptArgs[38] = state;
					attachInterrupt(pin,pcint_isr38,CHANGE);
					break;
			#endif
			#ifdef CORE_INT39_PIN
				case CORE_INT39_PIN:
					interruptArgs[39] = state;
					attachInterrupt(pin,pcint_isr39,CHANGE);
					break;
			#endif
			#ifdef CORE_INT40_PIN
				case CORE_INT40_PIN:
					interruptArgs[40] = state;
					attachInterrupt(pin,pcint_isr40,CHANGE);
					break;
			#endif
			#ifdef CORE_INT41_PIN
				case CORE_INT41_PIN:
					interruptArgs[41] = state;
					attachInterrupt(pin,pcint_isr41,CHANGE);
					break;
			#endif
			#ifdef CORE_INT42_PIN
				case CORE_INT42_PIN:
					interruptArgs[42] = state;
					attachInterrupt(pin,pcint_isr42,CHANGE);
					break;
			#endif
			#ifdef CORE_INT43_PIN
				case CORE_INT43_PIN:
					interruptArgs[43] = state;
					attachInterrupt(pin,pcint_isr43,CHANGE);
					break;
			#endif
			#ifdef CORE_INT44_PIN
				case CORE_INT44_PIN:
					interruptArgs[44] = state;
					attachInterrupt(pin,pcint_isr44,CHANGE);
					break;
			#endif
			#ifdef CORE_INT45_PIN
				case CORE_INT45_PIN:
					interruptArgs[45] = state;
					attachInterrupt(pin,pcint_isr45,CHANGE);
					break;
			#endif
			#ifdef CORE_INT46_PIN
				case CORE_INT46_PIN:
					interruptArgs[46] = state;
					attachInterrupt(pin,pcint_isr46,CHANGE);
					break;
			#endif
			#ifdef CORE_INT47_PIN
				case CORE_INT47_PIN:
					interruptArgs[47] = state;
					attachInterrupt(pin,pcint_isr47,CHANGE);
					break;
			#endif
			#ifdef CORE_INT48_PIN
				case CORE_INT48_PIN:
					interruptArgs[48] = state;
					attachInterrupt(pin,pcint_isr48,CHANGE);
					break;
			#endif
			#ifdef CORE_INT49_PIN
				case CORE_INT49_PIN:
					interruptArgs[49] = state;
					attachInterrupt(pin,pcint_isr49,CHANGE);
					break;
			#endif
			#ifdef CORE_INT50_PIN
				case CORE_INT50_PIN:
					interruptArgs[50] = state;
					attachInterrupt(pin,pcint_isr50,CHANGE);
					break;
			#endif
			#ifdef CORE_INT51_PIN
				case CORE_INT51_PIN:
					interruptArgs[51] = state;
					attachInterrupt(pin,pcint_isr51,CHANGE);
					break;
			#endif
			#ifdef CORE_INT52_PIN
				case CORE_INT52_PIN:
					interruptArgs[52] = state;
					attachInterrupt(pin,pcint_isr52,CHANGE);
					break;
			#endif
			#ifdef CORE_INT53_PIN
				case CORE_INT53_PIN:
					interruptArgs[53] = state;
					attachInterrupt(pin,pcint_isr53,CHANGE);
					break;
			#endif
			#ifdef CORE_INT54_PIN
				case CORE_INT54_PIN:
					interruptArgs[54] = state;
					attachInterrupt(pin,pcint_isr54,CHANGE);
					break;
			#endif
			#ifdef CORE_INT55_PIN
				case CORE_INT55_PIN:
					interruptArgs[55] = state;
					attachInterrupt(pin,pcint_isr55,CHANGE);
					break;
			#endif
			#ifdef CORE_INT56_PIN
				case CORE_INT56_PIN:
					interruptArgs[56] = state;
					attachInterrupt(pin,pcint_isr56,CHANGE);
					break;
			#endif
			#ifdef CORE_INT57_PIN
				case CORE_INT57_PIN:
					interruptArgs[57] = state;
					attachInterrupt(pin,pcint_isr57,CHANGE);
					break;
			#endif
			#ifdef CORE_INT58_PIN
					case CORE_INT58_PIN:
					interruptArgs[58] = state;
					attachInterrupt(pin,pcint_isr58,CHANGE);
					break;
			#endif
			#ifdef CORE_INT59_PIN
					case CORE_INT59_PIN:
					interruptArgs[59] = state;
					attachInterrupt(pin,pcint_isr59,CHANGE);
					break;
			#endif
			#ifdef CORE_INT60_PIN
				case CORE_INT60_PIN:
					interruptArgs[60] = state;
					attachInterrupt(pin,pcint_isr60,CHANGE);
					break;
			#endif
			#ifdef CORE_INT61_PIN
				case CORE_INT61_PIN:
					interruptArgs[61] = state;
					attachInterrupt(pin,pcint_isr61,CHANGE);
					break;
			#endif
			#ifdef CORE_INT62_PIN
				case CORE_INT62_PIN:
					interruptArgs[62] = state;
					attachInterrupt(pin,pcint_isr62,CHANGE);
					break;
			#endif
			#ifdef CORE_INT63_PIN
				case CORE_INT63_PIN:
					interruptArgs[63] = state;
					attachInterrupt(pin,pcint_isr63,CHANGE);
					break;
			#endif
			#ifdef CORE_INT64_PIN
				case CORE_INT64_PIN:
					interruptArgs[64] = state;
					attachInterrupt(pin,pcint_isr64,CHANGE);
					break;
			#endif
			#ifdef CORE_INT65_PIN
				case CORE_INT65_PIN:
					interruptArgs[65] = state;
					attachInterrupt(pin,pcint_isr65,CHANGE);
					break;
			#endif
			#ifdef CORE_INT66_PIN
				case CORE_INT66_PIN:
					interruptArgs[66] = state;
					attachInterrupt(pin,pcint_isr66,CHANGE);
					break;
			#endif
			#ifdef CORE_INT67_PIN
				case CORE_INT67_PIN:
					interruptArgs[67] = state;
					attachInterrupt(pin,pcint_isr67,CHANGE);
					break;
			#endif
			#ifdef CORE_INT68_PIN
				case CORE_INT68_PIN:
					interruptArgs[68] = state;
					attachInterrupt(pin,pcint_isr68,CHANGE);
					break;
			#endif
			#ifdef CORE_INT69_PIN
				case CORE_INT69_PIN:
					interruptArgs[69] = state;
					attachInterrupt(pin,pcint_isr69,CHANGE);
					break;
			#endif
				default:
					return 0;
		}
		return 1;
	}

	#ifdef CORE_INT0_PIN
	static void pcint_isr0(void) { update(interruptArgs[0]); }
	#endif
	#ifdef CORE_INT1_PIN
	static void pcint_isr1(void) { update(interruptArgs[1]); }
	#endif
	#ifdef CORE_INT2_PIN
	static void pcint_isr2(void) { update(interruptArgs[2]); }
	#endif
	#ifdef CORE_INT3_PIN
	static void pcint_isr3(void) { update(interruptArgs[3]); }
	#endif
	#ifdef CORE_INT4_PIN
	static void pcint_isr4(void) { update(interruptArgs[4]); }
	#endif
	#ifdef CORE_INT5_PIN
	static void pcint_isr5(void) { update(interruptArgs[5]); }
	#endif
	#ifdef CORE_INT6_PIN
	static void pcint_isr6(void) { update(interruptArgs[6]); }
	#endif
	#ifdef CORE_INT7_PIN
	static void pcint_isr7(void) { update(interruptArgs[7]); }
	#endif
	#ifdef CORE_INT8_PIN
	static void pcint_isr8(void) { update(interruptArgs[8]); }
	#endif
	#ifdef CORE_INT9_PIN
	static void pcint_isr9(void) { update(interruptArgs[9]); }
	#endif
	#ifdef CORE_INT10_PIN
	static void pcint_isr10(void) { update(interruptArgs[10]); }
	#endif
	#ifdef CORE_INT11_PIN
	static void pcint_isr11(void) { update(interruptArgs[11]); }
	#endif
	#ifdef CORE_INT12_PIN
	static void pcint_isr12(void) { update(interruptArgs[12]); }
	#endif
	#ifdef CORE_INT13_PIN
	static void pcint_isr13(void) { update(interruptArgs[13]); }
	#endif
	#ifdef CORE_INT14_PIN
	static void pcint_isr14(void) { update(interruptArgs[14]); }
	#endif
	#ifdef CORE_INT15_PIN
	static void pcint_isr15(void) { update(interruptArgs[15]); }
	#endif
	#ifdef CORE_INT16_PIN
	static void pcint_isr16(void) { update(interruptArgs[16]); }
	#endif
	#ifdef CORE_INT17_PIN
	static void pcint_isr17(void) { update(interruptArgs[17]); }
	#endif
	#ifdef CORE_INT18_PIN
	static void pcint_isr18(void) { update(interruptArgs[18]); }
	#endif
	#ifdef CORE_INT19_PIN
	static void pcint_isr19(void) { update(interruptArgs[19]); }
	#endif
	#ifdef CORE_INT20_PIN
	static void pcint_isr20(void) { update(interruptArgs[20]); }
	#endif
	#ifdef CORE_INT21_PIN
	static void pcint_isr21(void) { update(interruptArgs[21]); }
	#endif
	#ifdef CORE_INT22_PIN
	static void pcint_isr22(void) { update(interruptArgs[22]); }
	#endif
	#ifdef CORE_INT23_PIN
	static void pcint_isr23(void) { update(interruptArgs[23]); }
	#endif
	#ifdef CORE_INT24_PIN
	static void pcint_isr24(void) { update(interruptArgs[24]); }
	#endif
	#ifdef CORE_INT25_PIN
	static void pcint_isr25(void) { update(interruptArgs[25]); }
	#endif
	#ifdef CORE_INT26_PIN
	static void pcint_isr26(void) { update(interruptArgs[26]); }
	#endif
	#ifdef CORE_INT27_PIN
	static void pcint_isr27(void) { update(interruptArgs[27]); }
	#endif
	#ifdef CORE_INT28_PIN
	static void pcint_isr28(void) { update(interruptArgs[28]); }
	#endif
	#ifdef CORE_INT29_PIN
	static void pcint_isr29(void) { update(interruptArgs[29]); }
	#endif
	#ifdef CORE_INT30_PIN
	static void pcint_isr30(void) { update(interruptArgs[30]); }
	#endif
	#ifdef CORE_INT31_PIN
	static void pcint_isr31(void) { update(interruptArgs[31]); }
	#endif
	#ifdef CORE_INT32_PIN
	static void pcint_isr32(void) { update(interruptArgs[32]); }
	#endif
	#ifdef CORE_INT33_PIN
	static void pcint_isr33(void) { update(interruptArgs[33]); }
	#endif
	#ifdef CORE_INT34_PIN
	static void pcint_isr34(void) { update(interruptArgs[34]); }
	#endif
	#ifdef CORE_INT35_PIN
	static void pcint_isr35(void) { update(interruptArgs[35]); }
	#endif
	#ifdef CORE_INT36_PIN
	static void pcint_isr36(void) { update(interruptArgs[36]); }
	#endif
	#ifdef CORE_INT37_PIN
	static void pcint_isr37(void) { update(interruptArgs[37]); }
	#endif
	#ifdef CORE_INT38_PIN
	static void pcint_isr38(void) { update(interruptArgs[38]); }
	#endif
	#ifdef CORE_INT39_PIN
	static void pcint_isr39(void) { update(interruptArgs[39]); }
	#endif
	#ifdef CORE_INT40_PIN
	static void pcint_isr40(void) { update(interruptArgs[40]); }
	#endif
	#ifdef CORE_INT41_PIN
	static void pcint_isr41(void) { update(interruptArgs[41]); }
	#endif
	#ifdef CORE_INT42_PIN
	static void pcint_isr42(void) { update(interruptArgs[42]); }
	#endif
	#ifdef CORE_INT43_PIN
	static void pcint_isr43(void) { update(interruptArgs[43]); }
	#endif
	#ifdef CORE_INT44_PIN
	static void pcint_isr44(void) { update(interruptArgs[44]); }
	#endif
	#ifdef CORE_INT45_PIN
	static void pcint_isr45(void) { update(interruptArgs[45]); }
	#endif
	#ifdef CORE_INT46_PIN
	static void pcint_isr46(void) { update(interruptArgs[46]); }
	#endif
	#ifdef CORE_INT47_PIN
	static void pcint_isr47(void) { update(interruptArgs[47]); }
	#endif
	#ifdef CORE_INT48_PIN
	static void pcint_isr48(void) { update(interruptArgs[48]); }
	#endif
	#ifdef CORE_INT49_PIN
	static void pcint_isr49(void) { update(interruptArgs[49]); }
	#endif
	#ifdef CORE_INT50_PIN
	static void pcint_isr50(void) { update(interruptArgs[50]); }
	#endif
	#ifdef CORE_INT51_PIN
	static void pcint_isr51(void) { update(interruptArgs[51]); }
	#endif
	#ifdef CORE_INT52_PIN
	static void pcint_isr52(void) { update(interruptArgs[52]); }
	#endif
	#ifdef CORE_INT53_PIN
	static void pcint_isr53(void) { update(interruptArgs[53]); }
	#endif
	#ifdef CORE_INT54_PIN
	static void pcint_isr54(void) { update(interruptArgs[54]); }
	#endif
	#ifdef CORE_INT55_PIN
	static void pcint_isr55(void) { update(interruptArgs[55]); }
	#endif
	#ifdef CORE_INT56_PIN
	static void pcint_isr56(void) { update(interruptArgs[56]); }
	#endif
	#ifdef CORE_INT57_PIN
	static void pcint_isr57(void) { update(interruptArgs[57]); }
	#endif
	#ifdef CORE_INT58_PIN
	static void pcint_isr58(void) { update(interruptArgs[58]); }
	#endif
	#ifdef CORE_INT59_PIN
	static void pcint_isr59(void) { update(interruptArgs[59]); }
	#endif
	#ifdef CORE_INT60_PIN
	static void pcint_isr60(void) { update(interruptArgs[60]); }
	#endif
	#ifdef CORE_INT61_PIN
	static void pcint_isr61(void) { update(interruptArgs[61]); }
	#endif
	#ifdef CORE_INT62_PIN
	static void pcint_isr62(void) { update(interruptArgs[62]); }
	#endif
	#ifdef CORE_INT63_PIN
	static void pcint_isr63(void) { update(interruptArgs[63]); }
	#endif
	#ifdef CORE_INT64_PIN
	static void pcint_isr64(void) { update(interruptArgs[64]); }
	#endif
	#ifdef CORE_INT65_PIN
	static void pcint_isr65(void) { update(interruptArgs[65]); }
	#endif
	#ifdef CORE_INT66_PIN
	static void pcint_isr66(void) { update(interruptArgs[66]); }
	#endif
	#ifdef CORE_INT67_PIN
	static void pcint_isr67(void) { update(interruptArgs[67]); }
	#endif
	#ifdef CORE_INT68_PIN
	static void pcint_isr68(void) { update(interruptArgs[68]); }
	#endif
	#ifdef CORE_INT69_PIN
	static void pcint_isr69(void) { update(interruptArgs[69]); }
	#endif
};

#endif