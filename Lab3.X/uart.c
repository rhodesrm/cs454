/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab3 UART2 init,TX,RX functions   */
/*                                                  */
/****************************************************/

#include <p33FJ256MC710.h>

#include "uart.h"
#include "timer.h"

inline void uart2_init(uint16_t baud)
{
	CLEARBIT(U2MODEbits.UARTEN);  //Disable UART for configuration
/* Disable Interrupts */
    IEC1bits.U2RXIE = 0;
    IEC1bits.U2TXIE = 0;
    /* Clear Interrupt flag bits */
    IFS1bits.U2RXIF = 0;
    IFS1bits.U2TXIF = 0;

    /* Set IO pins */
    TRISFbits.TRISF4 = 1;        //set as input UART2 RX pin
    TRISFbits.TRISF5 = 0;        //set as output UART2 TX pin
    /* baud rate */
    // use the following equation to compute the proper
    // setting for a specific baud rate
    U2MODEbits.BRGH = 0;                    //Set low speed baud rate
    U2BRG  = (uint32_t)800000 / baud -1;    //Set the baudrate to be at 9600
    /* Operation settings and start port */
    U2MODE = 0; // 8-bit, no parity and, 1 stop bit
    U2MODEbits.RTSMD = 0;   //select simplex mode
    U2MODEbits.UEN = 0;     //select simplex mode
    U2MODE |= 0x00;
    U2MODEbits.UARTEN = 1;  //enable UART
    U2STA = 0;
    U2STAbits.UTXEN = 1;    //enable UART TX
}

void uart2_send_8(int8_t data)
{
    if(data) {
        while (U2STAbits.UTXBF);
        U2TXREG = data;
        while(!U2STAbits.TRMT);
    }
	
    while (U2STAbits.UTXBF);
    U2TXREG = data;
    while(!U2STAbits.TRMT);
}

int8_t uart2_recv(uint8_t* data)
{
	if (U2STAbits.OERR) {
        U2STAbits.OERR = 0;
        return -1;
    }
    if (U2STAbits.URXDA) {
        *data = U2RXREG & 0x00FF;
        return 1;
    }
    return 0;
}

struct with_error uart_getc_with_timeout(char with_timeout) {
    int8_t received;
    uint8_t byte;
    uint8_t is_timeout = 0;
    struct with_error witherror;
    witherror.error = 0;
    if(with_timeout)
        setup_timer(&is_timeout);
    do {
        received = uart2_recv(&byte);
        if(is_timeout)
            break;
    } while(received!= 1);
    if(is_timeout)
        witherror.error = 1;
    else
        witherror.byte = byte;
    
    if(with_timeout)
        turn_off_timer();
    return witherror;
}

struct with_error uart_getc() {
    return uart_getc_with_timeout(1);
}

struct with_error2 read_crc() {
    struct with_error2 return_me;
    return_me.error = 0;
    struct with_error witherror1 = uart_getc();
    if(witherror1.error) {
        return_me.error = 1;
        return return_me;
    }
    struct with_error witherror2 = uart_getc();
    if(witherror2.error) {
        return_me.error = 1;
        return return_me;
    }
    uint8_t first_byte = witherror1.byte;
    uint8_t second_byte = witherror2.byte;
    uint16_t first = first_byte << 8;
    uint16_t second = second_byte;
    return_me.bytes = first | second;
    return return_me;
}