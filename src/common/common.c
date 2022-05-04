/*
 * common.c
 *
 *  Created on: May 3, 2022
 *      Author: olima
 */


#include "src/common/common.h"

/**
 * Perform miscellaneous initializations and reset system variables
 */
void system_init(void) {

    BCSCTL3 |= LFXT1S_2;
    if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xff)
                while(1); // Erased calibration data? Trap!

        BCSCTL1 = CALBC1_1MHZ; // Set the DCO to 1 MHz
        DCOCTL = CALDCO_1MHZ; // And load calibration data


    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    system_reset();
}
/**
 * Check if move has resulted in a winning arrangement
 * for player
 *
 * Win: return 1
 * Lose: return 0
 */
uint8_t check_win(uint8_t player) {
    int i, j;
    uint8_t win;

    // Check horizontal
    for (i = 0; i < 3; i++) {
        win = 1;
        for (j = 0; j < 3; j++) {
            if (!(grid[i][j] & turn))
                win = 0;
        }
        if (win)
            return 1;

    }

    // Check vertical
        for (i = 0; i < 3; i++) {
            win = 1;
            for (j = 0; j < 3; j++) {
                if (!(grid[j][i] & turn))
                    win = 0;
            }
            if (win)
                return 1;

        }

    // Check diagonals
    win = 1;
    for (i = 0; i < 3; i++) {
        if (!(grid[i][i] & player))
            win = 0;
    }
    if (win)
        return 1;

    win = 1;
    for (i = 0; i < 3; i++) {
        if (!(grid[2 - i][i] & player))
            win = 0;
    }
    if (win)
        return 1;


    return 0;       // no winning arrangment found

}
/**
 * Play an LED animation to indicate that player has won.
 */
void animate_win(uint8_t player) {
    int i;

    CCTL0 &= ~CCIE;

    P2OUT &= ~(PLAYER1 + PLAYER2);
    P2OUT |= player;
    for (i = 0; i < 10; i++) {
        P1OUT = 0xFF;
        P2OUT |= BIT0;
        __delay_cycles(100000);
        P1OUT = ~0xFF;
       P2OUT &= ~BIT0;
       __delay_cycles(100000);
    }
    CCTL0 |= CCIE;
}

/**
 * Play an LED animation to indicate that the game has resulted
 * in a draw.
 */
void animate_draw(void) {

    int i, j, k;

    CCTL0 &= ~CCIE;
    P1OUT = 0xFF;
    P2OUT |= BIT0;
    P2OUT &= ~(PLAYER1 + PLAYER2);

    for (i = 0; i < 10; i++) {
            P2OUT &= ~PLAYER2;
            P2OUT |= PLAYER1;
           __delay_cycles(100000);
           P2OUT &= ~PLAYER1;
           P2OUT |= PLAYER2;
          __delay_cycles(100000);
       }

    CCTL0 = CCIE;
}

/**
 * Reset system variables
 */
void system_reset(void) {
    int i, j;
    for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                grid[i][j] = 0;
            }
        }
    pos = 0;
    turn = PLAYER1;
    num_turns = 0;
}


/**
 * Port 2 ISR
 *
 * Used to process button presses, and handles TicTacToe logic
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    uint8_t turn_done = 0;
    int i, j;
    uint8_t col = pos / 3;
    uint8_t row = pos - 3 * col;
    grid[row][col] &= ~turn;

    if (!(P2IN & BIT1)){    // If right button pressed
        P2IFG &= ~BIT1;
        __delay_cycles(40000);   // Allow for delay between possible press of right button
        if (!(P2IN & BIT2)) {   // If both buttons pressed
           P2IFG &= ~BIT2;
           turn_done = 1;       // Indicate turn is complete, used later
           num_turns++;

           while ((P2IN & BIT1) && (P2IN & BIT2)); // Wait for both buttons to be released


        }
        else {          // If only right button pressed
            while(1){   // Go to next available position
                if (++pos > 8)
                      pos = 0;
                col = pos / 3;
                row = pos - 3 * col;
                if ((grid[row][col] & (BIT4 + BIT5)) == 0)
                    break;
        }

        }

    } else if (!(P2IN & BIT2)) {    // If left button pressed
        P2IFG &= ~BIT2;
        __delay_cycles(40000);  // Allow for delay between possible press of right button
        if (!(P2IN & BIT1)) {   // If both buttons pressed
           P2IFG &= ~BIT1;
           turn_done = 1;       // Indicate turn is complete, used later
           num_turns++;
           while ((P2IN & BIT1) && (P2IN & BIT2));  // Wait for both buttons to be released
        } else {
            while(1){   // Go to next available position
                if(pos == 0)
                    pos = 8;
                else
                    pos--;
                col = pos / 3;
                row = pos - 3 * col;
                if ((grid[row][col] & (BIT4 + BIT5)) == 0)
                    break;
            }
        }
    }
    col = pos / 3;
    row = pos - 3 * col;
    grid[row][col] |= turn; // Update display for position

    if (turn_done) {        // If completed turn
        if (check_win(turn)) {  // Check if the move has caused a player to win
            animate_win(turn);
            system_reset();
            return;
        }
        if (num_turns > 8) {    // Check if the maximum number of moves has been player
            animate_draw();
            system_reset();
            return;
        }

        for(i = 0; i < 3; i++) {    // Start next go at first available position
           for(j = 0; j < 3; j++) {
               if ((grid[j][i] & (BIT4 + BIT5)) == 0) {
                   pos = 3 * i + j;
                   break;
               }
           }
       }
        if (turn == PLAYER1){       // Swap global turn variable to other player
            turn = PLAYER2;
        } else {
            turn = PLAYER1;
        }
    }

      __bic_SR_register_on_exit(LPM3_bits);
}


