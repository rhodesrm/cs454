/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab3 timer initialization func.   */
/*                                                  */
/****************************************************/


#include "timer.h"

uint8_t* is_timeout;

void setup_timer(uint8_t* timed_out) {
            //Timer1 every sec
    __builtin_write_OSCCONL(OSCCONL | 2);
    T1CONbits.TON = 0; //Disable Timer
    T1CONbits.TCS = 1; //Select external clock
    T1CONbits.TSYNC = 0; //Disable Synchronization
    T1CONbits.TCKPS = 0b00; //Select 1:1 Prescaler
    TMR1 = 0x00; //Clear timer register
    PR1 = 32767; //Load the period value
    IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
    IEC0bits.T1IE = 1;// Enable Timer1 interrupt
    T1CONbits.TON = 1;// Start Timer
    is_timeout = timed_out;
}

void turn_off_timer() {
    (*is_timeout) = 0;
    T1CONbits.TON = 0;
}

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    (*is_timeout) = 1;
    IFS0bits.T1IF = 0;        // clear the interrupt flag
}


