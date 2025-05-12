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

#ifndef DIGITAL_H
#define DIGITAL_H

/** @file digital.h
 ** @brief Declaraciones de funciones para el manejo de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include "config.h"
#include <stdint.h>
#include <stdbool.h>
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Representa una salidad digital
typedef struct digitalOutputS * digitalOutputT;

//! Representa una entrada digital
typedef struct digitalInputS * digitalInputT;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea un objeto de tipo digitalOutputT
 *
 * @param port  Puerto al que pertenece la salida
 * @param pin   Pin al que pertenece la salida
 * @param state Estado inicial de la salida
 * @return digitalOutputT Referencia a la salida digital
 */
digitalOutputT DigitalOutputCreate(uint8_t port, uint8_t pin, bool state);

/**
 * @brief Permite activar la salida digital
 *
 * @param self Referencia a la salida digital
 */
void DigitalOutputActivate(digitalOutputT self);

/**
 * @brief Permite desactivar la salida digital
 *
 * @param self Referencia a la salida digital
 */
void DigitalOutputDesactivate(digitalOutputT self);

/**
 * @brief Permite alternar el estado de la salida digital
 *
 * @param self Referencia a la salida digital
 */
void DigitalOutputToggle(digitalOutputT self);


/**
 * @brief Crea un objeto de tipo digitalInputT
 * 
 * @param port      Puerto al que pertenece la entrada
 * @param pin       Pin al que pertenece la entrada
 * @param inverted  Indica si la entrada es de logica invertida 
 * @return digitalInputT Referencia a la entrada digital 
 */
digitalInputT DigitalInputCreate(uint8_t port, uint8_t pin, bool inverted);

/**
 * @brief Permite obtener el estado de la entrada digital
 *
 * @param self Referencia a la entrada digital
 * @return bool Estado de la entrada digital
 * @note La función devuelve un True si la entrada está activa o False si está inactiva, sin importar si es de lógica invertida o no.
 */
bool DigitalInputGetState(digitalInputT self);


bool DigitalInputHasChanged(digitalInputT self);

void DigitalInputHasActivated(digitalInputT self);

void DigitalInputHasDesactivated(digitalInputT self);



/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_H */
