/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 *  ======== main_tirtos.c ========
 */
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>

/* Driver Header files */
// #include <ti/drivers/GPIO.h>
// #include <ti/display/Display.h>
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/Timer.h>
// #include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

Display_Handle display;

// extern void *sdCardThread(void *arg0);
// extern void *accelThread(void *arg0);
// extern void *motorThread(void *arg0);
// extern void *xbeeThread(void *arg0);

pthread_barrier_t barrier;
sem_t semFullAccelBuffer;   // Semaphore between accelerometer and SD card, for full buffer
sem_t semAccelData;   // Semaphore between accelerometer and SD card, for allowing SD card writes
sem_t semMoveMotors;   // Semaphore between XBee and motors


/* Stack size in bytes */
#define THREADSTACKSIZE    1024

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;

    /* Call driver init functions */
    Board_initGeneral();

    GPIO_init();
    Display_init();
    PWM_init();
    SPI_init();
    SDSPI_init();
    Timer_init();
    UART_init();

    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) while (1); // Failed to open display driver

    // Initialize barrier to sync XBee, accelerometer, and SD card threads
    pthread_barrier_init(&barrier, NULL, 3);

        // Initialize semaphore for changing to a filled accelerometer data buffer
    retc = sem_init(&semFullAccelBuffer, 0, 0);
    if (retc == -1) while (1);  // sem_init() failed

    // Initialize semaphore for accelerometer data
    retc = sem_init(&semAccelData, 0, 0);
    if (retc == -1) while (1);  // sem_init() failed

    // Initialize semaphore for motor movement
    retc = sem_init(&semMoveMotors, 0, 0);
    if (retc == -1) while (1);  // sem_init() failed


    // Set priority and stack size attributes
    pthread_attr_init(&pAttrs);

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if (retc != 0) while (1);  // pthread_attr_setdetachstate() failed

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if (retc != 0) while (1);  // pthread_attr_setstacksize() failed


    /* Create threads */

    // Create motor controller thread
    priParam.sched_priority = 1;
    pthread_attr_setschedparam(&pAttrs, &priParam);
    retc = pthread_create(&thread, &pAttrs, motorThread, NULL);
    if (retc != 0) while (1);  // pthread_create() failed

    // Create GPS thread
    priParam.sched_priority = 2;
    pthread_attr_setschedparam(&pAttrs, &priParam);
    // retc = pthread_create(&thread, &pAttrs, gpsThread, NULL);
    if (retc != 0) while (1);  // pthread_create() failed


    // Create SD card thread
    priParam.sched_priority = 3;
    pthread_attr_setschedparam(&pAttrs, &priParam);
    retc = pthread_create(&thread, &pAttrs, sdCardThread, NULL);
    if (retc != 0) while (1);  // pthread_create() failed

    // Create accelerometer thread
    priParam.sched_priority = 4;
    pthread_attr_setschedparam(&pAttrs, &priParam);
    retc = pthread_create(&thread, &pAttrs, accelThread, NULL);
    if (retc != 0) while (1);  // pthread_create() failed

    // Create XBee thread
    priParam.sched_priority = 5;
    pthread_attr_setschedparam(&pAttrs, &priParam);
    retc = pthread_create(&thread, &pAttrs, xbeeThread, NULL);
    if (retc != 0) while (1);  // pthread_create() failed

    BIOS_start();

    return (0);
}
