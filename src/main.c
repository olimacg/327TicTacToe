#include <msp430.h> 
#include <stdint.h>
#include "src/common/common.h"

/**
 * main.c
 *
 * TicTacToe Game
 */


// CA0-7 ==> P1.0-7
// CA8 ==> P2.0

// RED ==> P2.4
// GREEN ==> P2.5
// BLUE ==> P2.6

// Left Switch ==> P2.1
// Right Switch ==> P2.2


int main(void)
{
    // System initializations
    system_init();
	led_init();
	button_init();
	timer_init();


    while(1) {
        __bis_SR_register(LPM3_bits + GIE);
    }

	return 0;
}



