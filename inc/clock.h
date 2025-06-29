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

#ifndef CLOCK_H_
#define CLOCK_H_

/** @file clock.h
 ** @brief declaraciones de funciones para el manejo del reloj
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/// @brief  Acciones que se pueden realizar sobre la alarma del reloj.
typedef enum AlarmActions {
    ALARM_CANCEL,  //!< Cancelar la alarma
    ALARM_DISABLE, //!< Desactivar la alarma
    ALARM_ENABLE   //!< Habilitar la alarma
} AlarmActions;

typedef union {
    struct {
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    } time;
    uint8_t bcd[6];
} clockTimeT;

typedef struct clockS * clockT;

typedef void (*clockAlarmRingingT)(clockT clock);

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea un reloj con la cantidad de ticks por segundo especificada.
 *
 * @param ticksPerSecond  Cantidad de ticks por segundo que tendrá el reloj.
 * @return clockT  Retorna un puntero al reloj creado.
 */
clockT ClockCreate(uint16_t ticksPerSecond, clockAlarmRingingT function);

/**
 * @brief Obtiene la hora actual del reloj.
 *
 * @param clock  Referencia al objeto reloj del cual se desea obtener la hora.
 * @param currentTime  Puntero a una estructura donde se almacenará la hora actual.
 * @return true
 * @return false
 * @note Si el puntero `currentTime` es NULL, la función retornará false. Ademas determina si la hora es válida.
 */
bool ClockGetTime(clockT clock, clockTimeT * currentTime);

/**
 * @brief Establece la hora del reloj.
 *
 * @param clock  Referencia al objeto reloj donde se desea establecer la hora.
 * @param NewTime  Puntero a una estructura que contiene la nueva hora a establecer.
 * @return true
 * @return false
 * @note Si el puntero `NewTime` es NULL o la hora no es válida, la función retornará false.
 */
bool ClockSetTime(clockT clock, const clockTimeT * NewTime);

/**
 * @brief Registra un nuevo tick en el reloj.
 *
 * @param clock  Referencia al objeto reloj que recibe el nuevo tick.
 */
bool ClockNewTick(clockT clock);

/**
 * @brief Establece una alarma en el reloj.
 *
 * @param clock  Referencia al objeto reloj donde se desea establecer la alarma.
 * @param alarm  Puntero a una estructura que contiene la hora de la alarma.
 * @return true
 * @return false
 * @note Si el puntero `alarm` es NULL o la hora de la alarma no es válida, la función retornará false.
 */
bool ClockSetAlarm(clockT clock, const clockTimeT * alarm);

/**
 * @brief Obtiene la hora de la alarma del reloj.
 *
 * @param clock  Referencia al objeto reloj del cual se desea obtener la hora de la alarma.
 * @param alarm  Puntero a una estructura donde se almacenará la hora de la alarma.
 * @return true
 * @return false
 * @note Si el puntero `alarm` es NULL, la función retornará false.
 */
bool ClockGetAlarm(clockT clock, clockTimeT * alarm);

/**
 * @brief  Verifica si la alarma está activa en el reloj.
 *
 * @param clock  Referencia al objeto reloj que se desea verificar.
 * @return true
 * @return false
 */
bool ClockIsAlarmActive(clockT clock);

/**
 * @brief Realiza una acción sobre la alarma del reloj.
 *
 * @param clock  Referencia al objeto reloj sobre el cual se desea realizar la acción.
 * @param action  Acción a realizar sobre la alarma (cancelar, desactivar o habilitar).
 */
void ClockAlarmAction(clockT clock, AlarmActions action);

/**
 * @brief  Verifica si la alarma está habilitada en el reloj.
 *
 * @param clock  Referencia al objeto reloj que se desea verificar.
 * @return true
 * @return false
 */
bool ClockIsAlarmEnabled(clockT clock);

/**
 * @brief Pospone la alarma del reloj por una cantidad de minutos especificada.
 *
 * @param clock  Referencia al objeto reloj sobre el cual se desea posponer la alarma.
 * @param minutes  Cantidad de minutos por los cuales se desea posponer la alarma.
 */
void ClockSnoozeAlarm(clockT clock, uint8_t minutes);

/**
 * @brief  Verifica si la alarma del reloj está sonando.
 *
 * @param clock  Referencia al objeto reloj que se desea verificar.
 * @return true
 * @return false
 */
bool ClockAlarmRinging(clockT clock);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H */
