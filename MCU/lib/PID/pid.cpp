#include "pid.h"

#include "board.h"

PID::PID(float* _input, float* _feedback, float _kp, float _ki, float _kd, unsigned short _max_output)
    :input(_input), feedback(_feedback), kp(_kp), ki(_ki), kd(_kd), max_output(_max_output*1.0)
{
    clear();
}

void PID::clear()
{
    error = integra = derivative = previous_error =0;
}

void PID::update(float _kp, float _ki, float _kd, unsigned short _max_output)
{
	kp = _kp;
	ki = _ki;
	kd = _kd;
	max_output = _max_output;
}

short PID::compute(float interval)
{
    error = *input - *feedback;

	integra = integra + error*interval;
	derivative = (error - previous_error) / interval;

	previous_error = error;

	if (ki != 0) {
	#if PID_DEBUG_OUTPUT
		pb_printf("integra=%ld max_output=%ld %ld", long(integra*1000), long(-(max_output/ki*1000)), long(max_output/ki*1000));
	#endif
		if (integra < -(max_output/ki)) {
			//pb_printf("integra clear-");
			integra = -(max_output/ki);
		}
		if (integra > max_output/ki) {
			//pb_printf("integra clear+");
			integra = max_output/ki;
		}
	}

	float val = error*kp + integra*ki + derivative*kd;

	if (val < -max_output)
		val = -max_output+1;
	else if (val > max_output)
		val = max_output-1;

#if PID_DEBUG_OUTPUT
	pb_printf("error=%ld integra=%ld derivative=%ld val=%ld", long(error*1000), long(integra*1000), long(derivative*1000), long(val*1000));
#endif

	return val;
}