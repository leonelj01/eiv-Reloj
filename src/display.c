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
 ** @brief Plantilla para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "display.h"
#include "clock.h"
#include "time.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void RefreshTask(void * pointer) {
    refreshTaskArgT args = pointer;
    TickType_t last_value = xTaskGetTickCount();

    while (true) {
        ScreenRefresh(args->display);
        xTaskDelayUntil(&last_value, pdMS_TO_TICKS(5));
    }
}

void DisplayTask(void * pointer){
    refreshTaskArgT args = pointer;
    clockTimeT currentTime;
    uint8_t digits[4];
    TickType_t lastUpdate = xTaskGetTickCount();

    for (;;) {
        xQueueReceive(args->dataTime, &currentTime, portMAX_DELAY);
        GetHourMinuteBCD(&currentTime, digits);

        xSemaphoreTake(args->mutex, portMAX_DELAY);
        ScreenWriteBCD(args->display, digits, 4);
        xSemaphoreGive(args->mutex);

        vTaskDelayUntil(&lastUpdate, pdMS_TO_TICKS(1000));
    }
}

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

/* === End of documentation ======================================================================================== */
