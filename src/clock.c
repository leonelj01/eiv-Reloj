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
    uint16_t clockTicks;
    clockTimeT currentTime;
    bool valid;
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static void ClockAdvanceTime(clockT self) {
    self->currentTime.time.seconds[0]++;
    if(self->currentTime.time.seconds[0] > 9) {
        self->currentTime.time.seconds[0] = 0;
        self->currentTime.time.seconds[1]++;
    }
    if(self->currentTime.time.seconds[1] > 5) {
        self->currentTime.time.seconds[1] = 0;
        self->currentTime.time.minutes[0]++;
    }
    if(self->currentTime.time.minutes[0] > 9) {
        self->currentTime.time.minutes[0] = 0;
        self->currentTime.time.minutes[1]++;
    }
    if(self->currentTime.time.minutes[1] > 5) {
        self->currentTime.time.minutes[1] = 0;
        self->currentTime.time.hours[0]++;
    }
    if(self->currentTime.time.hours[0] > 9) {
        self->currentTime.time.hours[0] = 0;
        self->currentTime.time.hours[1]++;
    }
    if(self->currentTime.time.hours[1] == 2 && self->currentTime.time.hours[0] > 3) {
        self->currentTime.time.hours[0] = 0;
        self->currentTime.time.hours[1] = 0;
    }
}

/* === Public function implementation ============================================================================== */

clockT ClockCreate(uint16_t ticksPerSeconds) {
    (void)ticksPerSeconds;
    static struct clockS self[1];
    memset(self, 0, sizeof(struct clockS));
    self->valid = false;
    return self;
}

bool ClockGetTime(clockT self, clockTimeT * result) {
    memcpy(result, &self->currentTime, sizeof(clockTimeT));
    return self->valid;
}

bool ClockSetTime(clockT self, const clockTimeT * newTime) {
    self->valid = true;
    memcpy(&self->currentTime, newTime, sizeof(clockTimeT));
    return self->valid;
}

void ClockNewTick(clockT self) {
    self->clockTicks++;
    if (self->clockTicks == 5) {
        self->clockTicks = 0;
        ClockAdvanceTime(self);
    }
}
/* === End of documentation ======================================================================================== */
