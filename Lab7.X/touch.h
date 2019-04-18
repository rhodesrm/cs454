#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>
#include "types.h"
#include <stdlib.h>

typedef struct xy {
    uint16_t x;
    uint16_t y;
} xy;

void touch_init(void);
xy touch_read(void);