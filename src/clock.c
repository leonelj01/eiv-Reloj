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
    uint16_t ticks;                  //!< Contador de ticks del reloj
    uint16_t ticksPerSecond;         //!< Cantidad de ticks por segundo
    clockTimeT currentTime;          //!< Hora actual del reloj
    clockTimeT alarm;                //!< Hora de la alarma
    bool validTime;                  //!< Indica si la hora actual es válida
    bool validAlarm;                 //!< Indica si la hora de la alarma es válida
    bool alarmActive;                //!< Indica si la alarma está activa
    bool alarmEnabled;               //!< Indica si la alarma está habilitada
    bool alarmRingingNow;            //!< Indica si la alarma está sonando en este momento
    clockAlarmRingingT alarmRinging; //!< Controlador del reloj
};

/* === Private function declarations =============================================================================== */

/**
 * @brief  Avanza el tiempo del reloj en un segundo y actualiza los campos de horas, minutos y segundos.
 *
 * @param self  Referencia al objeto reloj.
 */
static void AdvanceTime(clockT self);

/**
 * @brief  Verifica si la hora proporcionada es válida.
 *
 * @param time  Puntero a la estructura de hora a verificar.
 * @return true Si la hora es válida.
 * @return false Si la hora no es válida.
 */
static bool IsValidTime(const clockTimeT * time);

/**
 * @brief  Pospone la alarma por una cantidad de minutos especificada.
 *
 * @param self  Referencia al objeto reloj.
 * @param minutes  Cantidad de minutos para posponer la alarma.
 */
static void AlarmPospone(clockT self, uint8_t minutes);

/**
 * @brief  Incrementa un valor en formato BCD (Binary-Coded Decimal).
 *
 * @param units  Puntero al dígito de unidades.
 * @param tens  Puntero al dígito de decenas.
 * @param max_units  Valor máximo para el dígito de unidades.
 * @param max_tens  Valor máximo para el dígito de decenas.
 * @return true Si se ha producido un desbordamiento y se ha incrementado el dígito de decenas.
 * @return false Si no hay desbordamiento.
 */
static bool BcdIncrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens);

/**
 * @brief  Verifica si ha pasado un nuevo día comparando la hora previa con la actual.
 *
 * @param prev  Hora previa del reloj.
 * @return true Si ha pasado un nuevo día.
 * @return false Si no ha pasado un nuevo día.
 */
static bool IsNewDay(clockTimeT prev);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static void AdvanceTime(clockT self) {
    // Guardar hora previa para detectar el paso de un día
    clockTimeT previousTime = self->currentTime;

    if (BcdIncrement(&self->currentTime.time.seconds[0], &self->currentTime.time.seconds[1], 9, 5)) {
        if (BcdIncrement(&self->currentTime.time.minutes[0], &self->currentTime.time.minutes[1], 9, 5)) {
            BcdIncrement(&self->currentTime.time.hours[0], &self->currentTime.time.hours[1], 3, 2);
        }
    }

    // Detectar paso de 23:59:59 a 00:00:00
    if (IsNewDay(previousTime)) {
        self->alarmActive = true; // Si es un nuevo día, activar la alarma
    }

    // Verificar si alarma debe sonar y disparar callback
    ClockAlarmRinging(self);
}

static bool IsValidTime(const clockTimeT * time) {
    bool isValid = true;
    if (time->time.hours[1] > 2 || (time->time.hours[1] == 2 && time->time.hours[0] > 3) || time->time.minutes[1] > 5 ||
        time->time.minutes[0] > 9 || time->time.seconds[1] > 5 || time->time.seconds[0] > 9) {
        isValid = false; // Hora no válida
    }
    return isValid;
}

static void AlarmPospone(clockT self, uint8_t minutes) {
    // Convertir minutos actuales a decimal
    uint8_t dec_minutes = self->alarm.time.minutes[1] * 10 + self->alarm.time.minutes[0];
    uint8_t dec_hours = self->alarm.time.hours[1] * 10 + self->alarm.time.hours[0];

    // Sumar los minutos
    dec_minutes += minutes;

    // Ajustar horas si hay overflow de minutos
    dec_hours += dec_minutes / 60;
    dec_minutes = dec_minutes % 60;

    // Ajustar si pasa de 23:59
    if (dec_hours >= 24) {
        dec_hours = dec_hours % 24;
    }

    // Convertir de vuelta a BCD
    self->alarm.time.minutes[1] = dec_minutes / 10;
    self->alarm.time.minutes[0] = dec_minutes % 10;

    self->alarm.time.hours[1] = dec_hours / 10;
    self->alarm.time.hours[0] = dec_hours % 10;
}

