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

#include "bsp.h"
#include "clock.h"
#include <stdbool.h>

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

void GetHourMinuteBCD(clockTimeT * time, uint8_t digits[4]);

/* === Public variable definitions ============================================================= */

static boardT board;
static clockT clock;
static clockStates state = UNCONFIGURED;

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
void AlarmRinging(clockT clock) {
}

void GetHourMinuteBCD(clockTimeT * time, uint8_t digits[4]) {
    if (time && digits) {
        digits[0] = time->bcd[5]; // Hora de decenas
        digits[1] = time->bcd[4]; // Hora de unidades
        digits[2] = time->bcd[3]; // Minuto de decenas
        digits[3] = time->bcd[2]; // Minuto de unidades
    }
}
/* === Public function implementation ========================================================= */

int main(void) {

    clock = ClockCreate(50, AlarmRinging);
    board = BoardCreate();

    SysTickInit(1000);
    //ScreenFlashDigits(board->screen, 0, 3, 200);

    while (true) {

        switch (state) {
        case UNCONFIGURED:
            ScreenWriteBCD(board->screen, (uint8_t[]){0, 0, 0, 0}, 4);
            break;
        case SHOW_TIME:
            break;

        default:
            break;
        }

        for (int delay = 0; delay < 25000; delay++) {
            __asm("NOP");
        }
    }
}

void SysTick_Handler(void) {
    static bool lastValue = false;
    bool currentValue;
    uint8_t digits[4];
    clockTimeT hour;

    ScreenRefresh(board->screen);
    currentValue =  ClockNewTick(clock);

    if (currentValue != lastValue){
        lastValue = currentValue;
        if (state == SHOW_TIME){
            ScreenToggleDots(board->screen, 2);
            ClockGetTime(clock, &hour);
            GetHourMinuteBCD(&hour,digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));

        }
    }
}
/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
