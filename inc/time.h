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

#ifndef TIME_H_
#define TIME_H_

/** @file time.h
 ** @brief Plantilla para la creación de archivos de de cabeceras en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "clock.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define TIME_VALUE_SIZE      sizeof(uint32_t)

#define STATE_VALUE_SIZE     sizeof(char)

#define TIME_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)

#define LAPS_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

typedef struct timeTaskArgS {
    EventGroupHandle_t events;
    uint8_t accept;
    uint8_t cancel;
    uint8_t increment;
    uint8_t decrement;
    uint8_t setTime;
    uint8_t setAlarm;
    QueueHandle_t elapsed;
    QueueHandle_t states;
    clockT clock;
} * timeTaskArgT;

typedef enum clockStates {
    UNCONFIGURED,        //!< Hora no válida al iniciar el reloj.
    SHOW_TIME,           //!< Muestra la hora actual.
    SET_CURRENT_MINUTES, //!< Establece los minutos actuales.
    SET_CURRENT_HOURS,   //!< Establece la hora actual.
    SET_ALARM_MINUTES,   //!< Establece los minutos de la alarma.
    SET_ALARM_HOURS,     //!< Establece la hora de la alarma.
} clockStates;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

void TimeTask(void *args);

void TimeRefresh(void * pointer);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* TIME_H_ */