static bool BcdIncrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens) {
    (*units)++;
    if (*units > max_units) {
        *units = 0;
        (*tens)++;
        if (*tens > max_tens) {
            *tens = 0;
            return true;
        }
    }
    return false;
}

static bool IsNewDay(clockTimeT prev) {
    if (prev.time.hours[1] == 2 && prev.time.hours[0] == 3 && prev.time.minutes[1] == 5 && prev.time.minutes[0] == 9 &&
        prev.time.seconds[1] == 5 && prev.time.seconds[0] == 9) {
        return true; // Se ha detectado un nuevo día
    }
    return false; // No se ha detectado un nuevo día
}

/* === Public function implementation ============================================================================== */

clockT ClockCreate(uint16_t ticksPerSecond, clockAlarmRingingT function) {
    static struct clockS self[1];
    memset(self, 0, sizeof(struct clockS));
    self->validTime = false;
    self->validAlarm = false;
    self->alarmActive = false;
    self->alarmEnabled = false;
    self->alarmRingingNow = false;
    self->ticksPerSecond = ticksPerSecond;
    self->alarmRinging = function;
    return self;
}

bool ClockGetTime(clockT self, clockTimeT * result) {
    if (result != NULL) {
        memcpy(result, &self->currentTime, sizeof(clockTimeT));
        return self->validTime;
    }
    return false; // Protección ante NULL
}

bool ClockSetTime(clockT self, const clockTimeT * newTime) {
    if (!self || !newTime) {
        return false; // Protección ante NULL
    }

    if (IsValidTime(newTime)) {
        memcpy(&self->currentTime, newTime, sizeof(clockTimeT));
        self->validTime = true; // Hora válida
    } else {
        self->validTime = false; // Hora no válida
    }
    return self->validTime;
}

bool ClockNewTick(clockT self) {
    if (self) {
        self->ticks++;

        if (self->ticks >= self->ticksPerSecond) {
            self->ticks = 0;
            AdvanceTime(self);
            return true;
        }
    }

    return false;
}

bool ClockSetAlarm(clockT self, const clockTimeT * alarm) {

    if (!self || !alarm) {
        return false; // Protección ante NULL
    }

    if (!IsValidTime(alarm)) {
        self->validAlarm = false;
        self->alarmEnabled = false; // Deshabilita la alarma si la hora es inválida
        self->alarmActive = false;  // Desactiva la alarma si la hora es inválida
        return false;               // Hora de alarma inválida
    }

    memcpy(&self->alarm, alarm, sizeof(clockTimeT));
    self->validAlarm = true;
    self->alarmEnabled = true;
    self->alarmActive = true;
    return true;
}

bool ClockGetAlarm(clockT self, clockTimeT * alarm) {
    if (self && alarm) {
        memcpy(alarm, &self->alarm, sizeof(clockTimeT));
        return self->validAlarm; // Retorna si la alarma es válida
    }
    return false; // Si el reloj o el puntero de alarma son NULL, retorna false
}

bool ClockIsAlarmActive(clockT self) {
    return self ? self->alarmActive : false; // Retorna si la alarma está activa
}

bool ClockIsAlarmEnabled(clockT self) {
    return self ? self->alarmEnabled : false; // Retorna si la alarma está habilitada
}

void ClockAlarmAction(clockT self, AlarmActions action) {
    if (self) {
        switch (action) {
        case ALARM_CANCEL:
            self->alarmActive = false;     // Cancela la alarma
            self->alarmRingingNow = false; // Apaga sonido actual
            break;
        case ALARM_DISABLE:
            self->alarmEnabled = false;    // Desactiva la alarma
            self->alarmRingingNow = false; // Apaga sonido actual si estaba
            break;
        case ALARM_ENABLE:
            self->alarmEnabled = true; // Habilita la alarma
            break;
        }
    }
}

void ClockSnoozeAlarm(clockT self, uint8_t minutes) {
    if (self && self->alarmActive && self->alarmEnabled) {
        self->alarmRingingNow = false; // Evita que suene inmediatamente otra vez
        AlarmPospone(self, minutes);   // Pospone la alarma
    }
}

void ClockAlarmRinging(clockT self) {
    if (self && self->alarmEnabled && self->alarmActive) {
        if (!self->alarmRingingNow && !memcmp(&self->currentTime, &self->alarm, sizeof(clockTimeT))) {
            self->alarmRingingNow = true;
            self->alarmRinging(self);
        }
    }
}

bool ClockIsAlarmRinging(clockT self){
    return self ? self->alarmRingingNow : false;
}

/* === End of documentation ======================================================================================== */
