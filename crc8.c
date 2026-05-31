/**
 * @file crc8.c
 * @author Chiper
 *
 * Реализация всех функций для расчета CRC-8.
 */

#include "crc8.h"

/* --- Глобальные таблицы (статические) --- */
static uint8_t byte_table[256]; // Таблица для быстрого побайтового расчета
static uint8_t byte_matrix[8]; // Матрица для компактного побайтового расчета
static uint8_t bit_matrix[8]; // Матрица для битовых операций
static uint8_t tab_1wire[8]; // Таблица для рефлексного алгоритма

/* === Классический (нерефлексный) алгоритм === */

// Базовая операция: сдвиг влево и проверка выдвинутого бита.
// Конструкция ((crc ^ msb_bit) & 0x80) позволяет совместить сдвиг и проверку.

uint8_t calc_crc8_bit(uint8_t crc, uint8_t msb_bit)
{
	return((uint8_t) (crc << 1) ^ (((crc ^ msb_bit) & 0x80) ? STD_POLY : 0));
}

// Побайтовый расчет без таблиц. Медленный, но полностью прозрачный.

uint8_t calc_crc8_byte(uint8_t crc, uint8_t byte)
{
	crc ^= byte; // Математически обоснованное "внесение" байта в регистр.
	for (int j = 0; j < 8; j++) {
		if (crc & 0x80) {
			crc = (uint8_t) ((crc << 1) ^ STD_POLY);
		} else {
			crc <<= 1;
		}
	}
	return crc;
}

// Инициализация 256-байтной таблицы для максимальной скорости.

void init_crc8_byte_table(void)
{
	for (int i = 0; i < 256; i++) {
		byte_table[i] = calc_crc8_byte(0, (uint8_t) i);
	}
}

uint8_t calc_crc8_table(uint8_t crc, uint8_t byte)
{
	// Оптимизация: вместо побитовой обработки, используем предвычисленную таблицу.
	// Математически: new_crc = (crc << 8 + byte) mod poly.
	// Это эквивалентно: new_crc = table[ (crc ^ byte) ].
	crc = crc ^ byte;
	return byte_table[crc];
}

// Инициализация матрицы из 8 байт. Использует свойство линейности.

void init_crc8_byte_matrix(void)
{
	uint8_t wrk = 0x80;
	for (int i = 0; i < 8; i++) {
		byte_matrix[i] = calc_crc8_byte(0, wrk);
		wrk >>= 1;
	}
}
// Побайтовый расчет без таблиц. Медленный, но полностью прозрачный.

uint8_t calc_crc8_matrix(uint8_t crc, uint8_t byte)
{
	uint8_t x = crc ^ byte;
	uint8_t result = 0;
	for (int i = 0; i < 8; i++) {
		if (x & (0x80 >> i)) {
			result ^= byte_matrix[i];
		}
	}
	return result;
}

// Вспомогательная функция: расчет CRC для байта через битовую фукцию calc_crc8_biе().

uint8_t make_crc8_byte(uint8_t crc, uint8_t byte)
{
	for (int j = 0; j < 8; j++) {
		uint8_t bit = (byte & 0x80) ? 0x80 : 0;
		crc = calc_crc8_bit(crc, bit);
		byte <<= 1;
	}
	return crc;
}

void init_crc8_bit_matrix(void)
{
	uint8_t wrk = 0x01;
	for (int i = 0; i < 8; i++) {
		bit_matrix[i] = make_crc8_byte(0, wrk);
		wrk <<= 1;
	}
}


/* === Рефлексный алгоритм (1-Wire) === */

// Алгоритм со сдвигом вправо. Используется в протоколах типа Dallas 1-Wire.

uint8_t crc8_1wire(uint8_t crc, uint8_t byte)
{
	crc ^= byte;
	for (int j = 0; j < 8; j++) {
		if (crc & 0x01) { // Проверка младшего бита
			crc = (crc >> 1) ^ INV_POLY;
		} else {
			crc >>= 1;
		}
	}
	return crc;
}

void init_1wire_table(void)
{
	uint8_t wrk = 0x01;
	for (int i = 0; i < 8; i++) {
		tab_1wire[i] = crc8_1wire(0, wrk);
		wrk <<= 1;
	}
}


/* === Утилиты === */

// Проверка CRC для блока данных. Возвращает финальный остаток.

uint8_t check_crc8(uint8_t *data, uint8_t len)
{
	uint8_t crc = INIT_CRC;
	for (int i = 0; i < len; i++) {
		crc = calc_crc8_byte(crc, data[i]); // Можно заменить на любую другую реализацию
	}
	return crc;
}