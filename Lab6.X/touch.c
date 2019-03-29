/*
 * File:   motor.c
 * Author: team-2b
 *
 * Created on March 22, 2019, 3:05 PM
 */


#include "touch.h"



void init_adc(void) {
    CLEARBIT(AD1PCFGLbits.PCFG15);     //set AD1 AN15 input pin as analog
    CLEARBIT(AD1PCFGLbits.PCFG9);     //set AD1 AN09 input pin as analog
    //Configure AD1CON1
    SETBIT(AD1CON1bits.AD12B);  //set 10b Operation Mode
    AD1CON1bits.FORM = 0;   //set integer output
    AD1CON1bits.SSRC = 0x7; //set automatic conversion
    //Configure AD1CON2
    AD1CON2 = 0;  //not using scanning sampling
    //Configure AD1CON3
    CLEARBIT(AD1CON3bits.ADRC);    //internal clock source
    AD1CON3bits.SAMC = 0x1F; //sample-to-conversion clock = 31Tad
    AD1CON3bits.ADCS = 0x2;  //Tad = 3Tcy (Time cycles)
    //Leave AD1CON4 at its default value
    //enable ADC
    SETBIT(AD1CON1bits.ADON);
}

void touch_init(void) {
    //set up the I/O pins E1, E2, E3 to be output pins
    CLEARBIT(TRISEbits.TRISE1); //I/O pin set to output
    CLEARBIT(TRISEbits.TRISE2); //I/O pin set to output
    CLEARBIT(TRISEbits.TRISE3); //I/O pin set to output
    init_adc();
}


int cmpfunc (const void * a, const void * b) {
   return ( *(uint16_t*)a - *(uint16_t*)b );
}

uint16_t get_median() {
    const int num_readings = 5;
    uint16_t readings[num_readings];
    int i;
    for(i = 0; i < num_readings; i++) {
        __delay_us(200);
        SETBIT(AD1CON1bits.SAMP);        //start to sample
        while(!AD1CON1bits.DONE);        //wait for conversion to finish
        CLEARBIT(AD1CON1bits.DONE);        //MUST HAVE! clear conversion done bit
        readings[i] = ADC1BUF0;
    }
    qsort(readings, num_readings, sizeof(uint16_t), cmpfunc);
    return readings[num_readings/2];    
}

xy touch_read(void) {
    xy xy_read;
    __delay_ms(20);
    //set up the I/O pins E1, E2, E3 so that the touchscreen X-coordinate pin
    //connects to the ADC read at AN15
    CLEARBIT(PORTEbits.RE1);
    SETBIT(PORTEbits.RE2);
    SETBIT(PORTEbits.RE3);
    
    AD1CHS0bits.CH0SA = 0x00F;        //set ADC to Sample AN15 pin
    xy_read.x = get_median();
    
    __delay_ms(20);
    
    SETBIT(PORTEbits.RE1);
    CLEARBIT(PORTEbits.RE2);
    
    CLEARBIT(PORTEbits.RE3);
    
    __delay_ms(20);
    
    AD1CHS0bits.CH0SA = 0x009;        //set ADC to Sample AN9 pin
    xy_read.y = get_median();
    __delay_ms(20);
    return xy_read;
}