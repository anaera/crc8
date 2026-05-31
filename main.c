/**
 * @file main.c
 * @author Chiper
 *
 * Пример использования всех реализаций CRC-8.
 */

#include "crc8.h"

uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x39};
#define DATA_LEN sizeof(data)

void main(void)
{
	// --- Инициализация всех таблиц ---
	init_crc8_byte_matrix();
	init_crc8_bit_matrix();
	init_1wire_table();
	init_crc8_byte_table();
	
	uint16_t i;
	uint8_t crc;
	//--- Демонстрация реализаций CRC-8
	// Пример 1: Побитовый расчет через вспомогательную функцию 
	crc = INIT_CRC;
	for (i = 0; i < DATA_LEN; i++) {
		crc = make_crc8_byte(crc, data[i]);
	}
	// Пример 2: Побайтный расчет
	crc = INIT_CRC;
	for (i = 0; i < DATA_LEN; i++) {
		crc = calc_crc8_byte(crc, data[i]);
	}
	//Пример 3: Побайтовый расчет с матрицей (всего 8 байта на таблицу)
	crc = INIT_CRC;
	for (i = 0; i < DATA_LEN; i++) {
		crc = calc_crc8_matrix(crc, data[i]);
	}
	// Пример 4: Побайтовый расчет с таблицей (256 байт)
	crc = INIT_CRC;
	for (i = 0; i < DATA_LEN; i++) {
		crc = calc_crc8_table(crc, data[i]);
	}
	// Пример 4: Рефлексный алгоритм (1-Wire)
	crc = INIT_CRC;
	for (i = 0; i < DATA_LEN; i++) {
		crc = crc8_1wire(crc, data[i]);
	}
	// Пример 5: Утилита проверки блока данных
	crc = INIT_CRC;
	crc = check_crc8(data, DATA_LEN);
	NOP();
}