/*
 * timer.c
 *
 *  Created on: May 3, 2022
 *      Author: olima
 */

#include "src/common/common.h"

/**
 * Initialize registers to configure timers
 */
void timer_init(void) {
    // Flashing indicator timer
    TA0CTL = TASSEL_1 + MC_1;     // ACLK, upmode
    CCR0 = 1000;                  // Period
    CCTL0 = CCIE;

    // LED Display sweep timer
    TA1CTL = TASSEL_1 + MC_1;     // ACLK, upmode
    TA1CCR0 = 10;                 // Period
    TA1CCTL0 = CCIE;
}

/**
 * Timer0 ISR
 *
 * Creates Blinking affect for current position marker
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    uint8_t col = pos / 3;
    uint8_t row = pos - 3 * col;
    grid[row][col] ^= turn;

    __bic_SR_register_on_exit(LPM3_bits);
}

/**
 * Timer1 ISR
 *
 * Handles multicolored display of entire grid
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1p0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A1p0 (void)
#else
#error Compiler not supported!
#endif
{
    int i, j;
    static uint8_t color_idx = 0;
    uint8_t curr_color;

    // Turn or all CAs
    P1OUT = 0;
    P2OUT &= ~BIT0;
    // Change color
    P2OUT &= ~(BIT4 << color_idx);
    if(++color_idx > 2)
        color_idx = 0;
    curr_color = BIT4 << color_idx;
    P2OUT |= curr_color;
    //Set appropriate CAs
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            if (!((i == 2) && (j == 2))) {
                if (grid[i][j] & curr_color)
                    P1OUT |= 1 << (3 * i + j);
            } else {
                if(grid[2][2] & curr_color)
                    P2OUT |= BIT0;
            }

        }
    }
    __bic_SR_register_on_exit(LPM3_bits);


}



