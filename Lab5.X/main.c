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
#include "motor.h"

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

void init_joystick_adc(uint16_t pin) {
    //disable ADC
    CLEARBIT(AD2CON1bits.ADON);
    //initialize PIN
    SETBIT(TRISEbits.TRISE8);        //set TRISE RE8 to input
    CLEARBIT(pin);     //set AD1 AN20 input pin as analog
    //Configure AD1CON1
    CLEARBIT(AD2CON1bits.AD12B);  //set 10b Operation Mode
    AD2CON1bits.FORM = 0;   //set integer output
    AD2CON1bits.SSRC = 0x7; //set automatic conversion
    //Configure AD1CON2
    AD2CON2 = 0;  //not using scanning sampling
    //Configure AD1CON3
    CLEARBIT(AD2CON3bits.ADRC);    //internal clock source
    AD2CON3bits.SAMC = 0x1F; //sample-to-conversion clock = 31Tad
    AD2CON3bits.ADCS = 0x2;  //Tad = 3Tcy (Time cycles)
    //Leave AD1CON4 at its default value
    //enable ADC
    SETBIT(AD2CON1bits.ADON);
}

uint16_t sample(uint16_t pin) {
    AD2CHS0bits.CH0SA = pin;        //set ADC to Sample AN20 pin
    SETBIT(AD2CON1bits.SAMP);        //start to sample
    while(!AD2CON1bits.DONE);        //wait for conversion to finish
    CLEARBIT(AD2CON1bits.DONE);        //MUST HAVE! clear conversion done bit
    return ADC2BUF0;
}

void setup_timer1() {
        //Timer1 every sec
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

uint16_t min_x;
uint16_t max_x;
uint16_t min_y;
uint16_t max_y;

int inc = 0;

float clamp(uint16_t min, uint16_t max, uint16_t val) {
    if(val < min)
        val = min;
    
    if(val > max)
        val=max;
    
    return ((float)(val - min)) /((float)(max));
}

uint16_t get_x() {
    return angle_to_width(clamp(min_x,max_x, sample(0x004)));
}

uint16_t get_y() {
    return angle_to_width(clamp(min_y,max_y, sample(0x005)));
}


void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    
    if(inc == 0) {
        lcd_locate(1,1);
        lcd_printf("Joystick max x? %4u", sample(0x004));
    } else if(inc == 1) {
        lcd_locate(1,2);
        lcd_printf("Joystick min x? %4u", sample(0x004));
    } else if(inc == 2) {
        lcd_locate(1,3);
        lcd_printf("Joystick max y? %4u", sample(0x005));
    } else if(inc == 3) {
        lcd_locate(1,4);
        lcd_printf("Joystick min y? %4u", sample(0x005));
    } else if(inc == 4) {
        lcd_locate(1,5);
        uint16_t x = get_x();
        lcd_printf("pulse width x = %3u", x);
        motor_set_duty(8,x);
    } else if(inc == 5) {
        lcd_locate(1,6);
        uint16_t y = get_y();
        lcd_printf("pulse width y = %3u", y);
        motor_set_duty(7,y);
    }
    
    IFS0bits.T1IF = 0;        // clear the interrupt flag
}

int main(){
	/* LCD Initialization Sequence */
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
    motor_init(7);
    AD1PCFGHbits.PCFG20 = 1;Nop();
    
    init_joystick_adc(AD2PCFGLbits.PCFG4);
    init_joystick_adc(AD2PCFGLbits.PCFG5);
    setup_timer1();
   
    int length = 15;
    int past_clicks[length];
    int i;
    for(i = 0; i < length; i++) {
        past_clicks[i] = 0;
    }

    int previous_vote = 0;
	while(1){        
        push(BTN1_PRESSED(),past_clicks, length);
        
        int current_vote = vote(past_clicks,length);
        if(current_vote != previous_vote) {
            if(current_vote) {
                inc++;
                if(inc == 1)
                    max_x = sample(0x004);
                else if(inc == 2)
                    min_x = sample(0x004);
                else if(inc == 3)
                    max_y = sample(0x005);
                else if(inc == 4)
                    min_y = sample(0x005);    
            }
            previous_vote = current_vote;
        }
	}
    
    
    
    return 0;
}


