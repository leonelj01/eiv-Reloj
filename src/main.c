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

#define TOGGLE_DOT()                                                                                                   \
    ScreenToggleDot(board->screen, 0);                                                                                 \
    ScreenToggleDot(board->screen, 1);                                                                                 \
    ScreenToggleDot(board->screen, 2);                                                                                 \
    ScreenToggleDot(board->screen, 3);

/* === Private data type declarations ========================================================== */

typedef enum clockStates {
    UNCONFIGURED,        //!< Hora no válida al iniciar el reloj.
    SHOW_TIME,           //!< Muestra la hora actual.
    SET_CURRENT_MINUTES, //!< Establece los minutos actuales.
    SET_CURRENT_HOURS,   //!< Establece la hora actual.
    SET_ALARM_MINUTES,   //!< Establece los minutos de la alarma.
    SET_ALARM_HOURS,     //!< Establece la hora de la alarma.
} clockStates;

typedef enum buttonStates{
    IDLE,
    PRESSED,
    LONG_PRESSED,
} buttonStates;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

boardT board;
clockT clock;
clockStates mode;
uint8_t digits[4];
bool dotsOn = false;

buttonStates SetTimeState = IDLE;
volatile uint32_t mseg = 0; // Variable para el tiempo en milisegundos

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
void AlarmRinging(clockT clock) {
    DigitalOutputActivate(board->ledRed);
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
    clockTimeT hour;
    clockTimeT alarm;

    clock = ClockCreate(1000, AlarmRinging);
    board = BoardCreate();

    SysTickInit(1000);
    ChangeMode(UNCONFIGURED);

    while (true) {

        if (DigitalInputWasDeactivated(board->accept)) {
            if (mode == SHOW_TIME) {
                if (ClockIsAlarmRinging(clock)) {
                    ClockSnoozeAlarm(clock, 5); // Posponer alarma 5 minutos
                }else if(!ClockIsAlarmEnabled(clock)){
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

        if (DigitalInputWasDeactivated(board->cancel)) {
            if (mode == SHOW_TIME) {
                if (ClockIsAlarmRinging(clock)) {
                    ClockAlarmAction(clock, ALARM_CANCEL);
                } else if (ClockIsAlarmEnabled(clock)) {
                    ClockAlarmAction(clock, ALARM_DISABLE);
                }
            } else if (mode == SET_CURRENT_MINUTES || mode == SET_CURRENT_HOURS) {
                if (ClockGetTime(clock, &hour)) {
                    ChangeMode(SHOW_TIME);
                } else {
                    ChangeMode(UNCONFIGURED); // Si la hora no es válida, volver a UNCONFIGURED
                }
            } else if (mode == SET_ALARM_MINUTES || mode == SET_ALARM_HOURS) {
                ChangeMode(SHOW_TIME); // Cancelar configuración de alarma
            }
        }

        if (DigitalInputWasDeactivated(board->setTime)) {
            ChangeMode(SET_CURRENT_MINUTES);
            ClockGetTime(clock, &hour);
            GetHourMinuteBCD(&hour, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (DigitalInputWasDeactivated(board->setAlarm)) {
            dotsOn = false;
            ChangeMode(SET_ALARM_MINUTES);
            ClockGetAlarm(clock, &alarm);
            GetHourMinuteBCD(&alarm, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (DigitalInputWasDeactivated(board->decrement)) {
            if (mode == SET_CURRENT_MINUTES || mode == SET_ALARM_MINUTES) {
                BcdDecrement(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_CURRENT_HOURS || mode == SET_ALARM_HOURS) {
                BcdDecrement(&digits[1], &digits[0], 3, 2);
            }

            ScreenWriteBCD(board->screen, digits, sizeof(digits));
            
            if ((mode == SET_ALARM_MINUTES || mode == SET_ALARM_HOURS) && dotsOn) {
                TOGGLE_DOT();
                dotsOn = true;
            }

            
        }

        if (DigitalInputWasDeactivated(board->increment)) {
            if (mode == SET_CURRENT_MINUTES || mode == SET_ALARM_MINUTES) {
                BcdIncrement(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_CURRENT_HOURS || mode == SET_ALARM_HOURS) {
                BcdIncrement(&digits[1], &digits[0], 3, 2);
            }

            ScreenWriteBCD(board->screen, digits, sizeof(digits));

            if ((mode == SET_ALARM_MINUTES || mode == SET_ALARM_HOURS) && dotsOn) {
                TOGGLE_DOT();
                dotsOn = true; // nos aseguramos que sigue en true
            }
        }

        for (int delay = 0; delay < 25000; delay++) {
            __asm("NOP");
        }
    }
}

void SysTick_Handler(void) {
    static uint16_t count = 0;
    clockTimeT hour;

    ScreenRefresh(board->screen);
    if (ClockGetTime(clock,&hour)){
        ClockNewTick(clock);
    }


    count = (count + 1) % 1000;
    if (mode <= SHOW_TIME) {
        ClockGetTime(clock, &hour);
        GetHourMinuteBCD(&hour, digits);
        ScreenWriteBCD(board->screen, digits, sizeof(digits));
        if (count > 500 && mode == SHOW_TIME) {
            ScreenToggleDot(board->screen, 1);
        }
        if (ClockIsAlarmActive(clock) && ClockIsAlarmEnabled(clock)) {
            ScreenToggleDot(board->screen, 3);
        }
        if (!ClockIsAlarmRinging(clock)) {
            DigitalOutputDesactivate(board->ledRed);
        }
        mseg++;
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
