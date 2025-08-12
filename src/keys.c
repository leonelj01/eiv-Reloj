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

#include "keys.h"

/* === Macros definitions ========================================================================================== */

#define LONG_MS       3000
#define SCAN_DELAY_MS 40

/* === Private data type declarations ============================================================================== */

typedef enum { KEY_STATE_IDLE, KEY_STATE_PRESSED } keyState;

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void KeyTask(void * pointer) {
    keyTaskArgT args = pointer;
    static keyState state = KEY_STATE_IDLE;
    static TickType_t startTick = 0;

    while (true) {
        bool active = DigitalInputGetActivate(args->gpio);
        TickType_t now = xTaskGetTickCount();

        switch (state) {

        case KEY_STATE_IDLE:
            if (active) {
                // Inicia medición
                startTick = now;
                state = KEY_STATE_PRESSED;
            }
            break;

        case KEY_STATE_PRESSED:
            if (!active) {
                // Se soltó antes de LONG_MS → pulsación corta
                if ((now - startTick) < pdMS_TO_TICKS(LONG_MS)) {
                    xEventGroupSetBits(args->events, args->shortPulse);
                }
                state = KEY_STATE_IDLE;
            } else {
                // Sigue presionado, verificar si ya es largo
                if ((now - startTick) >= pdMS_TO_TICKS(LONG_MS)) {
                    xEventGroupSetBits(args->events, args->longPulse);
                    // Esperar liberación
                    state = KEY_STATE_IDLE;
                }
            }
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(SCAN_DELAY_MS));
    }
}

/* === End of documentation ======================================================================================== */
