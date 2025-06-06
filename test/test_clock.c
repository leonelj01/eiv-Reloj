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

/** @file test_clock.c
 ** @brief Archivo de pruebas unitarias para el reloj.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"
#include "clock.h"
#include <stdint.h>

/* === Testing functions =========================================================================================== */

/**
 * - Al inicializar el reloj está en 00:00 y con hora invalida.
 * - Después de n ciclos de reloj la hora avanza un segundo, diez
 *   segundos, un minutos, diez minutos, una hora, diez horas y un día completo.
 * - Fijar la hora de la alarma y consultarla.
 * - Fijar la alarma y avanzar el reloj para que suene.
 * - Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
 * - Hacer sonar la alarma y posponerla.
 * - Hacer sonar la alarma y cancelarla hasta el otro dia.
 * - Probar getTime con NULL como argumento.
 * - Tratar de ajustar la hora el reloj con valores invalidos y verificarque los rechaza.
 * - Hacer una prueba con frecuencias diferentes.
 *
 */

#define CLOCK_TICK_PER_SECONDS 5
#define TEST_ASSERT_TIME(hourTens, hourUnits, minutesTens, minutesUnits, secondsTens, secondsUnits)                    \
    clockTimeT currentTime = {0};                                                                                      \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &currentTime), "Clock has invalid time");                             \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(secondsUnits, currentTime.bcd[0], "Diference in unit seconds");                    \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(secondsTens, currentTime.bcd[1], "Diference in tens seconds");                     \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutesUnits, currentTime.bcd[2], "Diference in unit minutes");                    \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutesTens, currentTime.bcd[3], "Diference in tens minutes");                     \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hourUnits, currentTime.bcd[4], "Diference in unit hours");                         \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hourTens, currentTime.bcd[5], "Diference in tens hours")

static void SimulateSeconds(clockT clock, uint32_t seconds) {
    for (uint8_t i = 0; i < CLOCK_TICK_PER_SECONDS * seconds; i++) {
        ClockNewTick(clock);
    }
}

clockT clock;

void setUp(void) {
    clock = ClockCreate(CLOCK_TICK_PER_SECONDS);
}

void test_set_up_with_invalid_time(void) {
    clockTimeT currentTime = {.bcd = {1, 2, 3, 4, 5, 6}};

    clockT localClock = ClockCreate(CLOCK_TICK_PER_SECONDS);
    TEST_ASSERT_FALSE(ClockGetTime(localClock, &currentTime));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, currentTime.bcd, 6);
}

// Al ajustar la hora el reloj con valores correctos, queda en hora y es válida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clockTimeT newTime = {.time = {
                                           .hours = {4, 1}, .minutes = {0, 3}, .seconds = {5, 4} // 14:30:45
                                       }};
    TEST_ASSERT_TRUE(ClockSetTime(clock, &newTime));
    TEST_ASSERT_TIME(1, 4, 3, 0, 4, 5);
}

// Despues de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void) {

    // set initial time to 00:00:00
    ClockSetTime(clock, &(clockTimeT){0});
    SimulateSeconds(clock, 1);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1);
}

// Despues de diez ciclos de reloj la hora avanza diez segundos
void test_clock_advance_ten_second(void) {

    // set initial time to 00:00:00
    ClockSetTime(clock, &(clockTimeT){0});
    SimulateSeconds(clock, 10);
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0);
}

// Despues de n ciclos de reloj la hora avanza un minuto
void test_clock_advance_one_minute(void) {

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {0, 0}, .minutes = {0, 0}, .seconds = {9, 4} // 00:00:49
                                      }});
    SimulateSeconds(clock, 11);
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0);
}

// Despues de n ciclos de reloj la hora avanza diez minutos
void test_clock_advance_ten_minutes(void) {

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {0, 0}, .minutes = {9, 0}, .seconds = {9, 4} // 00:09:49
                                      }});
    SimulateSeconds(clock, 11);
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0);
}

// Despues de n ciclos de reloj la hora avanza una hora
void test_clock_advance_one_hour(void) {

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {0, 0}, .minutes = {9, 5}, .seconds = {9, 4} // 00:59:49
                                      }});
    SimulateSeconds(clock, 11);
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0);
}

// Despues de n ciclos de reloj la hora avanza diez horas
void test_clock_advance_ten_hours(void) {

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {9, 0}, .minutes = {9, 5}, .seconds = {9, 4} // 09:59:49
                                      }});
    SimulateSeconds(clock, 11);
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0);
}

// Despues de n ciclos de reloj la hora avanza un dia completo
void test_clock_advance_one_day(void) {

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {3, 2}, .minutes = {9, 5}, .seconds = {9, 4} // 23:59:49
                                      }});
    SimulateSeconds(clock, 12);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1);
}

/* === End of documentation ======================================================================================== */
