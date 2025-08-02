/*
 * frame.c
 *
 *  Created on: Jun 28, 2025
 *      Author: kamil
 */

#include "frame.h"


#define UART_TIMEOUT 100



static HAL_StatusTypeDef uart_send(motor_t *motor, uint8_t *data, uint16_t length) {
	HAL_GPIO_WritePin(motor->tx_en_port, motor->tx_en_pin, 1);
    return HAL_UART_Transmit(motor->huart, data, length, UART_TIMEOUT);
}


static HAL_StatusTypeDef uart_receive(motor_t *motor, uint8_t *buffer, uint16_t length) {
	HAL_GPIO_WritePin(motor->tx_en_port, motor->tx_en_pin, 0);
	return HAL_UART_Receive(motor->huart, buffer, length, UART_TIMEOUT);
}


static uint8_t calculate_crc(uint8_t *data, uint8_t length){
    uint16_t crc_sum = 0;
    for (int i = 0; i < length - 1; i++) {
        crc_sum += data[i];
    }
    return crc_sum % 256;
}



HAL_StatusTypeDef motor_set_state(motor_t *motor, uint8_t controller_state, uint8_t state) {
    uint8_t frame[6];

    frame[0] = 6;
    frame[1] = motor->address;
    frame[2] = 0x20;
    frame[3] = controller_state;
    frame[4] = state;
    frame[5] = calculate_crc(frame, 6);

    return uart_send(motor, frame, 6);
}


uint8_t motor_get_state(motor_t *motor, uint8_t controller_state) {
    uint8_t request[5];

    request[0] = 5;
    request[1] = motor->address;
    request[2] = 0x1F;
    request[3] = controller_state;
    request[4] = calculate_crc(request, 5);

    uart_send(motor, request, 5);

    uint8_t response[5];
    uart_receive(motor, response, 5);

    if (response[0] != 5 || response[1] != 0xFF || response[2] != 0x1F) {
        return 0xFF;
    }

    if (calculate_crc(response, 5) != response[4]) {
        return 0xFF;
    }

    return response[3];
}


HAL_StatusTypeDef motor_set_memory(motor_t *motor, uint8_t controller_state, uint8_t start_cell, int32_t* values, uint8_t num_values) {
    if (num_values > 62) return HAL_ERROR;
    if (start_cell > 64) return HAL_ERROR;

    uint8_t length = num_values * 4 + 7;
    uint8_t frame[255];

    frame[0] = length;
    frame[1] = motor->address;
    frame[2] = 0x2D;
    frame[3] = num_values;
    frame[4] = start_cell;
    frame[5] = controller_state;


    for (uint8_t i = 0; i < num_values; i++) {
        int32_t val = values[i];
        frame[6 + 4*i] = (val >> 24) & 0xFF;
        frame[6 + 4*i + 1] = (val >> 16) & 0xFF;
        frame[6 + 4*i + 2] = (val >> 8)  & 0xFF;
        frame[6 + 4*i + 3] = (val >> 0)  & 0xFF;
    }

    frame[length - 1] = calculate_crc(frame, length);

    return uart_send(motor, frame, length);
}



uint32_t motor_get_memory(motor_t* motor, uint8_t start_cell, uint8_t controller_state){
	uint8_t frame[7];

	frame[0] = 7;
	frame[1] = motor->address;
	frame[2] = 0x34;
	frame[3] = 1;
	frame[4] = start_cell;
	frame[5] = controller_state;
	frame[6] = calculate_crc(frame, 5);

	HAL_StatusTypeDef result = uart_send(motor, frame, 7);
	if(result != HAL_OK) return result;

	uint8_t response[8];

	result = uart_receive(motor, response, 8);
	if(result != HAL_OK) return result;

	if(response[0] != 8 || response[1] != 0xff || response[2] != 0x34 || response[7] != calculate_crc(response, 7)){
		return HAL_ERROR;
	}

	return ((uint32_t)response[0] << 24) |
						((uint32_t)response[1] << 16) |
						((uint32_t)response[2] << 8) |
						((uint32_t)response[3]);

}



