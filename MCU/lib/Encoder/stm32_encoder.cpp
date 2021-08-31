#include "stm32_encoder.h"

// Yes, all the code is in the header file, to provide the user
// configure options with #define (before they include it), and
// to facilitate some crafty optimizations!

Stm32_Encoder_internal_state_t * Stm32_Encoder::interruptArgs[];


