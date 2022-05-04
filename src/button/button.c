/*
 * button.c
 *
 *  Created on: May 3, 2022
 *      Author: olima
 */

#include "src/button/button.h"

/**
 * Perform initializations for the use of the buttons
 */
void button_init(void) {
    P2REN |= BIT1 + BIT2;
    P2IE |= BIT1 + BIT2;
    P2IES |= BIT1 + BIT2;
    P2IFG &= ~(BIT1 + BIT2);
    P2OUT |= BIT1 + BIT2;
}


