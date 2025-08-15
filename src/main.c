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
#include "semphr.h"
#include "event_groups.h"
#include "semphr.h"
#include "bsp.h"
#include "auxiliar.h"
#include "keys.h"
#include <stdbool.h>
#include <stdlib.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

typedef enum clockStates {
    UNCONFIGURED,        //!< Hora no válida al iniciar el reloj.
    SHOW_TIME,           //!< Muestra la hora actual.
    SET_CURRENT_MINUTES, //!< Establece los minutos actuales.
    SET_CURRENT_HOURS,   //!< Establece la hora actual.
    SET_ALARM_MINUTES,   //!< Establece los minutos de la alarma.
    SET_ALARM_HOURS,     //!< Establece la hora de la alarma.
} clockStates;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

boardT board;
clockT clock;
clockStates mode;
clockTimeT hour;
clockTimeT alarm;
SemaphoreHandle_t hourMutex;
EventGroupHandle_t keyEventGroup;
uint8_t digits[4] = {0};

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
        break;

    case SET_CURRENT_MINUTES:
        ScreenFlashDigits(board->screen, 2, 3, 100);
        break;

    case SET_CURRENT_HOURS:
        ScreenFlashDigits(board->screen, 0, 1, 100);
        break;

    case SET_ALARM_MINUTES:
        ScreenFlashDigits(board->screen, 2, 3, 100);
        break;

    case SET_ALARM_HOURS:
        ScreenFlashDigits(board->screen, 0, 1, 100);
        break;

    default:
        break;
    }
}

void TimeTask(void * pointer) {
    TickType_t last = xTaskGetTickCount();
    while (true) {
        ClockNewTick(clock); // tick a 1ms
        xSemaphoreTake(hourMutex, portMAX_DELAY);
        ClockGetTime(clock, &hour); // cache de hora para pantalla
        xSemaphoreGive(hourMutex);
        vTaskDelayUntil(&last, pdMS_TO_TICKS(1)); // 1 ms
    }
}

// Reemplazá tu RefreshTask por esta versión
void RefreshTask(void * pointer) {
    const TickType_t kRefreshMs = 1; // multiplex 1 ms
    const TickType_t kBlinkMs = 500; // parpadeo 2 Hz
    TickType_t lastFrame = xTaskGetTickCount();
    TickType_t lastBlink = xTaskGetTickCount();
    bool blink = false;

    clockTimeT localHour = {0};

    while (true) {
        // 1) Actualizar cache de hora
        xSemaphoreTake(hourMutex, portMAX_DELAY);
        localHour = hour;
        xSemaphoreGive(hourMutex);

        // 2) Actualizar dígitos solo cuando mostramos hora o UNCONFIGURED
        if (mode == SHOW_TIME || mode == UNCONFIGURED) {
            GetHourMinuteBCD(&localHour, digits);
        }
        TickType_t now = xTaskGetTickCount();
        if ((now - lastBlink) >= pdMS_TO_TICKS(kBlinkMs)) {
            blink = !blink;
            lastBlink = now;
        }
        ScreenWriteBCD(board->screen, digits, sizeof(digits));
        for (int i = 0; i < 4; i++) {
            ScreenSetDots(board->screen, i, false);
        }
        if (mode == SET_ALARM_MINUTES || mode == SET_ALARM_HOURS) {
            ScreenSetDots(board->screen, 0, true);
            ScreenSetDots(board->screen, 1, true);
            ScreenSetDots(board->screen, 2, true);
            ScreenSetDots(board->screen, 3, true);
        }
        if (mode == SHOW_TIME) {
            ScreenSetDots(board->screen, 1, blink);
        }
        if (ClockIsAlarmActive(clock) && ClockIsAlarmEnabled(clock)) {
            ScreenSetDots(board->screen, 3, true);
        }
        ScreenRefresh(board->screen);

        vTaskDelayUntil(&lastFrame, pdMS_TO_TICKS(kRefreshMs));
    }
}

