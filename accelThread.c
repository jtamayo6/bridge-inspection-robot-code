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
 *  ======== accelThread.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

/* POSIX Header files */
// #include <pthread.h>
// #include <semaphore.h>

/* Driver Header files */
// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/SPI.h>
// #include <ti/display/Display.h>

// #include <ti/drivers/Timer.h>

/* Example/Board Header files */
#include "Board.h"

#define THREADSTACKSIZE 768

// Buffer size for accelerometer SPI buffers
#define ACCEL_SPI_BUF_COUNT 10

// Array size for accelerometer data buffers
#define ACCEL_DATA_BUF_COUNT     1536    // 1536 = 3*512, since we want to write in 512 byte sectors to the SD card

// Watermark for reading n samples from accelerometer at a time
#define ACCEL_WATERMARK_SAMPLES 24

// extern pthread_barrier_t accelDataBarrier;
// extern sem_t semAccelData;

// extern Display_Handle display;

// // Global so that sdCardThread can access
int32_t accelDataBuffer0[ACCEL_DATA_BUF_COUNT];
int32_t accelDataBuffer1[ACCEL_DATA_BUF_COUNT];

uint32_t errorCount = 0;

int time_ms = 0;

void timerCallback(Timer_Handle myHandle)
{
    time_ms++;
}

/*
 *  ======== accelThread ========
 */
