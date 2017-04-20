/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
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
 *  ======== xbeeThread.c ========
 */
#include <stdio.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* Driver Header files */
// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

/* Console display strings */
const char textarray[] = \
"***********************************************************************\n"
"0         1         2         3         4         5         6         7\n"
"01234567890123456789012345678901234567890123456789012345678901234567890\n"
"This is some text to be inserted into the inputfile if there isn't\n"
"already an existing file located on the media.\n"
"If an inputfile already exists, or if the file was already once\n"
"generated, then the inputfile will NOT be modified.\n"
"***********************************************************************\n";


const char consoleDisplay[]   = "\fConsole (h for help)\r\n";
const char helpPrompt[]       = "Valid Commands\r\n"                  \
                                "--------------\r\n"                  \
                                "h: help\r\n"                         \
                                "q: quit and shutdown UART\r\n"       \
                                "c: clear the screen\r\n"             \
                                "r: record accelerometer data\r\n";
const char byeDisplay[]       = "Bye! Hit button1 to start UART again\r\n";
// const char tempStartDisplay[] = "Current temp = ";
// const char tempMidDisplay[]   = "C (";
// const char tempEndDisplay[]   = "F)\r\n";
const char cleanDisplay[]     = "\f";
const char userPrompt[]       = "> ";
const char readErrDisplay[]   = "Problem read UART.\r\n";

const char spaceDisplay[] = " ";
const char newLineDisplay[] = "\r\n";

const char recordAccelDisplay[] = "Reading the accelerometer for 5 seconds...\r\n";
const char recordAccelDoneDisplay[] = "Accelerometer reading complete\r\n";

const char moveMotorsDisplay[] = "Encoder counts for Motor 1 and 0: ";
const char moveMotorsDoneDisplay[] = "Robot has moved 5000 ticks. Counts = ";

char tempStr[50];

/* Used to determine whether to have the thread block */
volatile bool uartEnabled = true;
// sem_t semConsole;

// extern pthread_barrier_t accelDataBarrier;

// extern Display_Handle display;

// extern int count0;
// extern int count1;
// extern int count0Copy;
// extern int count1Copy;
// extern bool keepMoving;
// extern sem_t semMoveMotors;

UART_Handle uartXbee;
char count0Str[16];
char count1Str[16];

extern int motor0Speed;
extern int motor1Speed;
char motor0Str[16];
char motor1Str[16];
sem_t semPrintMotorEncoders;
// bool semPrintMotorEncoders = true;

/* Temperature written by the temperature thread and read by console thread */
// extern float temperatureC;
// extern float temperatureF;

/* Mutex to protect the reading/writing of the float temperature */
// extern pthread_mutex_t temperatureMutex;

/* Used itoa instead of sprintf to help minimize the size of the stack */
static void itoa(int n, char s[]);

void encoderTimerCallback(Timer_Handle myHandle)
{
    count0Copy = count0;
    count1Copy = count1;

    itoa(count0Copy, count0Str);
    itoa(count1Copy, count1Str);
    itoa(motor0Speed, motor0Str);
    itoa(motor1Speed, motor1Str);
    sem_post(&semPrintMotorEncoders);
    sem_post(&semMoveMotors);
    // semPrintMotorEncoders = true;

    // UART_write(uartXbee, moveMotorsDisplay, sizeof(moveMotorsDisplay));
    // UART_write(uartXbee, tempStr, strlen(tempStr));
    // UART_write(uartXbee, newLineDisplay, sizeof(newLineDisplay));
}

/*
 *  ======== xbeeThread ========
 */
