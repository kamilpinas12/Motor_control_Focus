/*
 * frame.h
 *
 *  Created on: Jun 28, 2025
 *      Author: kamil
 */

#ifndef INC_FRAME_H_
#define INC_FRAME_H_

#include <stdint.h>
#include "main.h"
#include "stm32l4xx_hal.h"   // nie wiem czy to na pewno ta płytka


typedef struct {
	UART_HandleTypeDef *huart;
	GPIO_TypeDef* tx_en_port;
	uint16_t tx_en_pin;
	uint8_t address;

}motor_typedef;





void motor_set_state(motor_typedef *motor, uint8_t controller_state, uint8_t state);

uint8_t motor_get_state(motor_typedef *motor, uint8_t controller_state);

void motor_set_memory(motor_typedef *motor, uint8_t controller_state, uint8_t start_cell, int32_t *values, uint8_t num_values);





#endif /* INC_FRAME_H_ */