void *accelThread(void *arg0)
{
    int i, j, bufNum; // i, j, and bufNum are for accel data capture
    int retc;   // retc is for semaphore return value
    uint8_t accelRxBuffer[ACCEL_SPI_BUF_COUNT] = {0};
    uint8_t accelTxBuffer[ACCEL_SPI_BUF_COUNT] = {0};
//    float xg, yg, zg;


    /* Initialize SPI for accelerometer */

    SPI_Handle masterSpi;
    SPI_Transaction masterTransaction;
    bool transferOK;

    // SPI_Params spiParams;
    // SPI_Params_init(&spiParams);
    // spiParams.bitRate = 5000000;    // 5 MHz

    masterSpi = SPI_open(Board_SPI2, NULL /*&spiParams*/);
    if (masterSpi == NULL) {
        Display_printf(display, 0, 0, "Error initializing SPI for accelerometer\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "SPI for accelerometer initialized\n");
    }


    /* Initialize timer for accelerometer */

    Timer_Handle timer0;
    Timer_Params timerParams;

    Timer_Params_init(&timerParams);
    timerParams.period = 1000;    // 1000 us = 1 ms
    timerParams.periodUnits = Timer_PERIOD_US;
    timerParams.timerMode = Timer_CONTINUOUS_CALLBACK;   // Timer_FREE_RUNNING;
    timerParams.timerCallback = timerCallback;

    timer0 = Timer_open(Board_TIMER0, &timerParams);
    if (timer0 == NULL) while (1);   // Failed to initialized timer


    /* Configure accelerometer */

    // Initialize accelerometer SPI transaction structure
    masterTransaction.count = 2;
    masterTransaction.txBuf = (void *)accelTxBuffer;
    masterTransaction.rxBuf = (void *)accelRxBuffer;

    GPIO_write(4, 1);   // Set Accel_SS high to prepare for SPI transfers

    // Set ODR
    accelTxBuffer[0] = (0x28 << 1);
    accelTxBuffer[1] = 0x02;
    GPIO_write(4, 0);
    transferOK = SPI_transfer(masterSpi, &masterTransaction);
    GPIO_write(4, 1);

    // Set FIFO SAMPLES
    accelTxBuffer[0] = (0x29 << 1);
    accelTxBuffer[1] = ACCEL_WATERMARK_SAMPLES;
    GPIO_write(4, 0);
    transferOK = SPI_transfer(masterSpi, &masterTransaction);
    GPIO_write(4, 1);

    // Set INT1 to FIFO_FULL and INT2 to FIFO_OVR
    accelTxBuffer[0] = (0x2A << 1);
    accelTxBuffer[1] = 0x42;
    GPIO_write(4, 0);
    transferOK = SPI_transfer(masterSpi, &masterTransaction);
    GPIO_write(4, 1);

    // Set INTs to active HIGH
    accelTxBuffer[0] = (0x2C << 1);
    accelTxBuffer[1] = 0x40;
    GPIO_write(4, 0);
    transferOK = SPI_transfer(masterSpi, &masterTransaction);
    GPIO_write(4, 1);

    // Set calibrated offsets
    // masterTransaction.count = 7;
    // accelTxBuffer[0] = (0x1E << 1);
    // accelTxBuffer[1] = 0xFE;
    // accelTxBuffer[2] = 0x1D;
    // accelTxBuffer[3] = 0xFF;
    // accelTxBuffer[4] = 0x69;
    // accelTxBuffer[5] = 0xBF;
    // accelTxBuffer[6] = 0xFC;
    // GPIO_write(4, 0);
    // transferOK = SPI_transfer(masterSpi, &masterTransaction);
    // GPIO_write(4, 1);

    // Clear accelTxBuffer
    for (i=0; i<7; i++) {
        accelTxBuffer[i] = 0x00;
    }

    uint8_t fifoOverTxBuffer[2] = {((0x04 << 1) | 1), 0x00};
    uint8_t fifoOverRxBuffer[2] = {0};
    SPI_Transaction fifoOverTransaction;
    fifoOverTransaction.count = 2;
    fifoOverTransaction.txBuf = (void *)fifoOverTxBuffer;
    fifoOverTransaction.rxBuf = (void *)fifoOverRxBuffer;

    PWM_Handle pwm;
    PWM_Params pwmParams;
    PWM_Params_init(&pwmParams);

    pwmParams.dutyUnits = PWM_DUTY_US;
    pwmParams.dutyValue = 1000;
    pwmParams.periodUnits = PWM_PERIOD_US;
    pwmParams.periodValue = 20000;
    pwm = PWM_open(Board_PWM0, &pwmParams);
    if (pwm == NULL) while (1);    // Failed to open linear actuator

    PWM_start(pwm);
    PWM_setDuty(pwm, 1000);

    while (1) {

        // retc = sem_wait(&semAccelData);
        pthread_barrier_wait(&accelDataBarrier); // Barrier until signal is received from XBee

        PWM_setDuty(pwm, 1650);

        sleep(6);

        pthread_barrier_wait(&accelDataBarrier); // Barrier until XBee and SD card is ready

        // Set accel to measurement mode
        accelTxBuffer[0] = (0x2D << 1);
        accelTxBuffer[1] = 0x02;
        GPIO_write(4, 0);
        transferOK = SPI_transfer(masterSpi, &masterTransaction);
        GPIO_write(4, 1);

        if (GPIO_read(7)) {
            GPIO_write(4, 0);
            transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
            GPIO_write(4, 1);
            // errorCount++;
        }

        errorCount = 0; // reset errorCount
        time_ms = 0;
        if (Timer_start(timer0) == Timer_STATUS_ERROR) while (1); // Failed to start timer

        masterTransaction.count = 10;
        accelTxBuffer[0] = (0x11 << 1) | 1;
        accelTxBuffer[1] = 0x00;

        /* Accelerometer data capture */

        for (bufNum=0; bufNum<10; bufNum++) {
            if (!(bufNum%2)) {
                for (i=0; i<ACCEL_DATA_BUF_COUNT; i+=ACCEL_WATERMARK_SAMPLES) {
                    if (GPIO_read(7)) {
                        GPIO_write(4, 0);
                        transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
                        GPIO_write(4, 1);
                        errorCount++;
                    }
                    while (!GPIO_read(6));
                    for (j=i; j<i+ACCEL_WATERMARK_SAMPLES; j+=3) {
                        GPIO_write(4, 0);
                        transferOK = SPI_transfer(masterSpi, &masterTransaction);
                        GPIO_write(4, 1);
                        // if (!(accelRxBuffer[3] & 0x01)) {
                        //     // GPIO_toggle(Board_GPIO_LED0);
                        //     errorCount++;
                        // }
                        accelDataBuffer0[j] = ((accelRxBuffer[1] << 24)
                                             | (accelRxBuffer[2] << 16)
                                             | (accelRxBuffer[3] << 8)) >> 12;
                        accelDataBuffer0[j+1] = ((accelRxBuffer[4] << 24)
                                               | (accelRxBuffer[5] << 16)
                                               | (accelRxBuffer[6] << 8)) >> 12;
                        accelDataBuffer0[j+2] = ((accelRxBuffer[7] << 24)
                                               | (accelRxBuffer[8] << 16)
                                               | (accelRxBuffer[9] << 8)) >> 12;
                    }
                    if (bufNum != 0 && !(i%96)) {
                        // sched_yield();
                        sem_post(&semAccelData);
                        sem_wait(&semAccelData);
                    }

                    // GPIO_write(4, 0);
                    // transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
                    // GPIO_write(4, 1);
                }
            } else {
                // if (GPIO_read(7)) {
                //     GPIO_write(4, 0);
                //     transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
                //     GPIO_write(4, 1);
                //     errorCount++;
                //     GPIO_toggle(Board_GPIO_LED0);
                // }
                for (i=0; i<ACCEL_DATA_BUF_COUNT; i+=ACCEL_WATERMARK_SAMPLES) {
                    if (GPIO_read(7)) {
                        GPIO_write(4, 0);
                        transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
                        GPIO_write(4, 1);
                        errorCount++;
                    }
                    while (!GPIO_read(6));
                    for (j=i; j<i+ACCEL_WATERMARK_SAMPLES; j+=3) {
                        GPIO_write(4, 0);
                        transferOK = SPI_transfer(masterSpi, &masterTransaction);
                        GPIO_write(4, 1);
                        // if (!(accelRxBuffer[3] & 0x01)) {
                        //     // GPIO_toggle(Board_GPIO_LED0);
                        //     errorCount++;
                        // }
                        accelDataBuffer1[j] = ((accelRxBuffer[1] << 24)
                                             | (accelRxBuffer[2] << 16)
                                             | (accelRxBuffer[3] << 8)) >> 12;
                        accelDataBuffer1[j+1] = ((accelRxBuffer[4] << 24)
                                               | (accelRxBuffer[5] << 16)
                                               | (accelRxBuffer[6] << 8)) >> 12;
                        accelDataBuffer1[j+2] = ((accelRxBuffer[7] << 24)
                                               | (accelRxBuffer[8] << 16)
                                               | (accelRxBuffer[9] << 8)) >> 12;
                    }
                    if (!(i%96)) {
                        // sched_yield();
                        sem_post(&semAccelData);
                        sem_wait(&semAccelData);
                    }

    //                 GPIO_write(4, 0);
    //                 transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
    //                 GPIO_write(4, 1);
    //                 if (GPIO_read(7)) {
    //                     GPIO_write(4, 0);
    //                     transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
    //                     GPIO_write(4, 1);
    //                     errorCount++;
    //                     GPIO_toggle(Board_GPIO_LED0);
    //                 }
                }
    //             for (i=0; i<ACCEL_DATA_BUF_COUNT; i+=3) {
    //                 while (!GPIO_read(6));
    //                 GPIO_write(4, 0);
    //                 transferOK = SPI_transfer(masterSpi, &masterTransaction);
    //                 GPIO_write(4, 1);
    //                 accelDataBuffer1[i] = ((accelRxBuffer[1] << 24)
    //                                      | (accelRxBuffer[2] << 16)
    //                                      | (accelRxBuffer[3] << 8)) >> 12;
    //                 accelDataBuffer1[i+1] = ((accelRxBuffer[4] << 24)
    //                                        | (accelRxBuffer[5] << 16)
    //                                        | (accelRxBuffer[6] << 8)) >> 12;
    //                 accelDataBuffer1[i+2] = ((accelRxBuffer[7] << 24)
    //                                        | (accelRxBuffer[8] << 16)
    //                                        | (accelRxBuffer[9] << 8)) >> 12;
    //                 // if (!(errorCount++ % 1000)) {
    //                 //     GPIO_toggle(Board_GPIO_LED0);
    //                 // }
    //                 GPIO_write(4, 0);
    //                 transferOK = SPI_transfer(masterSpi, &fifoOverTransaction);
    //                 GPIO_write(4, 1);
    //                 if (fifoOverRxBuffer[1] & 0x04) {
    //                     // Display_printf(display, 0, 0, "data missed!");
    //                     errorCount++;
    //                 }
    //             }
            }
            Display_printf(display, 0, 0, "Buffer %d filled", (bufNum%2));
            // timervals[timerval_count++] = Timer_getCount(timer0);

            retc = sem_post(&semFullAccelBuffer);
            if (retc == -1) while (1);
        }

    /*     while(count--) {
             GPIO_write(4, 0);
             transferOK = SPI_transfer(masterSpi, &masterTransaction);
             GPIO_write(4, 1);

     //         if(transferOK) {
     //             // Print contents of master receive buffer
     //             Display_printf(display, 0, 0, "Successful master SPI transfer\n");
     //         }
     //         else {
     //             Display_printf(display, 0, 0, "Unsuccessful master SPI transfer");
     //         }

             xval = ((accelRxBuffer[1] << 24) | (accelRxBuffer[2] << 16) | (accelRxBuffer[3] << 8)) >> 12;
             yval = ((accelRxBuffer[4] << 24) | (accelRxBuffer[5] << 16) | (accelRxBuffer[6] << 8)) >> 12;
             zval = ((accelRxBuffer[7] << 24) | (accelRxBuffer[8] << 16) | (accelRxBuffer[9] << 8)) >> 12;
     //        count++;

     //        xg = xval / 256000.0;
     //        yg = yval / 256000.0;
     //        zg = zval / 256000.0;

     //        if (!(++count % 1000)) {
     //            Display_printf(display, 0, 0, "%d", count);
     //        }

     //        Display_printf(display, 0, 0, "%d X-axis = %f g, Y-axis = %f g, Z-axis = %f g \n\r", count++, xg, yg, zg);
             // Display_printf(display, 0, 0, "%d,%d,%d", xval, yval, zval);
     //        int i;
     //        for (i=0; i<10; i++) {
     //            Display_printf(display, 0, 0, "accelRxBuffer[%d] = %d", i, accelRxBuffer[i]);
     //        }
     //        Task_sleep(10);
         } */

        // Timer_stop(timer0);
        // for (i=0; i<20; i++) {
        //     Display_printf(display, 0, 0, "%d\n", timervals[i]);
        // }

        // Set accel to standby mode
        masterTransaction.count = 2;
        accelTxBuffer[0] = (0x2D << 1);
        accelTxBuffer[1] = 0x01;
        GPIO_write(4, 0);
        transferOK = SPI_transfer(masterSpi, &masterTransaction);
        GPIO_write(4, 1);

        Display_printf(display, 0, 0, "\nErrors = %d\n", errorCount);
        Display_printf(display, 0, 0, "\nRunning time = %d ms\n", time_ms);

        PWM_setDuty(pwm, 1000);
        sleep(6);

        pthread_barrier_wait(&accelDataBarrier);
    }

    /* Deinitialize SPI */
    SPI_close(masterSpi);
    Timer_close(timer0);

    Display_printf(display, 0, 0, "Done\n");
    // Display_printf(display, 0, 0, "FIFO overflow: %d", errorCount);

    return (NULL);
}
