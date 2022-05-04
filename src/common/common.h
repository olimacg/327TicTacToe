/*
 * common.h
 *
 *  Created on: May 3, 2022
 *      Author: olima
 */

#ifndef SRC_COMMON_COMMON_H_
#define SRC_COMMON_COMMON_H_

#include <msp430.h>
#include <stdint.h>
#include "src/led/led.h"
#include "src/button/button.h"
#include "src/timer/timer.h"

#define PLAYER1 BIT4
#define PLAYER2 BIT5

uint8_t grid[3][3];
uint8_t pos;
uint8_t turn;
uint8_t num_turns;

void system_init(void);

uint8_t check_win(uint8_t player);

void animate_win(uint8_t player);

void animate_draw(void);

void system_reset(void);



#endif /* SRC_COMMON_COMMON_H_ */
