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

/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)

const char dataArray[] = \
"{\n"
"-8394,-2085,-262523,\n"
"-8187,-2118,-262415,\n"
"-8035,-2419,-262642,\n"
"-7733,-2506,-262989,\n"
"-7283,-2572,-262680,\n"
"-7048,-2705,-261708,\n"
"-7070,-2803,-261911,\n"
"-7186,-2751,-262200,\n"
"-7326,-2566,-261840,\n"
"-7346,-2692,-261673,\n"
"-7601,-2540,-262667,\n"
"-7762,-2345,-262377,\n"
"-8221,-2100,-262285,\n"
"-8431,-1954,-262605,\n"
"-8506,-1860,-262830,\n"
"-8492,-1934,-262253,\n"
"-8325,-2097,-261943,\n"
"-8110,-2192,-262124,\n"
"-7907,-2593,-262026,\n"
"-7423,-2820,-262176,\n"
"-7132,-2755,-262143,\n"
"-7062,-2804,-262166,\n"
"-6762,-2873,-261690,\n"
"-6841,-2835,-261660,\n"
"-7038,-2803,-262091,\n"
"-7611,-2547,-262526,\n"
"-7595,-2427,-262482,\n"
"-8077,-2180,-262302,\n"
"-8141,-1992,-261718,\n"
"-8248,-2439,-262563,\n"
"-8350,-1879,-262185,\n"
"-8312,-2020,-262968,\n"
"-8091,-2211,-262843,\n"
"-7996,-2201,-262014,\n"
"-7625,-2288,-261663,\n"
"-7294,-2477,-262009,\n"
"-7150,-2609,-262539,\n"
"-7121,-2947,-262826,\n"
"-7132,-2885,-262678,\n"
"-7121,-2603,-261786,\n"
"}\n";

/* Console display strings */
const char textarray[] = \
"************************************************************************\r\n"
"Sample text to test UART reliability abcdefghijklmnopqrstuvwxyz123456789\r\n"
"So then it's up with the White and Gold\r\n"
"Down with the Red and Black\r\n"
"Georgia Tech is out for a victory\r\n"
"We'll drop the battle axe on georgia's head, CHOP!\r\n"
"When we meet her our team will surely beat her\r\n"
"Down on the old farm there'll be no sound\r\n"
"When our bow wows rip through the air\r\n"
"When the battle is over georgia's team will be found\r\n"
"With the Yellow Jackets swarming around!\r\n"
"************************************************************************\r\n";
const char consoleDisplay[]   = "\fConsole (h for help)\r\n";
const char helpPrompt[]       = "Valid Commands\r\n"                  \
                                "--------------\r\n"                  \
                                "h: help\r\n"                         \
                                "c: clear the screen\r\n"             \
                                "r: record accelerometer data\r\n"    \
                                "m: move "STR(MOVE_DIST)" ticks\r\n"  \
                                "y: print sample text from robot\r\n"
                                "d: run demo\r\n";
const char cleanDisplay[]     = "\f";
const char userPrompt[]       = "> ";
const char readErrDisplay[]   = "Problem read UART.\r\n";
const char spaceDisplay[] = " ";
const char newLineDisplay[] = "\r\n";

const char recordAccelDisplay[] = "Reading the accelerometer for 5 seconds...\r\n";
const char recordAccelDoneDisplay[] = "Accelerometer reading complete\r\n";

const char moveMotorsDisplay[] = "Encoder counts for Motor 1 and 0: ";

// const char moveMotorsDoneDisplay[] = "Robot has moved 5000 ticks. Counts = ";

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
    uartParams.baudRate = 115200;   // 9600;

    uartXbee = UART_open(Board_UART3, &uartParams);
    if (uartXbee == NULL) while (1);    // Failed to open XBee UART

    Display_printf(display, 0, 0, "XBee UART opened");

    UART_write(uartXbee, consoleDisplay, sizeof(consoleDisplay));

    retc = sem_init(&semPrintMotorEncoders, 0, 0);
    if (retc == -1) while (1);  // sem_init() failed

    /* Loop until read fails or user quits */

    const char demoCommands[] = "mrm";
    bool runDemo = true;
    int idx;
    while (1) {
        if (runDemo) {
            cmd = demoCommands[idx++];
            if (idx == strlen(demoCommands)) {
                runDemo = false;
            }
            sleep(2);
        } else {
            UART_write(uartXbee, userPrompt, sizeof(userPrompt));
            retc = UART_read(uartXbee, &cmd, sizeof(cmd));
            if (retc == 0) {
                UART_write(uartXbee, readErrDisplay, sizeof(readErrDisplay));
                cmd = 'q';
            }
        }

        switch (cmd) {
            case 'd':
                runDemo = true;
                idx = 0;
                sprintf(tempStr, "Starting demo...\r\n");
                UART_write(uartXbee, tempStr, strlen(tempStr));
                break;
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
                    if (irFlag) {
                        Timer_stop(timer1);

                        sprintf(tempStr, "Edge detected on IR sensor %i, correcting...\r\n", irFlag);
                        UART_write(uartXbee, tempStr, strlen(tempStr));
                        
                        sem_wait(&semMoveMotors);   // Wait for robot to correct itself

                        sprintf(tempStr, "Robot has corrected itself, continuing movement...\r\n");
                        UART_write(uartXbee, tempStr, strlen(tempStr));

                        Timer_start(timer1);
                    }

                     itoa(count0Copy, count0Str);
                     itoa(count1Copy, count1Str);
                     itoa(motor0Speed, motor0Str);
                     itoa(motor1Speed, motor1Str);

//                    sprintf(tempStr, "count0 = %llu, count1 = %llu\r\n", count0Copy, count1Copy);
//                    UART_write(uartXbee, tempStr, strlen(tempStr));

                     UART_write(uartXbee, moveMotorsDisplay, sizeof(moveMotorsDisplay));
                     UART_write(uartXbee, count1Str, strlen(count1Str));
                     UART_write(uartXbee, spaceDisplay, sizeof(spaceDisplay));
                     UART_write(uartXbee, count0Str, strlen(count0Str));

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

                sprintf(tempStr, "Robot has moved %d ticks. Counts = \r\n", MOVE_DIST);
                UART_write(uartXbee, tempStr, strlen(tempStr));
                // UART_write(uartXbee, moveMotorsDoneDisplay, sizeof(moveMotorsDoneDisplay));
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
