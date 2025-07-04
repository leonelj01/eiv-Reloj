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

/** @file screen.c
 ** @brief Implementación del modulo para la gestión de una pantalla multiplexada de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */

#include "screen.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* === Macros definitions ========================================================================================== */

#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif

/* === Private data type declarations ============================================================================== */

struct screenS {
    uint8_t digits;
    uint8_t value[SCREEN_MAX_DIGITS];
    uint8_t dots[SCREEN_MAX_DIGITS];
    struct {
        uint8_t from;
        uint8_t to;
        uint8_t count;
        uint16_t frequency;
        uint8_t selector; // 0: Digits, 1: Dots, -1: No flashing
    } flashingDigits[1], flashingDots[1];
    screenDriverT driver;
    uint8_t currentDigit;
};

static const uint8_t IMAGES[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,             // 9
};

/* === Private function declarations =============================================================================== */

/**
 * @brief Controla el parpadeo del display.
 *
 * @param self Puntero a la instancia de la pantalla.
 *
 * @return El valor de los segmentos a mostrar en el dígito actual.
 *
 * @note Si la frecuencia de parpadeo es 0, no se realiza el parpadeo y se devuelve el valor del dígito actual.
 */
static uint8_t FlashingDig(screenT self);

/**
 * @brief Controla el parpadeo del punto decimal del display.
 *
 * @param self Puntero a la instancia de la pantalla.
 *
 * @return El valor del punto decimal a mostrar en el dígito actual.
 *
 * @note Si la frecuencia de parpadeo es 0, no se realiza el parpadeo y se devuelve el valor del punto decimal actual.
 */
static uint8_t FlashingDot(screenT self);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static uint8_t FlashingDig(screenT self) {
    uint8_t result = self->value[self->currentDigit];
    if (self->flashingDigits->frequency != 0 && self->flashingDigits->selector == 0) {
        if (self->currentDigit == 0) {
            self->flashingDigits->count = (self->flashingDigits->count + 1) % self->flashingDigits->frequency;
        }
        if (self->flashingDigits->count < (self->flashingDigits->frequency / 2)) {
            if (self->currentDigit >= self->flashingDigits->from) {
                if (self->currentDigit <= self->flashingDigits->to) {
                    result = 0;
                }
            }
        }
    }
    return result;
}

static uint8_t FlashingDot(screenT self) {
    uint8_t result = self->dots[self->currentDigit];
    if (self->flashingDots->frequency != 0 && self->flashingDots->selector == 1) {
        if (self->currentDigit == 0) {
            self->flashingDots->count = (self->flashingDots->count + 1) % self->flashingDots->frequency;
        }
        if (self->flashingDots->count < (self->flashingDots->frequency / 2)) {
            if (self->currentDigit >= self->flashingDots->from) {
                if (self->currentDigit <= self->flashingDots->to) {
                    result = 0;
                }
            }
        }
    }
    return result;
}

/* === Public function implementation ============================================================================== */

screenT ScreenCreate(uint8_t digits, screenDriverT driver) {
    screenT self = malloc(sizeof(struct screenS));
    if (digits > SCREEN_MAX_DIGITS) {
        digits = SCREEN_MAX_DIGITS;
    }
    if (self != NULL) {
        self->digits = digits;
        self->driver = driver;
        self->currentDigit = 0;
        self->flashingDigits->count = 0;
        self->flashingDigits->frequency = 0;
        self->flashingDots->count = 0;
        self->flashingDots->frequency = 0;
    }
    return self;
}

void ScreenWriteBCD(screenT self, uint8_t * value, uint8_t size) {
    memset(self->value, 0, sizeof(self->value)); // establece todos los elementos a 0

    if (size > self->digits) {
        size = self->digits;
    }
    for (uint8_t i = 0; i < size; i++) {
        self->value[i] = IMAGES[value[i]];
    }
}

void ScreenWriteDot(screenT self, uint8_t * dots, uint8_t size){
    memset(self->dots, 0, sizeof(self->dots));   // establece todos los elementos a 0

    if (size > self->digits) {
        size = self->digits;
    }
    for (uint8_t i = 0; i < size; i++) {
        self->dots[i] = dots[i] ? SEGMENT_DP : 0; // Si dots es NULL, se asigna 0
    }
}

void ScreenRefresh(screenT self) {
    uint8_t segments, dots;

    self->driver->DigitsTurnOff();
    self->currentDigit = (self->currentDigit + 1) % self->digits;

    segments = FlashingDig(self);
    dots = FlashingDot(self);

    self->driver->SegmentsUpdates(segments, dots);
    self->driver->DigitTurnOn(self->currentDigit);
}

int ScreenFlashDigits(screenT self, uint8_t from, uint8_t to, uint16_t divisor) {
    int result = 0;
    if (from > to || from >= SCREEN_MAX_DIGITS || to >= SCREEN_MAX_DIGITS) {
        result = -1; // Error: from debe ser menor o igual a to
    } else if (!self) {
        result = -1; // Error: pantalla no inicializada
    } else {
        self->flashingDigits->from = from;
        self->flashingDigits->to = to;
        self->flashingDigits->frequency = 2 * divisor;
        self->flashingDigits->count = 0;
        self->flashingDigits->selector = 0;
    }

    return result;
}

int ScreenFlashDots(screenT self, uint8_t from, uint8_t to, uint16_t divisor) {
    int result = 0;
    if (from > to || from >= SCREEN_MAX_DIGITS || to >= SCREEN_MAX_DIGITS) {
        result = -1; // Error: from debe ser menor o igual a to
    } else if (!self) {
        result = -1; // Error: pantalla no inicializada
    } else {
        self->flashingDots->from = from;
        self->flashingDots->to = to;
        self->flashingDots->frequency = 2 * divisor;
        self->flashingDots->count = 0;
        self->flashingDots->selector = 1;
    }

    return result;
}
/* === End of documentation ======================================================================================== */
