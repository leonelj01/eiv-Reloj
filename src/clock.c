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

/** @file clock.c
 ** @brief Implementación de las funciones del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <string.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

struct clockS {
    uint16_t ticks;          //!< Contador de ticks del reloj
    uint16_t ticksPerSecond; //!< Cantidad de ticks por segundo
    clockTimeT currentTime;  //!< Hora actual del reloj
    clockTimeT alarm;        //!< Hora de la alarma
    bool validTime;          //!< Indica si la hora actual es válida
    bool alarmActive;        //!< Indica si la alarma está activa
    bool validAlarm;         //!< Indica si la hora de la alarma es válida
};

/* === Private function declarations =============================================================================== */

/**
 * @brief  Avanza el tiempo del reloj en un segundo y actualiza los campos de horas, minutos y segundos.
 *
 * @param self  Referencia al objeto reloj.
 */
static void AdvanceTime(clockT self);

static bool IsValidTime(const clockTimeT * time);

static void AlarmPospone(clockT self);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static void AdvanceTime(clockT self) {
    self->currentTime.time.seconds[0]++;
    if (self->currentTime.time.seconds[0] > 9) {
        self->currentTime.time.seconds[0] = 0;
        self->currentTime.time.seconds[1]++;
    }
    if (self->currentTime.time.seconds[1] > 5) {
        self->currentTime.time.seconds[1] = 0;
        self->currentTime.time.minutes[0]++;
    }
    if (self->currentTime.time.minutes[0] > 9) {
        self->currentTime.time.minutes[0] = 0;
        self->currentTime.time.minutes[1]++;
    }
    if (self->currentTime.time.minutes[1] > 5) {
        self->currentTime.time.minutes[1] = 0;
        self->currentTime.time.hours[0]++;
    }
    if (self->currentTime.time.hours[0] > 9) {
        self->currentTime.time.hours[0] = 0;
        self->currentTime.time.hours[1]++;
    }
    if (self->currentTime.time.hours[1] == 2 && self->currentTime.time.hours[0] > 3) {
        self->currentTime.time.hours[0] = 0;
        self->currentTime.time.hours[1] = 0;
    }
}

static bool IsValidTime(const clockTimeT * time) {
    bool isValid = true;
    if (time->time.hours[1] > 2 || (time->time.hours[1] == 2 && time->time.hours[0] > 3) || time->time.minutes[1] > 5 ||
        time->time.minutes[0] > 9 || time->time.seconds[1] > 5 || time->time.seconds[0] > 9) {
        isValid = false; // Hora no válida
    }
    return isValid;
}

static void AlarmPospone(clockT self) {
    self->alarm.time.minutes[0] += 5;
    if (self->alarm.time.minutes[0] > 9) {
        self->alarm.time.minutes[0] -= 10;
        self->alarm.time.minutes[1]++;
    }
    if (self->alarm.time.minutes[1] > 5) {
        self->alarm.time.minutes[1] = 0;
        self->alarm.time.hours[0]++;
    }
    if (self->alarm.time.hours[0] > 9) {
        self->alarm.time.hours[0] = 0;
        self->alarm.time.hours[1]++;
    }
    if (self->alarm.time.hours[1] == 2 && self->alarm.time.hours[0] > 3) {
        self->alarm.time.hours[0] = 0;
        self->alarm.time.hours[1] = 0;
    }
}
/* === Public function implementation ============================================================================== */

clockT ClockCreate(uint16_t ticksPerSeconds) {
    static struct clockS self[1];
    memset(self, 0, sizeof(struct clockS));
    self->validTime = false;
    self->validAlarm = false;
    self->ticksPerSecond = ticksPerSeconds;
    return self;
}

bool ClockGetTime(clockT self, clockTimeT * result) {
    if (result != NULL) {
        memcpy(result, &self->currentTime, sizeof(clockTimeT));
    } else {
        return false; // No se puede obtener la hora si el puntero es NULL
    }
    return self->validTime;
}

bool ClockSetTime(clockT self, const clockTimeT * newTime) {
    if (IsValidTime(newTime)) {
        memcpy(&self->currentTime, newTime, sizeof(clockTimeT));
        self->validTime = true; // Hora válida
    } else {
        self->validTime = false; // Hora no válida
    }
    return self->validTime;
}

void ClockNewTick(clockT self) {
    self->ticks++;
    if (self->ticks == self->ticksPerSecond) {
        self->ticks = 0;
        AdvanceTime(self);
    }
}

bool ClockSetAlarm(clockT self, const clockTimeT * alarm) {
    memcpy(&self->alarm, alarm, sizeof(clockTimeT));
    self->validAlarm = true;
    self->alarmActive = true; // Se activa la alarma al establecerla
    return self->validAlarm;
}

bool ClockGetAlarm(clockT self, clockTimeT * alarm) {
    memcpy(alarm, &self->alarm, sizeof(clockTimeT));
    return self->validAlarm;
}

bool ClockIsAlarmActive(clockT self) {
    bool isActive = false;
    if (self->alarmActive && self->validTime) {
        if (!memcmp(&self->currentTime, &self->alarm, sizeof(clockTimeT))) {
            isActive = true; // La alarma está activa
            // se debera cambiar por un callback para que ejecute una accion
        }
    }
    return isActive;
}

// Luego quisiera hacer un case para utilizar distintas funciones de la alarma, por ejemplo:
// - Posponer la alarma
// - Cancelar la alarma
// - Desactivar la alarma
// Ademas deberia cambiar el nombre por ClockAlarmAction o algo similar
void ClockAlarmAction(clockT self, AlarmActions action) {
    if (self) {
        switch (action) {
        case ALARM_SNOOZE:
            // Posponer la alarma por 5 minutos
            AlarmPospone(self);
            break;
        case ALARM_CANCEL:
            self->alarmActive = false; // Cancela la alarma
            // Deberia tener en cuenta luego que la alarma se debe volver a activar para el dia siguiente.
            break;
        case ALARM_DISABLE:
            self->alarmActive = false; // Desactiva la alarma
            break;
        }
    }
}
/* === End of documentation ======================================================================================== */
