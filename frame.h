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
#include "stm32l4xx_hal.h"


typedef struct {
	UART_HandleTypeDef *huart;
	GPIO_TypeDef* tx_en_port;
	uint16_t tx_en_pin;

	uint8_t address;


}motor_t;



HAL_StatusTypeDef motor_set_stop_voltage(motor_t* motor, uint32_t voltage);

HAL_StatusTypeDef motor_set_move_voltage(motor_t* motor, uint32_t voltage);

void motor_set_angle(motor_t* motor, float angle);



HAL_StatusTypeDef motor_set_state(motor_t* motor, uint8_t controller_state, uint8_t state);

uint8_t motor_get_state(motor_t* motor, uint8_t controller_state);

HAL_StatusTypeDef motor_set_memory(motor_t* motor, uint8_t controller_state, uint8_t start_cell, int32_t *values, uint8_t num_values);

int32_t motor_get_memory(motor_t* motor, uint8_t start_cell, uint8_t controller_state);



HAL_StatusTypeDef motor_set_point(motor_t* motor, int32_t set_point);

int32_t motor_get_position(motor_t* motor);






#endif /* INC_FRAME_H_ */