void StateTask(void * pointer) {
    (void)pointer; // no usamos argumentos; evitamos puntero NULL
    EventBits_t events;
    for (;;) {
        events = xEventGroupWaitBits(
            keyEventGroup, BTN_ACCEPT | BTN_CANCEL | BTN_SET_TIME | BTN_SET_ALARM | BTN_DECREMENT | BTN_INCREMENT,
            pdTRUE,  // clearOnExit
            pdFALSE, // waitForAllBits
            portMAX_DELAY);

        if (events & BTN_ACCEPT) {
            if (mode == SHOW_TIME) {
                if (ClockIsAlarmRinging(clock)) {
                    ClockSnoozeAlarm(clock, 5);
                    DigitalOutputDesactivate(board->ledRed);
                } else if (!ClockIsAlarmEnabled(clock)) {
                    ClockAlarmAction(clock, ALARM_ENABLE);
                }
            } else if (mode == SET_CURRENT_MINUTES) {
                ChangeMode(SET_CURRENT_HOURS);
            } else if (mode == SET_CURRENT_HOURS) {
                SetHourMinuteBCD(&hour, digits);
                ClockSetTime(clock, &hour);
                ChangeMode(SHOW_TIME);
            } else if (mode == SET_ALARM_MINUTES) {
                ChangeMode(SET_ALARM_HOURS);
            } else if (mode == SET_ALARM_HOURS) {
                SetHourMinuteBCD(&alarm, digits);
                ClockSetAlarm(clock, &alarm);
                ChangeMode(SHOW_TIME);
            }
        }

        if (events & BTN_CANCEL) {
            if (mode == SHOW_TIME) {
                if (ClockIsAlarmRinging(clock)) {
                    ClockAlarmAction(clock, ALARM_CANCEL);
                    DigitalOutputDesactivate(board->ledRed);
                } else if (ClockIsAlarmEnabled(clock)) {
                    ClockAlarmAction(clock, ALARM_DISABLE);
                }
            } else if (mode == SET_CURRENT_MINUTES || mode == SET_CURRENT_HOURS) {
                if (ClockGetTime(clock, &hour)) {
                    ChangeMode(SHOW_TIME);
                } else {
                    ChangeMode(UNCONFIGURED);
                }
            } else if (mode == SET_ALARM_MINUTES || mode == SET_ALARM_HOURS) {
                ChangeMode(SHOW_TIME);
            }
        }

        if (events & BTN_SET_TIME) {
            ChangeMode(SET_CURRENT_MINUTES);
            ClockGetTime(clock, &hour);
            GetHourMinuteBCD(&hour, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (events & BTN_SET_ALARM) {
            ChangeMode(SET_ALARM_MINUTES);
            ClockGetAlarm(clock, &alarm);
            GetHourMinuteBCD(&alarm, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (events & BTN_DECREMENT) {
            if (mode == SET_CURRENT_MINUTES || mode == SET_ALARM_MINUTES) {
                BcdDecrement(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_CURRENT_HOURS || mode == SET_ALARM_HOURS) {
                BcdDecrement(&digits[1], &digits[0], 3, 2);
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (events & BTN_INCREMENT) {
            if (mode == SET_CURRENT_MINUTES || mode == SET_ALARM_MINUTES) {
                BcdIncrement(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_CURRENT_HOURS || mode == SET_ALARM_HOURS) {
                BcdIncrement(&digits[1], &digits[0], 3, 2);
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }
    }
}

/* === Public function implementation ========================================================= */

int main(void) {

    board = BoardCreate();
    SysTickInit(1000);
    clock = ClockCreate(1000, AlarmRinging);

    hourMutex = xSemaphoreCreateMutex();
    keyEventGroup = xEventGroupCreate();

    keyTaskArgT accept = malloc(sizeof(struct keyTaskArgS));
    keyTaskArgT cancel = malloc(sizeof(struct keyTaskArgS));
    keyTaskArgT setTime = malloc(sizeof(struct keyTaskArgS));
    keyTaskArgT setAlarm = malloc(sizeof(struct keyTaskArgS));
    keyTaskArgT decrement = malloc(sizeof(struct keyTaskArgS));
    keyTaskArgT increment = malloc(sizeof(struct keyTaskArgS));

    accept->events = keyEventGroup;
    accept->gpio = board->accept;
    accept->shortPulse = BTN_ACCEPT;
    accept->longPulse = 0;

    cancel->events = keyEventGroup;
    cancel->gpio = board->cancel;
    cancel->shortPulse = BTN_CANCEL;
    cancel->longPulse = 0;

    setTime->events = keyEventGroup;
    setTime->gpio = board->setTime;
    setTime->shortPulse = BTN_SET_TIME;
    setTime->longPulse = BTN_SET_TIME_LONG;

    setAlarm->events = keyEventGroup;
    setAlarm->gpio = board->setAlarm;
    setAlarm->shortPulse = BTN_SET_ALARM;
    setAlarm->longPulse = BTN_SET_ALARM_LONG;

    decrement->events = keyEventGroup;
    decrement->gpio = board->decrement;
    decrement->shortPulse = BTN_DECREMENT;
    decrement->longPulse = 0;

    increment->events = keyEventGroup;
    increment->gpio = board->increment;
    increment->shortPulse = BTN_INCREMENT;
    increment->longPulse = 0;

    // Cargar hora en variable global y escribir buffer
    ClockGetTime(clock, &hour);
    GetHourMinuteBCD(&hour, digits);
    ScreenWriteBCD(board->screen, digits, sizeof(digits));

    // Modo inicial (si querés parpadeo de todos)
    ChangeMode(UNCONFIGURED);

    // Tareas principales
    xTaskCreate(TimeTask, "TimeTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 4, NULL);
    xTaskCreate(RefreshTask, "DisplayTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(StateTask, "StateTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(KeyTask, "KeyAccept", configMINIMAL_STACK_SIZE * 2, accept, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(KeyTask, "KeyCancel", configMINIMAL_STACK_SIZE * 2, cancel, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(KeyTask, "KeySetTime", configMINIMAL_STACK_SIZE * 2, setTime, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(KeyTask, "KeySetAlarm", configMINIMAL_STACK_SIZE * 2, setAlarm, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(KeyTask, "KeyDecrement", configMINIMAL_STACK_SIZE * 2, decrement, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(KeyTask, "KeyIncrement", configMINIMAL_STACK_SIZE * 2, increment, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();
    while (true);
    return 0;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
