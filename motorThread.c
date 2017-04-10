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
#include <stdint.h>
#include <stddef.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* TI-RTOS Header files */
// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

#include <stdint.h>

#include "PololuQik.h"

#define TASKSTACKSIZE     768

extern Display_Handle display;

pthread_mutex_t encoderCount0Mutex;
pthread_mutex_t encoderCount1Mutex;

extern sem_t semMoveMotors;

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

int count0 = 0;
int count1 = 0;
int count0Copy = 0;
int count1Copy = 0;
bool keepMoving = true;

uint8_t triggeredVal0;
uint8_t otherVal0;
uint8_t direction0;

uint8_t triggeredVal1;
uint8_t otherVal1;
uint8_t direction1;

void motor0_encoderAInt(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */

    // triggeredVal0 = GPIO_read(4);
    otherVal0 = GPIO_read(9);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    // pthread_mutex_lock(&encoderCount0Mutex);
    otherVal0 ? count0++ : count0--;
    // GPIO_write(Board_GPIO_LED0, (/*triggeredVal &*/ direction0));
    // pthread_mutex_unlock(&encoderCount0Mutex);

    if (count0 >= 5000) keepMoving = false;

    // GPIO_write(Board_GPIO_LED0, otherVal0);

//    GPIO_write(Board_GPIO_LED1, (/*triggeredVal &*/ (0x01 ^ direction0)));
//    System_printf("triggeredVal = %d, otherVal = %d\n", triggeredVal, otherVal);
//    System_flush();
}

void motor0_encoderBInt(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */

    // triggeredVal0 = GPIO_read(4);
    otherVal0 = GPIO_read(8);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    // pthread_mutex_lock(&encoderCount0Mutex);
    otherVal0 ? count0-- : count0++;
    // GPIO_write(Board_GPIO_LED0, (/*triggeredVal &*/ direction0));
    // pthread_mutex_unlock(&encoderCount0Mutex);

    if (count0 >= 5000) keepMoving = false;

    // GPIO_write(Board_GPIO_LED1, otherVal0);

//    GPIO_write(Board_GPIO_LED1, (/*triggeredVal &*/ (0x01 ^ direction0)));
//    System_printf("triggeredVal = %d, otherVal = %d\n", triggeredVal, otherVal);
//    System_flush();
}

/* Motor 1 moves in the opposite direction as Motor 0 when going forward, 
 * so flip the count increment/decrements
 */
void motor1_encoderAInt(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */

    // triggeredVal0 = GPIO_read(4);
    otherVal1 = GPIO_read(11);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    otherVal1 ? count1-- : count1++;

    if (count1 >= 5000) keepMoving = false;
}

void motor1_encoderBInt(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */

    // triggeredVal0 = GPIO_read(4);
    otherVal1 = GPIO_read(10);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    otherVal1 ? count1++ : count1--;

    if (count1 >= 5000) keepMoving = false;
}

// void encoderTimerCallback(Timer_Handle myHandle)
// {
//     // pthread_mutex_lock(&encoderCount0Mutex);
//     count0Copy = count0;
//     // count1Copy = count1;
//     // pthread_mutex_unlock(&encoderCount0Mutex);

//     // // Differential code for motors
//     // countdiff = count0 - count1;
//     // if (countdiff > 32) {
//     //     countdiff = 32;
//     // } else if (countdiff < -32) {
//     //     countdiff = -32;
//     // }
//     // PololuQik_setSpeeds(uart, 64 + countdiff, -64 - countdiff);

//     sem_post(&semMoveMotors);

//     // Display_printf(display, 0, 0, "motor0 count = %d", count0Copy);

//     // encoderCount++;
//     // Display_printf(display, 0, 0, "time = %d s", encoderCount);

// }

