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

/** @file time.c
 ** @brief Plantilla para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "time.h"
#include "keys.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void TimeTask(void * pointer) {
    
    
    timeTaskArgT args = pointer;
    EventBits_t events;
    clockTimeT currentTime;
    char state = UNCONFIGURED;

    xQueueSend(args->states, &state, 0);
    
    while(1) {
        // Obtener la hora actual
        ClockGetTime(args->clock, &currentTime);
        xQueueSend(args->elapsed, &currentTime, 0);
        
        // Manejar eventos de botones
        events = xEventGroupWaitBits(args->events, args->accept | args->cancel | args->increment | args->decrement | args->setTime | args->setAlarm, 
            pdTRUE, pdFALSE, pdMS_TO_TICKS(100));
            
        if (events & args->setTime) {
            state = SET_CURRENT_MINUTES;
            xQueueSend(args->states, &state, 0);
        }
        if (events & args->accept) {
            switch (state)
            {
            case SET_CURRENT_MINUTES:
                state = SET_CURRENT_HOURS;
                xQueueSend(args->states, &state, 0);
                break;
            case SET_CURRENT_HOURS:
                state = SHOW_TIME;
                xQueueSend(args->states, &state, 0);
                break;
            case SET_ALARM_MINUTES:
                state = SET_ALARM_HOURS;
                xQueueSend(args->states, &state, 0);
                break;
            case SET_ALARM_HOURS:
                state = SHOW_TIME;
                xQueueSend(args->states, &state, 0);
                break;
            default:
                break;
            }
        }
        if (events & args->cancel){
            if (state == SHOW_TIME) {
                if (ClockIsAlarmRinging(args->clock)) {
                    ClockAlarmAction(args->clock, ALARM_CANCEL);
                } else if (ClockIsAlarmEnabled(args->clock)) {
                    ClockAlarmAction(args->clock, ALARM_DISABLE);
                }
            } else if (state == SET_CURRENT_MINUTES || state == SET_CURRENT_HOURS) {
                if (ClockGetTime(args->clock, &currentTime)) {
                    state = SHOW_TIME;
                    xQueueSend(args->states, &state, 0);
                } else {
                    state = UNCONFIGURED;
                    xQueueSend(args->states, &state, 0);
                }
            } else if (state == SET_ALARM_MINUTES || state == SET_ALARM_HOURS) {
                state = SHOW_TIME;
                xQueueSend(args->states, &state, 0);
            }
        }
        
        // Esperar hasta el próximo ciclo
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void TimeRefresh(void * pointer){
    timeTaskArgT args = pointer;
    TickType_t lastWakeTime = xTaskGetTickCount();

    while(1) {
        ClockNewTick(args->clock);
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1));
    }
}
/* === End of documentation ======================================================================================== */
