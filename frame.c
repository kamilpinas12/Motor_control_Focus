/*
 * frame.c
 *
 *  Created on: Jun 28, 2025
 *      Author: kamil
 */

#include "frame.h"


// na razie blokująco
static void uart_send(motor_typedef *motor, uint8_t *data, uint16_t length) {
	HAL_GPIO_WritePin(motor->tx_en_port, motor->tx_en_pin, 1);
    HAL_UART_Transmit(motor->huart, data, length, HAL_MAX_DELAY);
}

static void uart_receive(motor_typedef *motor, uint8_t *buffer, uint16_t length) {
	HAL_GPIO_WritePin(motor->tx_en_port, motor->tx_en_pin, 0);
    HAL_UART_Receive(motor->huart, buffer, length, HAL_MAX_DELAY);
}


static uint8_t calculate_crc(uint8_t *data, uint8_t length) {
    uint16_t crc_sum = 0;
    for (int i = 0; i < length - 1; i++) {
        crc_sum += data[i];
    }
    return crc_sum % 256;
}




void motor_set_state(motor_typedef *motor, uint8_t controller_state, uint8_t state) {
    uint8_t frame[6];

    frame[0] = 6;                        // Długość
    frame[1] = motor->address;          // Adres
    frame[2] = 0x20;                    // Rozkaz: ustaw stan
    frame[3] = controller_state;      //numer wewnętrznej maszyny stanów(kontrollera)
    frame[4] = state;                   // Nowy stan
    frame[5] = calculate_crc(frame, 6); // CRC

    uart_send(motor, frame, 6);
}


uint8_t motor_get_state(motor_typedef *motor, uint8_t controller_state) {
    uint8_t request[5];

    request[0] = 5;                        // Długość
    request[1] = motor->address;
    request[2] = 0x1F;                    // Rozkaz: pobierz stan
    request[3] = controller_state;
    request[4] = calculate_crc(request, 5);

    uart_send(motor, request, 5);

    // Odbierz odpowiedź
    uint8_t response[5];
    uart_receive(motor, response, 5);

    // Sprawdź poprawność odpowiedzi
    if (response[0] != 5 || response[1] != 0xFF || response[2] != 0x1F) {
        return 0xFF; // Niewłaściwa ramka
    }

    if (calculate_crc(response, 5) != response[4]) {
        return 0xFF; // Błąd CRC
    }

    return response[3];
}


void motor_set_memory(motor_typedef *motor, uint8_t controller_state, uint8_t start_cell, int32_t *values, uint8_t num_values) {
    if (num_values > 62) return; // Maksymalna liczba intów
    if (start_cell > 64) return; // adres poza zakresem

    uint8_t length = num_values * 4 + 7;
    uint8_t frame[255];  // Bufor na całą ramkę

    frame[0] = length;
    frame[1] = motor->address;
    frame[2] = 0x2D;             // Rozkaz: ustaw pamięć
    frame[3] = num_values;
    frame[4] = start_cell;
    frame[5] = controller_state;


    /*
     Chyba nie ma doprecyzowane w jakiej kolejności wysyłane sa poszczególne bajty inta 32
     więc trzeba zapytać !!!
    */
    for (uint8_t i = 6; i < num_values; i++) {
        int32_t val = values[i];
        frame[i * 4 + 0] = (val >> 24) & 0xFF;
        frame[i * 4 + 1] = (val >> 16) & 0xFF;
        frame[i * 4 + 2] = (val >> 8)  & 0xFF;
        frame[i * 4 + 3] = (val >> 0)  & 0xFF;
    }

    frame[length - 1] = calculate_crc(frame, length);

    uart_send(motor, frame, length);
}

