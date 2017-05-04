/*
 * Bridge Inspection Robot Team
 * ECE 4012 Spring 2017 PV2
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
 *  ======== motorThread.c ========
 */

/* XDCtools Header files */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* Example/Board Header files */
#include "Board.h"

#include <stdint.h>

#include "PololuQik.h"

#define TASKSTACKSIZE     768

// extern Display_Handle display;

pthread_mutex_t encoderCount0Mutex;
pthread_mutex_t encoderCount1Mutex;

// extern sem_t semMoveMotors;

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
bool keepMoving = false;

uint8_t triggeredVal0;
uint8_t otherVal0;
uint8_t direction0;

uint8_t triggeredVal1;
uint8_t otherVal1;
uint8_t direction1;

void motor0_encoderAInt(uint_least8_t index)
{
    // triggeredVal0 = GPIO_read(4);
    otherVal0 = GPIO_read(9);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    // pthread_mutex_lock(&encoderCount0Mutex);
    otherVal0 ? count0++ : count0--;
    // count0++;
    // GPIO_write(Board_GPIO_LED0, (/*triggeredVal &*/ direction0));

   if (abs(count0) >= MOVE_DIST) keepMoving = false;
}

void motor0_encoderBInt(uint_least8_t index)
{
    // triggeredVal0 = GPIO_read(4);
    otherVal0 = GPIO_read(8);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    // pthread_mutex_lock(&encoderCount0Mutex);
    otherVal0 ? count0-- : count0++;
    // count0++;

   if (abs(count0) >= MOVE_DIST) keepMoving = false;
}

/* 
 * Motor 1 moves in the opposite direction as Motor 0 when going forward, 
 * so flip the count increment/decrements
 */
void motor1_encoderAInt(uint_least8_t index)
{
    // triggeredVal0 = GPIO_read(4);
    otherVal1 = GPIO_read(11);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    otherVal1 ? count1-- : count1++;
    // count1++;

   if (abs(count1) >= MOVE_DIST) keepMoving = false;
}

