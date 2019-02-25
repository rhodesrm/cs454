//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>

#include <p33Fxxxx.h>
#include "types.h"
#include "uart.h"
#include "crc16.h"
#include "lab3.h"
#include "lcd.h"
#include "timer.h"

#define MAX_MSG 255
// Primary (XT, HS, EC) Oscillator without PLL
_FOSCSEL(FNOSC_PRIPLL);
// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystanl
_FOSC(OSCIOFNC_ON & POSCMD_XT);
// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);
// Disable Code Protection
_FGS(GCP_OFF);   

int16_t read_message(uint8_t supposed_msg_len, char* msg) {
    int16_t i;
    for(i = 0; i < MAX_MSG; i++) {
        struct with_error witherror = uart_getc();
        if(witherror.error)
            return -1;
        uint8_t byte = witherror.byte;
        msg[i] = byte;
        if(byte == 0)
            break;
    }
    if(msg[i] != 0 || i != supposed_msg_len)
        return -1;
    return i;
}


void sub_array(char* break_me, char* broken,int startI, int len) {
    int i;
    for(i = 0; i < len; i++)
        broken[i] = break_me[i+startI];
}

void print_long_msg(char* msg) {
    int start_line = 3;
    int i;
    for(i = 0; i < 5; i++) {
        char window[21];
        sub_array(msg,window,i*20,20);
        window[20] = 0;
        lcd_locate(0,start_line+i);
        lcd_printf_d("%s",window);
    }
        
}


char read_data(char* msg, uint16_t* returned_crc) {
    uint8_t start_byte;
    uint16_t crc_bytes;
    uint8_t msg_len_byte;
    
    struct with_error start = uart_getc_with_timeout(0);
    if(start.error)
        return 0;
    else
        start_byte = start.byte;
    if(start_byte != 0)
        return 0;
    
    struct with_error2 crc = read_crc();
    if(crc.error)
        return 0;
    else
        crc_bytes = crc.bytes;
        
    struct with_error msg_len = uart_getc();
    
    if(msg_len.error)
        return 0;
    else
        msg_len_byte = msg_len.byte;

    int i;
    for(i = 0; i< MAX_MSG; i++)
        msg[i] = 0;
    int16_t real_len = read_message(msg_len_byte,msg);
    if(real_len == -1)
        return 0;
    
    uint16_t real_crc = compute_crc(msg,real_len);
    if(real_crc != crc_bytes)
        return 0;
    
    (*returned_crc) = real_crc;
    
    return 1;
}

int main(void)
{	
    __C30_UART=1;
	lcd_initialize();
    lcd_clear();
	/* Q: What is my purpose? */
	/* A: You pass butter. */
	/* Q: Oh. My. God. */
    lcd_locate(0,0);
    lcd_printf_d("failures:0");
    uart2_init(9600);
    int num_failures = 0;
    while(1) {
        char msg[MAX_MSG];
        uint16_t crc;
        int8_t worked = read_data(msg,&crc);
        uart2_send_8(worked); 
        if(worked) {
            lcd_clear();
            lcd_locate(0,0);
            lcd_printf_d("failures:%d",num_failures);
            lcd_locate(0,1);
            lcd_printf_d("crc:%x",crc);
            lcd_locate(0,2);
            lcd_printf_d("msg:");
            print_long_msg(msg);
            num_failures = 0;
        }
        else {
            num_failures++;
            lcd_locate(0,0);
            lcd_printf_d("failures:%d",num_failures);
        }
    }
    lcd_printf_d("cheese");
    while(1) {}
}	

