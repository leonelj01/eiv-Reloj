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

/** @file bsp.c
 ** @brief Plantilla para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "bsp.h"
#include "chip.h"
#include "digital.h"
#include "edu-ciaa.h"
#include "poncho.h"
#include "screen.h"
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

static void DigitsInit(void);

static void SegmentsInit(void);

/**
 * @brief Apaga todos los dígitos del display.
 *
 */
static void DigitsTurnOff(void);

/**
 * @brief Actualiza los segmentos del display con el valor proporcionado.
 *
 * @param value Valor que se mostrará en los segmentos del display.
 */
static void SegmentsUpdates(uint8_t value);

/**
 * @brief Enciende el dígito especificado en el display.
 *
 * @param digit Índice del dígito a encender (0 a 3).
 */
static void DigitTurnOn(uint8_t digit);

/* === Private variable definitions ================================================================================ */

static const struct screenDriverS screenDriver = {
    .DigitsTurnOff = DigitsTurnOff, .SegmentsUpdates = SegmentsUpdates, .DigitTurnOn = DigitTurnOn};

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static void DigitsInit(void) {
    Chip_SCU_PinMuxSet(DIGIT_1_PORT, DIGIT_1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_1_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, DIGIT_1_GPIO, DIGIT_1_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_1_GPIO, DIGIT_1_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_2_PORT, DIGIT_2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_2_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, DIGIT_2_GPIO, DIGIT_2_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_2_GPIO, DIGIT_2_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_3_PORT, DIGIT_3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_3_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, DIGIT_3_GPIO, DIGIT_3_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_3_GPIO, DIGIT_3_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_4_PORT, DIGIT_4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_4_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, DIGIT_4_GPIO, DIGIT_4_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_4_GPIO, DIGIT_4_BIT, true);
}

static void SegmentsInit(void) {
    Chip_SCU_PinMuxSet(SEGMENT_A_PORT, SEGMENT_A_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_A_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_A_GPIO, SEGMENT_A_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_A_GPIO, SEGMENT_A_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_B_PORT, SEGMENT_B_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_B_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_B_GPIO, SEGMENT_B_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_B_GPIO, SEGMENT_B_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_C_PORT, SEGMENT_C_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_C_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_C_GPIO, SEGMENT_C_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_C_GPIO, SEGMENT_C_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_D_PORT, SEGMENT_D_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_D_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_D_GPIO, SEGMENT_D_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_D_GPIO, SEGMENT_D_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_E_PORT, SEGMENT_E_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_E_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_E_GPIO, SEGMENT_E_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_E_GPIO, SEGMENT_E_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_F_PORT, SEGMENT_F_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_F_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_F_GPIO, SEGMENT_F_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_F_GPIO, SEGMENT_F_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_G_PORT, SEGMENT_G_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_G_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_G_GPIO, SEGMENT_G_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_G_GPIO, SEGMENT_G_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_DP_PORT, SEGMENT_DP_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_DP_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_DP_GPIO, SEGMENT_DP_BIT, false);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_DP_GPIO, SEGMENT_DP_BIT, true);
}

static void DigitsTurnOff(void) {
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, DIGITS_GPIO, DIGITS_MASK);
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, SEGMENTS_GPIO, SEGMENTS_MASK);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_DP_GPIO, SEGMENT_DP_BIT, false);
}

static void SegmentsUpdates(uint8_t value) {
    Chip_GPIO_SetValue(LPC_GPIO_PORT, SEGMENTS_GPIO, value & SEGMENTS_MASK);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_DP_GPIO, SEGMENT_DP_BIT, (value & SEGMENT_DP));
}

static void DigitTurnOn(uint8_t digit) {
    Chip_GPIO_SetValue(LPC_GPIO_PORT, DIGITS_GPIO, (1 << (3 - digit)) & DIGITS_MASK);
}

/* === Public function implementation ============================================================================== */

boardT BoardCreate(void) {
    struct boardS * board = malloc(sizeof(struct boardS));

    if (board != NULL) {
        DigitsInit();
        SegmentsInit();
        board->screen = ScreenCreate(4, &screenDriver);

        // Inicialización de las salidas del poncho

        Chip_SCU_PinMuxSet(RGB_RED_PORT, RGB_RED_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | RGB_RED_FUNC);
        board->ledRed = DigitalOutputCreate(RGB_RED_GPIO, RGB_RED_BIT, true);

        Chip_SCU_PinMuxSet(RGB_GREEN_PORT, RGB_GREEN_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | RGB_GREEN_FUNC);
        board->ledGreen = DigitalOutputCreate(RGB_GREEN_GPIO, RGB_GREEN_BIT, true);

        Chip_SCU_PinMuxSet(RGB_BLUE_PORT, RGB_BLUE_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | RGB_BLUE_FUNC);
        board->ledBlue = DigitalOutputCreate(RGB_BLUE_GPIO, RGB_BLUE_BIT, true);

        // Inicialización de las entradas del poncho
        Chip_SCU_PinMuxSet(KEY_F1_PORT, KEY_F1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | KEY_F1_FUNC);
        board->setTime = DigitalInputCreate(KEY_F1_GPIO, KEY_F1_BIT, true);

        Chip_SCU_PinMuxSet(KEY_F2_PORT, KEY_F2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | KEY_F2_FUNC);
        board->setAlarm = DigitalInputCreate(KEY_F2_GPIO, KEY_F2_BIT, true);

        Chip_SCU_PinMuxSet(KEY_F3_PORT, KEY_F3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | KEY_F3_FUNC);
        board->decrement = DigitalInputCreate(KEY_F3_GPIO, KEY_F3_BIT, true);

        Chip_SCU_PinMuxSet(KEY_F4_PORT, KEY_F4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | KEY_F4_FUNC);
        board->increment = DigitalInputCreate(KEY_F4_GPIO, KEY_F4_BIT, true);

        Chip_SCU_PinMuxSet(KEY_ACCEPT_PORT, KEY_ACCEPT_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | KEY_ACCEPT_FUNC);
        board->accept = DigitalInputCreate(KEY_ACCEPT_GPIO, KEY_ACCEPT_BIT, true);

        Chip_SCU_PinMuxSet(KEY_CANCEL_PORT, KEY_CANCEL_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | KEY_CANCEL_FUNC);
        board->cancel = DigitalInputCreate(KEY_CANCEL_GPIO, KEY_CANCEL_BIT, true);
    }

    return board;
}

void SysTickInit(uint16_t ticks) {
    __asm volatile("cpsid i"); // Deshabilita las interrupciones

    SystemCoreClockUpdate(); // Actualiza la frecuencia del núcleo del sistema
    SysTick_Config(SystemCoreClock / ticks); // Configura SysTick para interrupciones cada 1 ms

    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1); // Establece la prioridad más baja para SysTick
    
    __asm volatile("cpsie i"); // Habilita las interrupciones
}

/* === End of documentation ======================================================================================== */
