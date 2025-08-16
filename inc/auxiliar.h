/*********************************************************************************************************************
Copyright (c) 2025, Gustavo Leonel Juarez <leonellj01@gmail.com>
Copyright (c) 2025, Laboratorio de microprocesadores, Universidad Nacional de Tucumán, Argentina

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

#ifndef AUXILIAR_H_
#define AUXILIAR_H_

/** @file auxiliar.h
 ** @brief Declaraciones de funciones auxiliares.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Reorganiza los dígitos BCD de la hora y los minutos.
 *
 * @param time  Puntero a la estructura de tiempo.
 * @param digits Array donde se almacenarán los dígitos BCD.
 *
 * @note Esta función pasa de la estructura de tiempo a un arreglo de dígitos BCD.
 */
void GetHourMinuteBCD(clockTimeT * time, uint8_t digits[]);

/**
 * @brief Reorganiza los dígitos BCD de la hora y los minutos.
 *
 * @param time  Puntero a la estructura de tiempo.
 * @param digits Array donde se almacenarán los dígitos BCD.
 *
 * @note Esta función pasa de un arreglo de dígitos BCD a la estructura de tiempo.
 */
void SetHourMinuteBCD(clockTimeT * time, uint8_t digits[]);

/**
 * @brief Obtiene el máximo valor de unidades para una determinada decena.
 *
 * @param tens  Valor de la decena.
 * @param max_tens  Valor máximo de la decena.
 * @param max_units  Valor máximo de las unidades.
 * @return uint8_t  Valor máximo de las unidades.
 */
uint8_t GetMaxUnits(uint8_t tens, uint8_t max_tens, uint8_t max_units);

/**
 * @brief Incrementa un valor BCD de unidades y decenas.
 *
 * @param units  Puntero al valor de las unidades.
 * @param tens  Puntero al valor de las decenas.
 * @param max_units  Valor máximo de las unidades.
 * @param max_tens  Valor máximo de las decenas.
 * @return true  Si se produjo un desbordamiento.
 * @return false  Si no hubo desbordamiento.
 */
bool BcdIncrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens);

/**
 * @brief Decrementa un valor BCD de unidades y decenas.
 *
 * @param units  Puntero al valor de las unidades.
 * @param tens  Puntero al valor de las decenas.
 * @param max_units  Valor máximo de las unidades.
 * @param max_tens  Valor máximo de las decenas.
 */
void BcdDecrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* AUXILIAR_H_ */
