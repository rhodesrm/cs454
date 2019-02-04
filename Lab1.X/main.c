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
#include "joystick.h"

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF);  

void push(int new_item, int* array, int length) {
    int i;
    for(i = 1;i < length; i++)
        array[i-1] = array[i];
    array[length-1] = new_item;
}

int vote(int* array,int length) {
    int zeros = 0;
    int ones = 0;
    int i;
    for(i = 0; i < length; i++)
        if(array[i] == 0)
            zeros++;
        else
            ones++;
    return ones > zeros;
}

void print_names() {
    lcd_printf("Robin Rhodes\r");
    __delay_ms(500);
    lcd_printf("Bassel El Mabsout\r");
    __delay_ms(500);
    lcd_printf("Eric Chang\r");
    __delay_ms(500);
}

int main(){
	/* LCD Initialization Sequence */
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
    led_initialize();
    int i;
    for(i = 0; i < 6; i++) {
        TOGGLEBIT(LED4_PORT); 
        __delay_ms(1000);
    }
//    
//    
    AD1PCFGHbits.PCFG20 = 1;Nop();
    TRISEbits.TRISE8 = 1; Nop();
    TRISDbits.TRISD10 = 1; Nop();
    int inc = 0;
    int length = 7;
    int past_clicks[length];
    for(i = 0; i < length; i++) {
        past_clicks[i] = 0;
    }
    lcd_locate(1,1);
    print_names();
    lcd_printf("%d %#005x", 0,0);
    int previous_vote = 0;
	while(1){
        if(BTN2_PRESSED()) {
            SETLED(LED2_PORT);
        } else {
            CLEARLED(LED2_PORT);
        }
        
        if(BTN1_PRESSED()) {
            SETLED(LED1_PORT);
        } else {
            CLEARLED(LED1_PORT);
        }
        if(BTN1_PRESSED() != BTN2_PRESSED()) {
            SETLED(LED3_PORT);
        } else {
            CLEARLED(LED3_PORT);
        }
        
        push(BTN1_PRESSED(),past_clicks, length);
        
        int current_vote = vote(past_clicks,length);
        if(current_vote != previous_vote) {
            if(current_vote) {
                inc++;
                lcd_clear_row(4);
                __delay_ms(10);
                lcd_locate(1,4);
                __delay_ms(10);
                lcd_printf("%d %#005x", inc,inc);
            }
            previous_vote = current_vote;
        }
	}
    
    
    
    return 0;
}


