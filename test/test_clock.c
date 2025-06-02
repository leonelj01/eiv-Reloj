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

static void SimulateSeconds(clockT clock, uint32_t seconds) {
    for (uint8_t i = 0; i < seconds; i++) {
        ClockNewTick(clock);
    }
}

clockT clock;

void setUp(void){
    clock = ClockCreate(CLOCK_TICK_PER_SECONDS);
}

void test_set_up_with_invalid_time(void) {
    clockTimeT currentTime = {.bcd = {1, 2, 3, 4, 5, 6}};

    clockT localClock = ClockCreate(CLOCK_TICK_PER_SECONDS);
    TEST_ASSERT_FALSE(ClockGetTime(localClock, &currentTime)); // Decidir si hacer una sola funcion o no
    TEST_ASSERT_EACH_EQUAL_UINT8(0, currentTime.bcd, 6);
}

// Al ajustar la hora el reloj con valores correctos, queda en hora y es válida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clockTimeT newTime = {.time = {
                                           .seconds = {4, 5}, .minutes = {3, 0}, .hours = {1, 4} // 14:30:45
                                       }};
    clockTimeT currentTime = {.bcd = {0, 0, 0, 0, 0, 0}};

    TEST_ASSERT_TRUE(ClockSetTime(clock, &newTime));
    TEST_ASSERT_TRUE(ClockGetTime(clock, &currentTime));

    TEST_ASSERT_EQUAL_INT8_ARRAY(newTime.bcd, currentTime.bcd, 6);
}

// Despues de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void){
    clockTimeT currentTime = {0};
    static const clockTimeT espectedValue = {.time = {
                                           .seconds = {1, 0}, .minutes = {0,0}, .hours = {0,0} // 14:30:45
                                       }};

    // set initial time to 00:00:00
    ClockSetTime(clock,&(clockTimeT){0});
    SimulateSeconds(clock, 1);
    ClockGetTime(clock, &currentTime);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(espectedValue.bcd,currentTime.bcd,6);
        
}

/* === End of documentation ======================================================================================== */
