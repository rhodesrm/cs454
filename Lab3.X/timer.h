/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab3 timer functions header       */
/*                                                  */
/****************************************************/

#include <p33Fxxxx.h>
#include "types.h"

//period in ms
void setup_timer(uint8_t* timed_out);
void turn_off_timer();
