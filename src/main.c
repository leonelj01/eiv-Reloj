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

/* === Public variable definitions ============================================================= */

boardT board;
clockT clock;
clockStates mode;
uint8_t digits[4];

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
        ScreenWriteDot(board->screen, (uint8_t[]){1, 1, 1, 1}, 4);
        break;
    case SET_ALARM_HOURS:
        ScreenFlashDigits(board->screen, 2, 3, 100);
        ScreenWriteDot(board->screen, (uint8_t[]){1, 1, 1, 1}, 4);
        break;
    default:
        break;
    }
}

void BcdIncrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens) {
    (*units)++;
    if (*units > max_units) {
        *units = 0;
        (*tens)++;
        if (*tens > max_tens) {
            *tens = 0;
        }
    }
}

void BcdDecrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens) {
    if (*units == 0) {
        *units = max_units;
        if (*tens == 0) {
            *tens = max_tens;
        } else {
            (*tens)--;
        }
    } else {
        (*units)--;
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    clockTimeT input;

    clock = ClockCreate(50, AlarmRinging);
    board = BoardCreate();

    SysTickInit(1000);
    ChangeMode(UNCONFIGURED);

    while (true) {

        if (DigitalInputWasActivated(board->accept)) {
            if (mode == SET_CURRENT_MINUTES) {
                ChangeMode(SET_CURRENT_HOURS);
            } else if (mode == SET_CURRENT_HOURS) {
                ClockSetTime(clock, &input);
                ChangeMode(SHOW_TIME);
            }
        }

        if (DigitalInputWasActivated(board->cancel)) {
            if (ClockGetTime(clock, &input)) {
                ChangeMode(SHOW_TIME);
            } else {
                ChangeMode(UNCONFIGURED);
            }
        }

        if (DigitalInputWasActivated(board->setTime)) {
            ChangeMode(SET_CURRENT_MINUTES);
            ClockGetTime(clock, &input);
            GetHourMinuteBCD(&input, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (DigitalInputWasActivated(board->setAlarm)) {
        }

        if (DigitalInputWasActivated(board->decrement)) {
            if (mode == SET_CURRENT_MINUTES) {
                BcdDecrement(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_CURRENT_HOURS) {
                BcdDecrement(&digits[1], &digits[0], 3, 2);
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (DigitalInputWasActivated(board->increment)) {
            if (mode == SET_CURRENT_MINUTES) {
                BcdIncrement(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_CURRENT_HOURS) {
                BcdIncrement(&digits[1], &digits[0], 3, 2);
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        for (int delay = 0; delay < 25000; delay++) {
            __asm("NOP");
        }
    }
}

void SysTick_Handler(void) {
    static bool dotsOn = false;
    uint8_t aux[4];
    clockTimeT hour;

    ScreenRefresh(board->screen);

    if (ClockNewTick(clock)) {
        dotsOn = !dotsOn;

        if (mode == SHOW_TIME) {
            ScreenWriteDot(board->screen, (uint8_t[]){0, dotsOn, 0, 0}, 4);
            ClockGetTime(clock, &hour);
            GetHourMinuteBCD(&hour, aux);
            ScreenWriteBCD(board->screen, aux, 4);
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
