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

#ifndef SCREEN_H_
#define SCREEN_H_

/** @file screen.h
 ** @brief Declaraciones de funciones para el manejo del display de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define SEGMENT_A  (1 << 0)
#define SEGMENT_B  (1 << 1)
#define SEGMENT_C  (1 << 2)
#define SEGMENT_D  (1 << 3)
#define SEGMENT_E  (1 << 4)
#define SEGMENT_F  (1 << 5)
#define SEGMENT_G  (1 << 6)
#define SEGMENT_DP (1 << 7)

/* === Public data type declarations =============================================================================== */

typedef struct screenS * screenT;

typedef void (*digitsTurnOffT)(void);
typedef void (*digitTurnOnT)(uint8_t);
typedef void (*segmentsUpdatesT)(uint8_t,uint8_t);

typedef struct screenDriverS {
    digitsTurnOffT DigitsTurnOff;
    segmentsUpdatesT SegmentsUpdates;
    digitTurnOnT DigitTurnOn;
} const * screenDriverT;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función para crear una nueva instancia de la pantalla.
 *
 * @param digits    Número de dígitos del display.
 * @param driver    Puntero a la estructura que contiene las funciones del driver del display.
 * 
 * @return screenT Puntero a la nueva instancia de la pantalla.
 */
screenT ScreenCreate(uint8_t digits, screenDriverT driver);

/**
 * @brief Función para escribir un valor en formato BCD en la pantalla.
 *
 * @param self      Puntero a la instancia de la pantalla.
 * @param value     Puntero al arreglo que contiene los valores BCD a escribir.
 * @param size      Tamaño del arreglo de valores BCD.
 */
void ScreenWriteBCD(screenT self, uint8_t * value, uint8_t size, uint8_t *dots);

/**
 * @brief Función para refrescar la pantalla, actualizando el dígito actual.
 *
 * @param self  Puntero a la instancia de la pantalla.
 */
void ScreenRefresh(screenT self);

/**
 * @brief Función para parpadear los dígitos del display.
 *
 * @param screen        Puntero al descriptor de la pantalla con la que se va a trabajar.
 * @param from          Posición del primer dígito desde el cual se comenzará a parpadear.
 * @param to            Posición del último dígito hasta el cual se parpadeará.
 * @param frecuency     Frecuencia de parpadeo en milisegundos.
 * 
 * @return int Retorna 0 si la operación fue exitosa, -1 si hubo un error.
 */
int ScreenFlashDigits(screenT screen, uint8_t from, uint8_t to, uint16_t frecuency, uint8_t selector);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H_ */
