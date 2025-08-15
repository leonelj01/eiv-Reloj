/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "clock.h"
#include "keys.h"
#include "time.h"
#include "display.h"
#include <stdbool.h>
#include <stdlib.h>

/* === Macros definitions ====================================================================== */

#define TOGGLE_DOT()                                                                                                   \
    ScreenToggleDot(board->screen, 0);                                                                                 \
    ScreenToggleDot(board->screen, 1);                                                                                 \
    ScreenToggleDot(board->screen, 2);                                                                                 \
    ScreenToggleDot(board->screen, 3);

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

boardT board;
clockT clock;
clockStates mode;
uint8_t digits[4];
bool dotsOn = false;

volatile uint32_t mseg = 0; // Variable para el tiempo en milisegundos

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
void AlarmRinging(clockT clock) {
    DigitalOutputActivate(board->ledRed);
}

void ChangeMode(clockStates value) {
    mode = value;

    switch (mode) {
    case UNCONFIGURED:
        ScreenFlashDigits(board->screen, 0, 3, 100);
        break;

    case SHOW_TIME:
        ScreenFlashDigits(board->screen, 0, 0, 0);

        // Apagar los puntos si estaban encendidos
        if (dotsOn) {
            TOGGLE_DOT();
            dotsOn = false;
        }
        break;

    case SET_CURRENT_MINUTES:
        ScreenFlashDigits(board->screen, 2, 3, 100);
        break;

    case SET_CURRENT_HOURS:
        ScreenFlashDigits(board->screen, 0, 1, 100);
        break;

    case SET_ALARM_MINUTES:
        ScreenFlashDigits(board->screen, 2, 3, 100);
        if (!dotsOn) {
            TOGGLE_DOT();
            dotsOn = true;
        }
        break;

    case SET_ALARM_HOURS:
        ScreenFlashDigits(board->screen, 0, 1, 100);
        if (!dotsOn) {
            TOGGLE_DOT();
            dotsOn = true;
        }
        break;

    default:
        break;
    }
}

// Función auxiliar para obtener el máximo valor de unidades según las decenas
uint8_t GetMaxUnits(uint8_t tens, uint8_t max_tens, uint8_t max_units) {
    if (tens == max_tens) {
        // Caso especial: para horas, si tens=2, max_units=3
        // Para minutos, si tens=5, max_units=9
        return (max_tens == 2) ? 3 : 9; // Asume horas si max_tens=2
    }
    return 9; // Para decenas normales, unidades van de 0-9
}

void BcdIncrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens) {
    (*units)++;

    // Obtener el límite real de unidades para la decena actual
    uint8_t current_max_units = GetMaxUnits(*tens, max_tens, max_units);

    if (*units > current_max_units) {
        *units = 0;
        (*tens)++;

        if (*tens > max_tens) {
            *tens = 0;
            *units = 0;
        }
    }
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

/* === Public function implementation ========================================================= */

int main(void) {
    QueueHandle_t dataTime;
    QueueHandle_t dataState;
    EventGroupHandle_t keyEvents;
    SemaphoreHandle_t mutex;

    board = BoardCreate();
    clock = ClockCreate(1000, AlarmRinging);

    dataTime = xQueueCreate(10, sizeof(clockTimeT));
    dataState = xQueueCreate(10, sizeof(clockStates));
    keyEvents = xEventGroupCreate();
    mutex = xSemaphoreCreateMutex();

    refreshTaskArgT refreshArgs = malloc(sizeof(struct refreshTaskArgS));
    refreshArgs->dataTime = dataTime;
    refreshArgs->dataState = dataState;
    refreshArgs->mutex = mutex;
    refreshArgs->display = board->screen;
    refreshArgs->clk = clock;

    timeTaskArgT timeArgs = malloc(sizeof(struct timeTaskArgS));
    timeArgs->events = keyEvents;
    timeArgs->accept = BTN_ACCEPT;
    timeArgs->cancel = BTN_CANCEL;
    timeArgs->setTime = BTN_SET_TIME;
    timeArgs->setAlarm = BTN_SET_ALARM;
    timeArgs->increment = BTN_INCREMENT;
    timeArgs->decrement = BTN_DECREMENT;
    timeArgs->elapsed = dataTime;
    timeArgs->states = dataState;
    timeArgs->clock = clock;

    keyTaskArgT keyArgs = malloc(sizeof(struct keyTaskArgS));
    keyArgs->events = keyEvents;
    keyArgs->gpio = board->setTime;
    keyArgs->shortPulse = BTN_SET_TIME;

    xTaskCreate(RefreshTask, "RefreshTask", configMINIMAL_STACK_SIZE, refreshArgs, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(TimeTask, "TimeTask", configMINIMAL_STACK_SIZE, timeArgs, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(DisplayTask, "DisplayTask", configMINIMAL_STACK_SIZE, refreshArgs, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(TimeRefresh, "TimeRefresh", configMINIMAL_STACK_SIZE, timeArgs, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(KeyTask, "KeyTask", configMINIMAL_STACK_SIZE, keyArgs, tskIDLE_PRIORITY + 2, NULL);

    vTaskStartScheduler();

    while(true);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
