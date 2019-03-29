
#include <p33Fxxxx.h>
#include <libpic30.h>
#include "types.h"

void motor_init(uint8_t chan);
void motor_set_duty(uint8_t chan, uint16_t pulse_width);
float angle_to_width(float angle);