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

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF);  


int main(){
	/* LCD Initialization Sequence */
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
	lcd_locate(1,1);
	lcd_printf("Hello World!");	
    drawCircle(64,32,30,1);
	/* Do nothing */
    int waiter = 0;
    int rad = 2;
	while(1){
        if(waiter > 1000) {
            waiter = 0;
            lcd_clear();
            int final_rad = 25+10*sin(rad/100);
            drawCircle(64,32,final_rad,1);
            rad++;
        }
        waiter++;
		
	}
    
    return 0;
}


