/*
 * File:   motor.c
 * Author: team-2b
 *
 * Created on March 22, 2019, 3:05 PM
 */


#include "motor.h"

//void setup_timer3() {
//    //every ms
//    CLEARBIT(T3CONbits.TON);      // Disable Timer
//    CLEARBIT(T3CONbits.TCS);      // Select internal instruction cycle clock
//    CLEARBIT(T3CONbits.TGATE);    // Disable Gated Timer mode
//    TMR3 = 0x00;            // Clear timer register
//    T3CONbits.TCKPS = 0b00; // Select 1:64 Prescaler
//    PR3 = 64*1000;             // Load the period value
//    IPC0bits.T1IP2 = 0x02;   // Set Timer1 Interrupt Priority Level
//    CLEARBIT(IFS0bits.T3IF);      // Clear Timer3 Interrupt Flag
//    SETBIT(IEC0bits.T3IE);      // Enable Timer3 interrupt
//    SETBIT(T3CONbits.TON);      // Start Timer
//}
//
//
//void __attribute__((__interrupt__)) _T3Interrupt(void)
//{
//    
//    IFS0bits.T3IF = 0;        // clear the interrupt flag
//}



void motor_init(uint8_t chan) {
    //setup Timer 2
    CLEARBIT(T2CONbits.TON);     // Disable Timer
    CLEARBIT(T2CONbits.TCS);    // Select internal instruction cycle clock
    CLEARBIT(T2CONbits.TGATE);  // Disable Gated Timer mode
    TMR2 = 0x00;                // Clear timer register
    T2CONbits.TCKPS = 0b10;     // Select 1:64 Prescaler
    CLEARBIT(IFS0bits.T2IF);    // Clear Timer2 interrupt status flag
    CLEARBIT(IEC0bits.T2IE);    // Disable Timer2 interrupt enable control bit
    PR2 = 4000;                 // Set timer period 20ms:
    // 4000= 20*10^-3 * 12.8*10^6 * 1/64
    //setup OC8
    if(chan == 8) {
        CLEARBIT(TRISDbits.TRISD7); /* Set OC8 as output */
        OC8R = 3700;             /* Set the initial duty cycle to 5ms*/
        OC8RS = 3700;            /* Load OCRS: next pwm duty cycle */
        OC8CON = 0x0006;      /* Set OC8: PWM, no fault check, Timer2 */
    } else if(chan == 7) {
        CLEARBIT(TRISDbits.TRISD6); /* Set OC8 as output */
        OC7R = 3700;             /* Set the initial duty cycle to 5ms*/
        OC7RS = 3700;            /* Load OCRS: next pwm duty cycle */
        OC7CON = 0x0006;      /* Set OC8: PWM, no fault check, Timer2 */
    }
    
    SETBIT(T2CONbits.TON); /* Turn Timer 2 on */
//    setup_timer3();
}

float lerp(float zero_to_one, float lower, float upper) {
    if(zero_to_one < 0)
        zero_to_one = 0;
    if (zero_to_one > 1)
        zero_to_one = 1;
    return zero_to_one*(upper-lower) + lower;
}

float last_smoothed = 0.5;
const float smoothness = 0;
float smoothed_to_width(float angle) {
    float smoothed = (1-smoothness)*angle + smoothness*last_smoothed;
    last_smoothed = smoothed;
    return angle_to_width(smoothed);
}

float angle_to_width(float angle) {
    return lerp(1-angle,140,470);
}

void motor_set_duty(uint8_t chan, uint16_t pulse_width) {
    uint16_t final_num = 4000-pulse_width;
    if(chan == 8) {
        OC8RS = final_num;            /* Load OCRS: next pwm duty cycle */
    } else if (chan == 7) {
        OC7RS = final_num;            /* Load OCRS: next pwm duty cycle */
    }
    
}