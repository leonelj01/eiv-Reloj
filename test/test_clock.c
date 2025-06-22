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
 * - Tratar de ajustar la hora el reloj con valores invalidos y verificar que los rechaza.
 * - Fijar la hora de la alarma y consultarla.
 * - Fijar la alarma y avanzar el reloj para que suene.
 * - Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
 * - Hacer sonar la alarma y posponerla.
 * - Hacer sonar la alarma y cancelarla hasta el otro dia.
 * - Probar getTime con NULL como argumento.
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

#define TEST_ASSERT_ALARM(hourTens, hourUnits, minutesTens, minutesUnits, secondsTens, secondsUnits)                   \
    clockTimeT alarmTime = {0};                                                                                        \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetAlarm(clock, &alarmTime), "Clock has invalid time");                              \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(secondsUnits, alarmTime.bcd[0], "Diference in unit seconds");                      \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(secondsTens, alarmTime.bcd[1], "Diference in tens seconds");                       \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutesUnits, alarmTime.bcd[2], "Diference in unit minutes");                      \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutesTens, alarmTime.bcd[3], "Diference in tens minutes");                       \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hourUnits, alarmTime.bcd[4], "Diference in unit hours");                           \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hourTens, alarmTime.bcd[5], "Diference in tens hours");

static void SimulateSeconds(clockT clock, uint32_t seconds) {
    uint32_t ticks = CLOCK_TICK_PER_SECONDS * seconds;
    for (uint32_t i = 0; i < ticks; i++) {
        ClockNewTick(clock);
    }
}

clockT clock;

void setUp(void) {
    clock = ClockCreate(CLOCK_TICK_PER_SECONDS);
}

// Al inicializar el reloj está en 00:00:00 y con hora invalida.
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

// Tratar de ajustar la hora el reloj con valores invalidos y verificar que los rechaza.
void test_set_up_and_adjust_with_invalid_time(void) {
    static const clockTimeT newTime = {.bcd = {9, 9, 9, 9, 9, 9}}; // Hora invalida
    TEST_ASSERT_FALSE(ClockSetTime(clock, &newTime));
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

// Fijar la hora de la alarma y consultarla.
void test_clock_set_and_get_alarm_time(void) {
    static const clockTimeT alarm = {.time = {
                                         .hours = {2, 2}, .minutes = {0, 0}, .seconds = {0, 0} // 22:00:00
                                     }};

    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm));
    TEST_ASSERT_ALARM(2, 2, 0, 0, 0, 0); // Verifica que la alarma se haya establecido correctamente
}

// Fijar la alarma y avanzar el reloj para que suene.
void test_clock_set_alarm_and_advance_to_ring(void) {
    static const clockTimeT alarmTime = {.time = {
                                             .hours = {0, 1}, .minutes = {0, 0}, .seconds = {0, 0} // 10:00:00
                                         }};

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {9, 0}, .minutes = {9, 5}, .seconds = {9, 4} // 09:59:49
                                      }});

    ClockSetAlarm(clock, &alarmTime);
    SimulateSeconds(clock, 11);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock)); // Verifica que la alarma esté activa
}

// Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
void test_clock_set_alarm_and_disable(void) {
    static const clockTimeT alarmTime = {.time = {
                                             .hours = {0, 1}, .minutes = {0, 3}, .seconds = {0, 0} // 10:30:00
                                         }};

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {0, 1}, .minutes = {9, 2}, .seconds = {9, 4} // 09:59:49
                                      }});

    ClockSetAlarm(clock, &alarmTime);
    ClockAlarmAction(clock, ALARM_DISABLE); // Deshabilita la alarma
    SimulateSeconds(clock, 11);
    TEST_ASSERT_FALSE(ClockAlarmRinging(clock)); // Verifica que la alarma no esté activa
}

