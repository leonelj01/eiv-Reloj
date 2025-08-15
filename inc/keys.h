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

#ifndef KEYS_H_
#define KEYS_H_

/** @file keys.h
 ** @brief Declaraciones de la biblioteca para gestionar las teclas del proyecto
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "digital.h"
#include "event_groups.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* event bits para teclas */
#define BTN_SET_TIME  (1 << 0)
#define BTN_SET_ALARM (1 << 1)
#define BTN_DECREMENT (1 << 2)
#define BTN_INCREMENT (1 << 3)
#define BTN_ACCEPT    (1 << 4)
#define BTN_CANCEL    (1 << 5)

#define KEY_EVENT_ANY_KEY   0xFF

#define KEY_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

typedef struct keyTaskArgS {
    EventGroupHandle_t events;
    uint32_t shortPulse;
    uint32_t longPulse;
    digitalInputT gpio;
} * keyTaskArgT;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

void KeyTask(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* KEYS_H_ */
