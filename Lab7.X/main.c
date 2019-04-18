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

float clamp(int16_t val,int16_t min, int16_t max) {
//    if(val < min)
//        val = min;
//    
//    if(val > max)
//        val=max;
//    
    return ((float)(val - min)) /((float)(max-min));
}


float Kp = 0.5;


float proportional(float err) {
    return err - 0.5;
}
float smoothed = 0;
float last_error = 0;
float Kd = 0;

float derivative(float err) {
    smoothed = (err - last_error)*1 + smoothed*0;
    last_error = err;
    return smoothed;
}

#define N 100
float errors[N];

void push(float new_item, float* array, int length) {
    int i;
    for(i = 1;i < length; i++)
        array[i-1] = array[i];
    array[length-1] = new_item;
}

void init_errors() {
    int i;
    for(i = 0; i < N; i++)
        errors[i] = 0;
}
float last_integral = 0;

float Ki = 0;
float integrative(float err) {
    float avg = 0;
    int i = 0;
    for(i=0; i<N; i++)
        avg+=errors[i];
    
    avg = avg/((float)(N));
    
    push(err - 0.5,errors, N);
    
    return avg;
}

void control(float prop, float deriv, float integ) {
    motor_set_duty(8,smoothed_to_width((Kp*prop + Kd*deriv + Ki*integ + 0.5)));
}

void setup_timer1() {
    __builtin_write_OSCCONL(OSCCONL | 2);
    T1CONbits.TON = 0; //Disable Timer
    T1CONbits.TCS = 1; //Select external clock
    T1CONbits.TSYNC = 0; //Disable Synchronization
    T1CONbits.TCKPS = 0b00; //Select 1:1 Prescaler
    TMR1 = 0x00; //Clear timer register
    PR1 = 32*50; //Load the period value
    IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
    IEC0bits.T1IE = 1;// Enable Timer1 interrupt
    T1CONbits.TON = 1;// Start Timer
}

float smoothed_x = 0.5;
float smoothed_y = 0.5;
uint16_t x_things = 0;
uint16_t y_things = 0;

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    xy xy_read = touch_read();
    float x = clamp(xy_read.x,280,2800);
    smoothed_x = x*1 + 0*smoothed_x;
//    
//    float y = clamp(xy_read.y,400,2550);
//    smoothed_y = y*1 + 0*smoothed_y;
    control(proportional(smoothed_x),derivative(smoothed_x),integrative(smoothed_x));
    x_things = xy_read.x;
    y_things = xy_read.y;
    IFS0bits.T1IF = 0;        // clear the interrupt flag
}


int main(){
	/* LCD Initialization Sequence */
	__C30_UART=1;	
	lcd_initialize();
    
    
    led_initialize();
	lcd_clear();
    lcd_locate(1,1);    
    lcd_printf_d("set_value: 0.5");
    __delay_ms(200);
    lcd_locate(1,2);
    lcd_printf_d("Kp: %4f",Kp);
    lcd_locate(1,3);
    lcd_printf_d("Kd: %4f",Kd);
    lcd_locate(1,4);
    lcd_printf_d("Ki: %4f",Ki);
    touch_init();
    init_errors();
    AD1PCFGHbits.PCFG20 = 1;Nop();
    setup_timer1();
    motor_init(7);
    motor_init(8);
    motor_set_duty(7, angle_to_width(1));
//    motor_set_duty(8, angle_to_width(0.5));
    motor_set_duty(8,angle_to_width(0.5));
	while(1){
        lcd_locate(1,0);
        lcd_printf("Px: %4u, %4u",x_things, y_things);
        __delay_ms(60);
//        lcd_locate(1,6);
//        lcd_printf_d("Dx: %4f",smoothed);
//        lcd_locate(1,7);
//        float avg = 0;
//        int i = 0;
//        for(i=0; i<N; i++)
//            avg+=errors[i];
//
//        avg = avg/((float)(N));
//        lcd_printf_d("Ix: %4f",avg);
//        lcd_locate(1,0);
//        lcd_printf_d("Fx: %4f",(Kp*proportional(smoothed_x) + Kd*derivative(smoothed_x) + Ki*avg));
	}
    
    
    
    return 0;
}