// Probar getTime con NULL como argumento.
void test_clock_get_time_with_null_argument(void) {
    clockTimeT * nullTime = NULL;
    TEST_ASSERT_FALSE(ClockGetTime(clock, nullTime)); // Verifica que no se pueda obtener la hora con NULL
}

// Hacer sonar la alarma y posponerla.
void test_clock_ring_and_postpone_alarm(void) {
    static const clockTimeT alarm = {.time = {
                                         .hours = {0, 1}, .minutes = {0, 1}, .seconds = {0, 0} // 10:10:00
                                     }};

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {0, 1}, .minutes = {9, 0}, .seconds = {0, 0} // 10:09:00
                                      }});

    ClockSetAlarm(clock, &alarm);
    SimulateSeconds(clock, 60);
    TEST_ASSERT_TRUE(ClockAlarmRinging(clock)); // Verifica que la alarma suene
    ClockSnoozeAlarm(clock, 5);                 // Pospone la alarma por 5 minutos
    TEST_ASSERT_ALARM(1, 0, 1, 5, 0, 0);        // Verifica que la alarma se haya pospuesto correctamente
    SimulateSeconds(clock, 300);
    TEST_ASSERT_TRUE(ClockAlarmRinging(clock)); // Verifica que la alarma suene
}

// Hacer sonar la alarma y cancelarla hasta el otro dia.
void test_clock_ring_and_cancel_alarm(void) {
    static const clockTimeT alarm = {.time = {
                                         .hours = {0, 1}, .minutes = {0, 1}, .seconds = {0, 0} // 10:10:00
                                     }};

    ClockSetTime(clock, &(clockTimeT){.time = {
                                          .hours = {0, 1}, .minutes = {9, 0}, .seconds = {0, 0} // 10:09:00
                                      }});

    ClockSetAlarm(clock, &alarm);
    SimulateSeconds(clock, 60);
    TEST_ASSERT_TRUE(ClockAlarmRinging(clock));  // Verifica que la alarma esté activa
    ClockAlarmAction(clock, ALARM_CANCEL);       // Cancela la alarma
    TEST_ASSERT_FALSE(ClockAlarmRinging(clock)); // Verifica que la alarma no esté activa
    SimulateSeconds(clock, 86400);               // Avanza un día completo
    TEST_ASSERT_TRUE(ClockAlarmRinging(clock));  // Verifica que la alarma está activa después de un día
}

// Probar que la alarma se pospone correctamente al exceder el tiempo máximo permitido.
void test_alarm_snooze_overflow(void) {
    static const clockTimeT alarm = {.time = {.hours = {3, 2}, .minutes = {0, 5}, .seconds = {0, 0}}}; // 23:50:00

    ClockSetTime(clock, &(clockTimeT){.time = {.hours = {3, 2}, .minutes = {9, 4}, .seconds = {0, 0}}}); // 23:49:00
    ClockSetAlarm(clock, &alarm);

    SimulateSeconds(clock, 60); // Avanza un minuto para activar la alarma
    TEST_ASSERT_TRUE(ClockAlarmRinging(clock)); // Verifica que la alarma suene
    ClockSnoozeAlarm(clock,70); // Debería quedar 00:05:00
    SimulateSeconds(clock, 4200); // Avanza 5 minutos
    TEST_ASSERT_TRUE(ClockAlarmRinging(clock)); // Verifica que la alarma suene nuevamente
    TEST_ASSERT_ALARM(0, 1, 0, 0, 0, 0); // Verifica que la alarma se haya pospuesto correctamente
}

// Probar la protección contra NULL en las funciones de reloj.
void test_null_protection(void) {
    TEST_ASSERT_FALSE(ClockSetTime(NULL, NULL));
    TEST_ASSERT_FALSE(ClockSetAlarm(NULL, NULL));
    TEST_ASSERT_FALSE(ClockGetAlarm(NULL, NULL));
    TEST_ASSERT_FALSE(ClockGetTime(NULL, NULL));
}

/* === End of documentation ======================================================================================== */
