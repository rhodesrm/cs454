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


void setup_timer1() {
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
}

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    TOGGLELED(LED2_PORT);
    IFS0bits.T1IF = 0;        // clear the interrupt flag
}


void setup_timer2() {
    //runs every 50ms
    CLEARBIT(T2CONbits.TON);      // Disable Timer
    CLEARBIT(T2CONbits.TCS);      // Select internal instruction cycle clock
    CLEARBIT(T2CONbits.TGATE);    // Disable Gated Timer mode
    TMR2 = 0x00;            // Clear timer register
    T2CONbits.T32 = 0; 
    T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
    PR2 = 10000;             // Load the period value
    IPC0bits.T1IP1 = 0x01;   // Set Timer2 Interrupt Priority Level
    CLEARBIT(IFS0bits.T2IF);      // Clear Timer2 Interrupt Flag
    SETBIT(IEC0bits.T2IE);      // Enable Timer2 interrupt
    SETBIT(T2CONbits.TON);      // Start Timer
}

void __attribute__((__interrupt__)) _T2Interrupt(void)
{
    TOGGLELED(LED1_PORT);
    IFS0bits.T2IF = 0;        // clear the interrupt flag
}

void setup_timer3() {
    //every ms
    CLEARBIT(T3CONbits.TON);      // Disable Timer
    CLEARBIT(T3CONbits.TCS);      // Select internal instruction cycle clock
    CLEARBIT(T3CONbits.TGATE);    // Disable Gated Timer mode
    TMR3 = 0x00;            // Clear timer register
    T3CONbits.TCKPS = 0b00; // Select 1:64 Prescaler
    PR3 = 64*200;             // Load the period value
    IPC0bits.T1IP2 = 0x01;   // Set Timer1 Interrupt Priority Level
    CLEARBIT(IFS0bits.T3IF);      // Clear Timer3 Interrupt Flag
    SETBIT(IEC0bits.T3IE);      // Enable Timer3 interrupt
    SETBIT(T3CONbits.TON);      // Start Timer
}

uint16_t milliseconds = 0;
uint16_t seconds = 0;


void __attribute__((__interrupt__)) _T3Interrupt(void)
{
    if(milliseconds % 1000 == 0) {
        seconds++;
    }
        
    milliseconds++;
    IFS0bits.T3IF = 0;        // clear the interrupt flag
}

void setup_button() {
    AD1PCFGHbits.PCFG20 = 1;Nop(); //ADC1 Port Configuration Register High Channel 20
    TRISEbits.TRISE8 = 1;Nop();  //PortE Pin 8 I/O Configuration
    IEC1bits.INT1IE = 1; //Interrupt Enable Control Register 1
    //External Interrupt 1 Enable bit
    IPC5bits.INT1IP = 0x02;  //Interrupt Priority Control Register 5
    //External Interrupt 1 Priority bits
    CLEARBIT(IFS1bits.INT1IF);  //Interrupt Flag Status Register 1
    INTCON2bits.INT1EP = 1; /*Interrupt Control Register 2 External
    Interrupt 1 Edge Detect Polarity Select bit*/
}

uint16_t button_presses = 0;
uint16_t curr_s = 0;
uint16_t curr_m = 0;

void __attribute__((__interrupt__)) _INT1Interrupt(void) {
    milliseconds = 0;
    seconds = 0;
    curr_s = 0;
    curr_m = 0;
    CLEARBIT(IFS1bits.INT1IF);
}

int main(){
	/* LCD Initialization Sequence */
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
    led_initialize();
    setup_button();

    setup_timer1();
    setup_timer2();
    setup_timer3();
    
    IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
    IFS0bits.T1IF = 0;     // Clear Timer1 Interrupt Flag
    IEC0bits.T1IE = 1;     // Enable Timer1 interrupt

    int count = 0;
    while(1) {
        uint16_t old = TMR3;
        TOGGLELED(LED4_PORT);        
        __delay_us(10);
        count++;
        
        uint16_t diff = TMR3 - old;
        
        if(count == 20000) {
            lcd_locate(1,1);
            lcd_printf("%u cycles\r\n",diff);
            __delay_ms(25);
            lcd_printf("%0.4f ms\r\n",diff/(12.8*1e3));
            __delay_ms(25);
            lcd_printf("%um:%us.%ums",(curr_s/60)%60,curr_s%60,curr_m%1000);
            __delay_ms(25);
            count = 0;
            curr_s = seconds;
            curr_m = milliseconds;
        }
    }
    
    return 0;
}


