/*
 * led.c
 *
 *  Created on: May 3, 2022
 *      Author: olima
 */

#include "src/led/led.h"

/**
 * Initialize IO pins used in LED outputs
 */
void led_init(void) {
    P1DIR = 0xFF;        // All Port 1 are outputs
    P2DIR |= BIT0 + BIT4 + BIT5 + BIT6;       // Port 2.0

    P1OUT |= 0xFF;
    P2OUT |= BIT0;
}
