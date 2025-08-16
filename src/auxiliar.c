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

/** @file plantilla.c
 ** @brief Implementacion de funciones auxiliares para el reloj.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "auxiliar.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void GetHourMinuteBCD(clockTimeT * time, uint8_t digits[]) {
    if (time && digits) {
        digits[0] = time->bcd[5]; // Hora de decenas
        digits[1] = time->bcd[4]; // Hora de unidades
        digits[2] = time->bcd[3]; // Minuto de decenas
        digits[3] = time->bcd[2]; // Minuto de unidades
    }
}

void SetHourMinuteBCD(clockTimeT * time, uint8_t digits[]) {
    if (time && digits) {
        time->bcd[5] = digits[0]; // Hora de decenas
        time->bcd[4] = digits[1]; // Hora de unidades
        time->bcd[3] = digits[2]; // Minuto de decenas
        time->bcd[2] = digits[3]; // Minuto de unidades
        time->bcd[1] = 0;
        time->bcd[0] = 0;
    }
}

uint8_t GetMaxUnits(uint8_t tens, uint8_t max_tens, uint8_t max_units) {
    if (tens == max_tens) {
        // Caso especial: para horas, si tens=2, max_units=3
        // Para minutos, si tens=5, max_units=9
        return (max_tens == 2) ? 3 : 9; // Asume horas si max_tens=2
    }
    return 9; // Para decenas normales, unidades van de 0-9
}

bool BcdIncrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens) {
    (*units)++;

    // Obtener el límite real de unidades para la decena actual
    uint8_t current_max_units = GetMaxUnits(*tens, max_tens, max_units);

    if (*units > current_max_units) {
        *units = 0;
        (*tens)++;

        if (*tens > max_tens) {
            *tens = 0;
            *units = 0;
            return true;
        }
    }
    return false;
}

void BcdDecrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens) {
    if (*units > 0) {
        (*units)--;
    } else {
        if (*tens > 0) {
            (*tens)--;
            // Establecer unidades al máximo permitido para la nueva decena
            *units = GetMaxUnits(*tens, max_tens, max_units);
        } else {
            // Ir al valor máximo permitido
            *tens = max_tens;
            *units = GetMaxUnits(*tens, max_tens, max_units);
        }
    }
}
/* === End of documentation ======================================================================================== */
