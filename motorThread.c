/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

#include <stdint.h>

#include "PololuQik.h"

#define TASKSTACKSIZE     768

extern Display_Handle display;

int32_t encoderCount = 0;

//Void xbeeFxn(UArg arg0, UArg arg1)
//{
//    char input[8];
//    UART_Handle uartMotor;
//    UART_Params uartParams;
//    const char echoPrompt[] = "\fEchoing characters:\r\n";
//
//    const char timePrompt[] = "time (ms) = "
//    const char countPrompt[] = ", encoderCount = ";
//    const char newLinePrompt[] = "\r\n";
//
//    /* Create a UART with data processing off. */
//    UART_Params_init(&uartParams);
//    uartParams.writeDataMode = UART_DATA_BINARY;
//    uartParams.readDataMode = UART_DATA_TEXT;   // UART_DATA_BINARY;
//    uartParams.readReturnMode = UART_RETURN_NEWLINE;    // UART_RETURN_FULL;
//    uartParams.readEcho = UART_ECHO_ON; // UART_ECHO_OFF;
//    uartParams.baudRate = 9600;
//    uartMotor = UART_open(Board_UART0, &uartParams);
//
//    if (uartMotor == NULL) {
//        System_abort("Error opening the UART");
//    }
//
//    UART_write(uartMotor, echoPrompt, sizeof(echoPrompt));
//
//    /* Loop forever echoing */
//    while (1) {
////        UART_read(uartMotor, &input, 8);
////        int i = 0;
//////        for (i=0; i<8; i++) {
//////            input[i]++;
//////        }
////        while (input[i] != '\n' && i < 8) {
////            input[i++]++;
////        }
//        Timer_init();
//
//        UART_write(uartMotor, timePrompt, sizeof(timePrompt));
////        UART_write(uartMotor, )
//        UART_write(uartMotor, echoPrompt, sizeof(echoPrompt));
//        UART_write(uartMotor, &encoderCount, sizeof(encoderCount));
//        UART_write(uartMotor, newLinePromt, sizeof(newLinePrompt));
//        Task_sleep(2000);
//    }
//
//}


/*
 *  ======== echoFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
void *motorThread(void *arg0)
{
    char input;
    UART_Handle uartMotor;
    UART_Params uartParams;
    const char echoPrompt[] = "\fEchoing characters:\r\n";

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uartMotor = UART_open(Board_UART1, &uartParams); // Changed from Board_UART0 (PC UART) to Board_UART1 (P3.2 and P3.3)

    if (uartMotor == NULL) while (1);  // Failed to open motor UART

    PololuQik_init(uartMotor);
    input = PololuQik_getFirmwareVersion(uartMotor);
    // UART_write(uartPC, &input, 1);
    Display_printf(display, 0, 0, "%c", input);

    //    UART_write(uartMotor, echoPrompt, sizeof(echoPrompt));
    //    System_printf("Serial starting\n" );

    /* Loop forever echoing */
//    while (1) {
//        UART_read(uartMotor, &input, 1);
//        UART_write(uartMotor, &input, 1);
//    }

    int i;
    PololuQik_setSpeeds(uartMotor, 64, 64);
    Task_sleep(5000);
    PololuQik_setSpeeds(uartMotor, 0, 0);
    while (1) {
//        for (i = 0; i <= 127; i++) {
//            PololuQik_setSpeeds(uartMotor, i, -i);
////            PololuQik_setM0Speed(uartMotor, i);
//            Task_sleep(20);
//        }
//
//        for (i = 127; i >= -127; i--) {
//            PololuQik_setSpeeds(uartMotor, i, -i);
////            PololuQik_setM0Speed(uartMotor, i);
//            Task_sleep(20);
//        }
//
//        for (i = -127; i <= 0; i++) {
//            PololuQik_setSpeeds(uartMotor, i, -i);
////            PololuQik_setM0Speed(uartMotor, i);
//            Task_sleep(20);
//        }

//        for (i = 0; i <= 127; i++) {
//            PololuQik_setM1Speed(uartMotor, i);
//            Task_sleep(50);
//        }
//
//        for (i = 127; i >= -127; i--) {
//            PololuQik_setM1Speed(uartMotor, i);
//            Task_sleep(50);
//        }
//
//        for (i = -127; i <= 0; i++) {
//            PololuQik_setM1Speed(motorUART, i);
//            Task_sleep(50);
//        }
    }

    return (NULL);
}

//void gpioButtonFxn0(unsigned int index)
//{
//    /* Clear the GPIO interrupt and toggle an LED */
//
//    triggeredVal = GPIO_read(4);
//    otherVal = GPIO_read(5);
//    direction = (triggeredVal ^ otherVal) & 0x01;
//    direction ? encoderCount++ : encoderCount--;
//    GPIO_write(Board_LED0, (/*triggeredVal &*/ direction));
//    GPIO_write(Board_LED1, (/*triggeredVal &*/ (0x01 ^ direction)));
////    System_printf("triggeredVal = %d, otherVal = %d\n", triggeredVal, otherVal);
////    System_flush();
//}
