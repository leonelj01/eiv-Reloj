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

/** @file digital.c
 ** @brief Funciones para el manejo de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */

#include "chip.h"
#include "config.h"
#include "digital.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

//! Representa una salida digital
struct digitalOutputS {
    uint8_t port; //!< Puerto al que pertenece la salida
    uint8_t pin;  //!< Pin al que pertenece la salida
    bool state;   //!< Estado inicial de la salida
};

//! Representa una entrada digital
struct digitalInputS {
    uint8_t port;   //!< Puerto al que pertenece la entrada
    uint8_t pin;    //!< Pin al que pertenece la entrada
    bool inverted;  //!< Indica si la entrada es invertida
    bool lastState; //!< Estado anterior de la entrada
};
/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

digitalOutputT DigitalOutputCreate(uint8_t port, uint8_t pin, bool state) {
    digitalOutputT self = malloc(sizeof(struct digitalOutputS));
    if (self != NULL) {
        self->port = port;
        self->pin = pin;
        self->state = state;

        Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, self->state);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, true);
    }

    return self;
}

void DigitalOutputActivate(digitalOutputT self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, !self->state);
}

void DigitalOutputDesactivate(digitalOutputT self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, self->state);
}

void DigitalOutputToggle(digitalOutputT self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->port, self->pin);
}

digitalInputT DigitalInputCreate(uint8_t port, uint8_t pin, bool inverted) {
    digitalInputT self = malloc(sizeof(struct digitalInputS));
    if (self != NULL) {
        self->port = port;
        self->pin = pin;
        self->inverted = inverted;

        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, false);

        self->lastState = DigitalInputGetActivate(self);
    }

    return self;
}

bool DigitalInputGetActivate(digitalInputT self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->port, self->pin);

    if (self->inverted) {
        return !state;
    }

    return state;
}

digitalStates DigitalInputWasChanged(digitalInputT self) {

    digitalStates result = DIGITAL_INPUT_NO_CHANGE;

    bool stateAct = DigitalInputGetActivate(self);

    if (stateAct && !self->lastState) {
        result = DIGITAL_INPUT_WAS_ACTIVATE;
    } else if (!stateAct && self->lastState) {
        result = DIGITAL_INPUT_WAS_DEACTIVATE;
    }
    self->lastState = stateAct;

    return result;
}

bool DigitalInputWasActivated(digitalInputT self) {
    return DIGITAL_INPUT_WAS_ACTIVATE == DigitalInputWasChanged(self);
}

bool DigitalInputWasDeactivated(digitalInputT self) {
    return DIGITAL_INPUT_WAS_DEACTIVATE == DigitalInputWasChanged(self);
}
/* === End of documentation ======================================================================================== */