/*
 *  ======== echoFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
void *motorThread(void *arg0)
{
    char input;
    UART_Handle uartMotor;
    UART_Params uartParams;
    // const char echoPrompt[] = "\fEchoing characters:\r\n";
    int retc;

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

    retc = pthread_mutex_init(&encoderCount0Mutex, NULL);
    if (retc != 0) while (1);   // pthread_mutex_init() failed

    // Timer_Handle timer1;
    // Timer_Params params;

    // Timer_Params_init(&params);
    // params.period = 200000;    // 200,000 us = 0.2 s
    // params.periodUnits = Timer_PERIOD_US;
    // params.timerMode = Timer_CONTINUOUS_CALLBACK;   // Timer_FREE_RUNNING;
    // params.timerCallback = encoderTimerCallback;

    // timer1 = Timer_open(Board_TIMER1, &params);
    // if (timer1 == NULL) while (1);   // Failed to initialized timer

    GPIO_setCallback(8, motor0_encoderAInt);
    GPIO_setCallback(9, motor0_encoderBInt);
    GPIO_setCallback(10, motor1_encoderAInt);
    GPIO_setCallback(11, motor1_encoderBInt);


    /* Enable interrupts */
    GPIO_enableInt(8);
    GPIO_enableInt(9);
    GPIO_enableInt(10);
    GPIO_enableInt(11);

    // if (Timer_start(timer1) == Timer_STATUS_ERROR) while (1); // Failed to start timer

    //    UART_write(uartMotor, echoPrompt, sizeof(echoPrompt));
    //    System_printf("Serial starting\n" );

    /* Loop forever echoing */
//    while (1) {
//        UART_read(uartMotor, &input, 1);
//        UART_write(uartMotor, &input, 1);
//    }

    
    // PololuQik_setSpeeds(uartMotor, 64, 64);
    // Task_sleep(5000);
    // PololuQik_setSpeeds(uartMotor, 0, 0);

    while (1) {
        count0 = 0;
        count1 = 0;
        keepMoving = true;
        retc = sem_wait(&semMoveMotors);
        if (retc == -1) while (1);

        // if (Timer_start(timer1) == Timer_STATUS_ERROR) while (1); // Failed to start timer
        int motor0Speed = 64;
        int motor1Speed = 64;
        PololuQik_setSpeeds(uartMotor, motor0Speed, -motor1Speed);

        // while (keepMoving);
        int error;
        while (keepMoving) {
            error = (count0Copy - count1Copy)/8/2;
            // if (error > 0) {
                motor0Speed -= error;
                if (motor0Speed > 127) {
                    motor0Speed = 127;
                } else if (motor0Speed < -127) {
                    motor0Speed = -127;
                }

                motor1Speed += error;
                if (motor1Speed > 127) {
                    motor1Speed = 127;
                } else if (motor1Speed < -127) {
                    motor1Speed = -127;
                }
            // } else {
            //     motor0Speed += error;
            //     if (motor0Speed > 127) {
            //         motor0Speed = 127;
            //     } else if (motor0Speed < -127) {
            //         motor0Speed = -127;
            //     }

            //     motor1Speed -= error;
            //     if (motor1Speed > 127) {
            //         motor1Speed = 127;
            //     } else if (motor1Speed < -127) {
            //         motor1Speed = -127;
            //     }
            // }
            
            PololuQik_setSpeeds(uartMotor, motor0Speed, -motor1Speed);

            sem_wait(&semMoveMotors);
        }

        // Timer_stop(timer1);
        PololuQik_setSpeeds(uartMotor, 0, 0);
        // count0Copy = count0;

        // sem_wait(&semMoveMotors);
        sleep(1);

        sem_post(&semMoveMotors);
        // sem_wait(&semMoveMotors);

        // count0Copy = count0;
        // pthread_mutex_unlock(&encoderCount0Mutex);

        Display_printf(display, 0, 0, "Final motors count = %d %d", count0, count1);
        // encoderCount++;
        // Display_printf(display, 0, 0, "time = %d s", encoderCount);
        // sleep(1);
    }

    // int i;
//    while (1) {
//        for (i = 0; i <= 127; i++) {
//            PololuQik_setSpeeds(uartMotor, i, -i);
// //            PololuQik_setM0Speed(uartMotor, i);
//            Task_sleep(20);
//        }

//        for (i = 127; i >= -127; i--) {
//            PololuQik_setSpeeds(uartMotor, i, -i);
// //            PololuQik_setM0Speed(uartMotor, i);
//            Task_sleep(20);
//        }

//        for (i = -127; i <= 0; i++) {
//            PololuQik_setSpeeds(uartMotor, i, -i);
// //            PololuQik_setM0Speed(uartMotor, i);
//            Task_sleep(20);
//        }

//        for (i = 0; i <= 127; i++) {
//            PololuQik_setM1Speed(uartMotor, i);
//            Task_sleep(50);
//        }

//        for (i = 127; i >= -127; i--) {
//            PololuQik_setM1Speed(uartMotor, i);
//            Task_sleep(50);
//        }

//        for (i = -127; i <= 0; i++) {
//            PololuQik_setM1Speed(motorUART, i);
//            Task_sleep(50);
//        }
//     }

    return (NULL);
}

//void motor0_encoderAInt(unsigned int index)
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