void motor1_encoderBInt(uint_least8_t index)
{
    // triggeredVal0 = GPIO_read(4);
    otherVal1 = GPIO_read(10);
    // direction0 = (triggeredVal0 ^ otherVal0) & 0x01;
    // direction0 ? count0++ : count0--;

    otherVal1 ? count1++ : count1--;
    // count1++;

   if (abs(count1) >= MOVE_DIST) keepMoving = false;
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

UART_Handle uartMotor;
UART_Params uartParams;

int motor0Speed = 64;
int motor1Speed = 64;
uint8_t irFlag = 0;

void edgeDetected(uint_least8_t index) {
    // motor0Speed = 0;
    // motor1Speed = 0;
    if (!irFlag) {
        irFlag = index - 11;
    }
}

/*
 *  ======== motorThread ========
 */
void *motorThread(void *arg0)
{
//    char input;
    // UART_Handle uartMotor;
    // UART_Params uartParams;
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
    // input = PololuQik_getFirmwareVersion(uartMotor);
    // UART_write(uartPC, &input, 1);
    // Display_printf(display, 0, 0, "%c", input);

    // retc = pthread_mutex_init(&encoderCount0Mutex, NULL);
    // if (retc != 0) while (1);   // pthread_mutex_init() failed

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
    // GPIO_setCallback(9, motor0_encoderBInt);
    GPIO_setCallback(10, motor1_encoderAInt);
    // GPIO_setCallback(11, motor1_encoderBInt);

    GPIO_setCallback(12, edgeDetected);
    GPIO_setCallback(13, edgeDetected);
    GPIO_setCallback(14, edgeDetected);
    GPIO_setCallback(15, edgeDetected);


    /* Enable interrupts */
    GPIO_enableInt(8);
    GPIO_enableInt(9);
    GPIO_enableInt(10);
    GPIO_enableInt(11);

    // GPIO_enableInt(12);
    // GPIO_enableInt(13);  // disable rear IR sensors...
    // GPIO_enableInt(14);  // disable rear IR sensors...
    // GPIO_enableInt(15);

    // if (Timer_start(timer1) == Timer_STATUS_ERROR) while (1); // Failed to start timer

    //    UART_write(uartMotor, echoPrompt, sizeof(echoPrompt));
    //    System_printf("Serial starting\n" );


    // count0 = 0;
    // count1 = 0;
    // count0Copy = 0;
    // count1Copy = 0;
    // keepMoving = true;
    // retc = sem_wait(&semMoveMotors);
    // while (1) {
    //     if (irFlag) {
    //         PololuQik_setSpeeds(uartMotor, -64, 64);
    //         irFlag = 0;
    //         sleep(4);
    //     }
    //     PololuQik_setSpeeds(uartMotor, 64, -64);
    //     // sleep(4);
    //     // PololuQik_setSpeeds(uartMotor, -64, 64);
    //     // sleep(4);
    //     sem_wait(&semMoveMotors);
    // }
    bool forwards = true;

    while (1) {
        count0 = 0;
        count1 = 0;
        count0Copy = 0;
        count1Copy = 0;
        keepMoving = true;
        retc = sem_wait(&semMoveMotors);
        if (retc == -1) while (1);

//        if (Timer_start(timer1) == Timer_STATUS_ERROR) while (1); // Failed to start timer

        // int motor0Speed = 48;
        // int motor1Speed = 48;

        if (forwards) {
            PololuQik_setSpeeds(uartMotor, motor0Speed, -motor1Speed);
        } else {
            PololuQik_setSpeeds(uartMotor, -motor0Speed, motor1Speed);
        }

//       GPIO_enableInt(5);

        // while (keepMoving) {
        //     PololuQik_setSpeeds(uartMotor, motor0Speed, -motor1Speed);
        //     usleep(100000);
        // }

        int error;
        while (keepMoving) {
            if (irFlag) {
                PololuQik_setSpeeds(uartMotor, 0, 0);
                sleep(1);
                switch (irFlag) {
                    case 1:
                       // PololuQik_setSpeeds(uartMotor, -32, 32);
                       // sleep(2);
                       // PololuQik_setSpeeds(uartMotor, 16, 16);
                       // sleep(1);
                         PololuQik_setSpeeds(uartMotor, -64, 64);
                         sleep(1);
                         PololuQik_setSpeeds(uartMotor, 64, 64);
                         sleep(1);
                        break;
                    case 2:
                        PololuQik_setSpeeds(uartMotor, 32, -32);
                        sleep(2);
                        PololuQik_setSpeeds(uartMotor, -16, -16);
                        sleep(1);
                        break;
                    case 3:
                        PololuQik_setSpeeds(uartMotor, 32, -32);
                        sleep(2);
                        PololuQik_setSpeeds(uartMotor, 16, 16);
                        sleep(1);
                        break;
                    default:    // irFlag == 4
//                        PololuQik_setSpeeds(uartMotor, -32, 32);
//                        sleep(2);
//                        PololuQik_setSpeeds(uartMotor, -16, -16);
//                        sleep(1);
                         PololuQik_setSpeeds(uartMotor, -64, 64);
                         sleep(1);
                         PololuQik_setSpeeds(uartMotor, -64, -64);
                         sleep(1);
                        break;
                }
                PololuQik_setSpeeds(uartMotor, 0, 0);
                count0 = count0Copy;
                count1 = count1Copy;
                irFlag = 0;
                sem_post(&semMoveMotors);   // Signal XBee to continue
            }

            error = count1Copy - count0Copy;
            // if (error > 100) {
            //     error = 100;
            // } else if (error < -100) {
            //     error = -100;
            // }
            // error /= 50;
            if (error > 0) {
                // motor0Speed -= error;
                if (forwards) {
                    motor0Speed = 64 + error/3;
                    motor1Speed = 64;
                    if (motor0Speed > 90) {
                        motor0Speed = 90;
                    }
                } else {
                    motor0Speed = 64;
                    motor1Speed = 64 + error/3;
                    if (motor1Speed > 90) {
                        motor1Speed = 90;
                    }
                }

                // } else if (motor0Speed < 43) {
                //     motor0Speed = 43;
                // }
            } else {
                // motor1Speed += error;
                if (forwards) {
                    motor0Speed = 64;
                    motor1Speed = 64 - error/3;
                    if (motor1Speed > 90) {
                        motor1Speed = 90;
                    }
                } else {
                    motor0Speed = 64 - error/3;
                    motor1Speed = 64;
                    if (motor0Speed > 90) {
                        motor0Speed = 90;
                    }
                }
                // if (motor1Speed > 53) {
                //     motor1Speed = 53;
                // } else if (motor1Speed < 43) {
                //     motor1Speed = 43;
                // }
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

           motor0Speed += 32;
           motor1Speed += 32;
            
            if (forwards) {
                PololuQik_setSpeeds(uartMotor, motor0Speed, -motor1Speed);
            } else {
                PololuQik_setSpeeds(uartMotor, -motor0Speed, motor1Speed);
            }

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

        Display_printf(display, 0, 0, "Final motors count = %d %d", count1, count0);
        // encoderCount++;
        // Display_printf(display, 0, 0, "time = %d s", encoderCount);
        // sleep(1);

        if (forwards) {
            forwards = false;
        } else {
            forwards = true;
        }
    }

    return (NULL);
}
