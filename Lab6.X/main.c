/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: simple HelloWorld application     */
/*                for Amazing Ball platform         */
/*                                                  */
/****************************************************/

#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <math.h>
#include <libpic30.h>

#include "lcd.h"
#include "led.h"
#include "touch.h"

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF);  

//float clamp(int16_t val,int16_t min, int16_t max) {
//    if(val < min)
//        val = min;
//    
//    if(val > max)
//        val=max;
//    
//    return ((float)(val - min)) /((float)(max-min));
//}

void setup_timer1() {
    __builtin_write_OSCCONL(OSCCONL | 2);
    T1CONbits.TON = 0; //Disable Timer
    T1CONbits.TCS = 1; //Select external clock
    T1CONbits.TSYNC = 0; //Disable Synchronization
    T1CONbits.TCKPS = 0b00; //Select 1:1 Prescaler
    TMR1 = 0x00; //Clear timer register
    PR1 = 6767; //Load the period value
    IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
    IEC0bits.T1IE = 1;// Enable Timer1 interrupt
    T1CONbits.TON = 1;// Start Timer
}


void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    lcd_locate(1,1);
    xy xy_read = touch_read();
    lcd_printf("x: %u",xy_read.x);
    lcd_locate(1,2);
    lcd_printf("y: %u",xy_read.y);
    IFS0bits.T1IF = 0;        // clear the interrupt flag
}

int main(){
	/* LCD Initialization Sequence */
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
    touch_init();
    AD1PCFGHbits.PCFG20 = 1;Nop();
    setup_timer1();
   
	while(1){
        
	}
    
    
    
    return 0;
}