void *xbeeThread(void *arg0)
{
    UART_Params uartParams;
    // UART_Handle uartXbee;
    int retc;

    char cmd;
    // char tempStr[8];
    // char tempStr[16];

    /* install Button callback and enable it */
    // GPIO_setCallback(Board_GPIO_BUTTON1, gpioButtonFxn);
    // GPIO_enableInt(Board_GPIO_BUTTON1);

    // retc = sem_init(&semConsole, 0, 0);
    // if (retc == -1) {
    //     while (1);
    // }

    // UART_init();

    Timer_Handle timer1;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = 100000;     // 100,000 us = 0.1 s
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;   // Timer_FREE_RUNNING;
    params.timerCallback = encoderTimerCallback;

    timer1 = Timer_open(Board_TIMER1, &params);
    if (timer1 == NULL) while (1);   // Failed to initialized timer

    /*
     *  Initialize the UART parameters outside the loop. Let's keep
     *  most of the defaults (e.g. baudrate = 115200) and only change the
     *  following.
     */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode  = UART_DATA_BINARY;
    uartParams.readDataMode   = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 9600;

    uartXbee = UART_open(Board_UART3, &uartParams);
    if (uartXbee == NULL) while (1);    // Failed to open XBee UART

    Display_printf(display, 0, 0, "XBee UART opened");

    UART_write(uartXbee, consoleDisplay, sizeof(consoleDisplay));

    retc = sem_init(&semPrintMotorEncoders, 0, 0);
    if (retc == -1) while (1);  // sem_init() failed

    /* Loop until read fails or user quits */
    while (1) {
        UART_write(uartXbee, userPrompt, sizeof(userPrompt));
        retc = UART_read(uartXbee, &cmd, sizeof(cmd));
        if (retc == 0) {
            UART_write(uartXbee, readErrDisplay, sizeof(readErrDisplay));
            cmd = 'q';
        }

        switch (cmd) {
            case 'y':
                UART_write(uartXbee, textarray, sizeof(textarray));
                break;
            case 'r':
                // retc = sem_post(&semAccelData);
                // if (retc == -1) while (1);

                pthread_barrier_wait(&accelDataBarrier); // Deploy accelerometer; open new file on SD card

                pthread_barrier_wait(&accelDataBarrier);
                
                // UART_write(uartXbee, recordAccelDisplay, sizeof(recordAccelDisplay));
                sprintf(tempStr, "Reading the accelerometer for 5 seconds...\r\n");
                UART_write(uartXbee, tempStr, strlen(tempStr));

                pthread_barrier_wait(&accelDataBarrier);

                // UART_write(uartXbee, recordAccelDoneDisplay, sizeof(recordAccelDoneDisplay));
                sprintf(tempStr, "Accelerometer reading complete\r\n");
                UART_write(uartXbee, tempStr, strlen(tempStr));

                break;
            case 'c':
                UART_write(uartXbee, cleanDisplay, sizeof(cleanDisplay));
                break;
            case 'q':
                UART_write(uartXbee, byeDisplay, sizeof(byeDisplay));
                UART_close(uartXbee);
                return (NULL);
            case 'm':
                retc = sem_post(&semMoveMotors);
                if (retc == -1) while (1);

                if (Timer_start(timer1) == Timer_STATUS_ERROR) while (1); // Failed to start timer


                // while (keepMoving) {
                //     // sem_wait(&semMoveMotors);

                //     // Display_printf(display, 0, 0, "motor0 count = %d", count0Copy);

                //     itoa(count0Copy, tempStr);
                //     UART_write(uartXbee, moveMotorsDisplay, sizeof(moveMotorsDisplay));
                //     UART_write(uartXbee, tempStr, strlen(tempStr));
                //     UART_write(uartXbee, newLineDisplay, sizeof(newLineDisplay));

                //     sem_wait(&semMoveMotors);

                //     // usleep(500000); // 500,000 us = 0.5 s
                // }

                // while(keepMoving);

                while (keepMoving) {
                    // itoa(count0Copy, tempStr);
                    UART_write(uartXbee, moveMotorsDisplay, sizeof(moveMotorsDisplay));
                    UART_write(uartXbee, count1Str, strlen(count1Str));
                    UART_write(uartXbee, spaceDisplay, sizeof(spaceDisplay));
                    UART_write(uartXbee, count0Str, strlen(count0Str));
                    // UART_write(uartXbee, newLineDisplay, sizeof(newLineDisplay));

                    UART_write(uartXbee, spaceDisplay, sizeof(spaceDisplay));
                    UART_write(uartXbee, motor1Str, strlen(motor1Str));
                    UART_write(uartXbee, spaceDisplay, sizeof(spaceDisplay));
                    UART_write(uartXbee, motor0Str, strlen(motor0Str));
                    UART_write(uartXbee, newLineDisplay, sizeof(newLineDisplay));

                    // usleep(200000);
                    sem_wait(&semPrintMotorEncoders);
                    // semPrintMotorEncoders = false;
                    // while (!semPrintMotorEncoders);
                }

                Timer_stop(timer1);

                sem_wait(&semMoveMotors);
                count0Copy = count0;
                count1Copy = count1;
                itoa(count0Copy, count0Str);
                itoa(count1Copy, count1Str);

                UART_write(uartXbee, moveMotorsDoneDisplay, sizeof(moveMotorsDoneDisplay));
                UART_write(uartXbee, count1Str, strlen(count1Str));
                UART_write(uartXbee, spaceDisplay, sizeof(spaceDisplay));
                UART_write(uartXbee, count0Str, strlen(count0Str));
                UART_write(uartXbee, newLineDisplay, sizeof(newLineDisplay));

                // sem_post(&semMoveMotors);

                break;
            default:
                UART_write(uartXbee, helpPrompt, sizeof(helpPrompt));
                break;

        }
    }

    UART_close(uartXbee);

    return (NULL);
}

/*
 * The following function is from good old K & R.
 */
static void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/*
 * The following function is from good old K & R.
 */
static void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
         s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}
