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
	//communication parameters
	UART_HandleTypeDef *huart;
	GPIO_TypeDef* tx_en_port;
	uint16_t tx_en_pin;

	uint8_t address;


}motor_t;





HAL_StatusTypeDef motor_set_state(motor_t* motor, uint8_t controller_state, uint8_t state);

uint8_t motor_get_state(motor_t* motor, uint8_t controller_state);

HAL_StatusTypeDef motor_set_memory(motor_t* motor, uint8_t controller_state, uint8_t start_cell, int32_t *values, uint8_t num_values);

int32_t motor_get_memory(motor_t* motor, uint8_t start_cell, uint8_t controller_state);



HAL_StatusTypeDef motor_set_point(motor_t* motor, int32_t set_point);

int32_t motor_get_position(motor_t* motor);


//wartość napięcia na uzwojeniu podczas zatrzymania (0-1000)
HAL_StatusTypeDef motor_set_stop_voltage(motor_t* motor, uint32_t voltage);


//maksymalna wartość napięcia na uzwojeniu podczas ruchu  (0-1000)
//im większa tym silnik będzie się kręcił szybciej
HAL_StatusTypeDef motor_set_move_voltage(motor_t* motor, uint32_t voltage);







#endif /* INC_FRAME_H_ */
