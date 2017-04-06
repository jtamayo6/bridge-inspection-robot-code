/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 *  ======== fatsd.c ========
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>
#include <file.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* Driver Header files */
// #include <ti/display/Display.h>
#include <third_party/fatfs/ffcio.h>

// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/SDSPI.h>

/* Example/Board Header files */
#include "Board.h"

/* Buffer size used for the file copy process */
#ifndef CPY_BUFF_SIZE
#define CPY_BUFF_SIZE       2048
#endif

/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)

/* Drive number used for FatFs */
#define DRIVE_NUM           0

// Array size for accelerometer data buffers
// 1536 = 3*512, since we want to write in 512 byte sectors to the SD card
#define ACCEL_DATA_BUF_COUNT     1536

const char inputfile[] = "fat:"STR(DRIVE_NUM)":input.txt";
// const char outputfile[] = "fat:"STR(DRIVE_NUM)":output.txt";
const char outputfile[] = "fat:"STR(DRIVE_NUM)":dataOut.txt";

const char textarray[] = \
"***********************************************************************\n"
"0         1         2         3         4         5         6         7\n"
"01234567890123456789012345678901234567890123456789012345678901234567890\n"
"This is some text to be inserted into the inputfile if there isn't\n"
"already an existing file located on the media.\n"
"If an inputfile already exists, or if the file was already once\n"
"generated, then the inputfile will NOT be modified.\n"
"***********************************************************************\n";

extern Display_Handle display;

/* Set this to the current UNIX time in seconds */
const struct timespec ts =
{
    .tv_sec = 1469647026,
    .tv_nsec = 0
};

/* File name prefix for this filesystem for use with TI C RTS */
char fatfsPrefix[] = "fat";

unsigned char cpy_buff[CPY_BUFF_SIZE + 1];

extern int32_t accelDataBuffer0[ACCEL_DATA_BUF_COUNT];
extern int32_t accelDataBuffer1[ACCEL_DATA_BUF_COUNT];

extern pthread_barrier_t barrier;
extern sem_t semAccelData;

/*
 *  ======== mainThread ========
 *  Thread to perform a file copy
 *
 *  Thread tries to open an existing file inputfile[]. If the file doesn't
 *  exist, create one and write some known content into it.
 *  The contents of the inputfile[] are then copied to an output file
 *  outputfile[]. Once completed, the contents of the output file are
 *  printed onto the system console (stdout).
 */
void *sdCardThread(void *arg0)
{
    // memset(accelDataBuffer0, 'a', 4*ACCEL_DATA_BUF_COUNT);
    // memset(accelDataBuffer1, 'A', 4*ACCEL_DATA_BUF_COUNT);

    SDSPI_Handle sdspiHandle;
    SDSPI_Params sdspiParams;

    /* Variables for the CIO functions */
    FILE *src, *dst;

    /* Variables to keep track of the file copy progress */
    unsigned int bytesRead = 0;
    unsigned int bytesWritten = 0;
    unsigned int filesize;
    unsigned int totalBytesCopied = 0;

    /* Call driver init functions */
    // GPIO_init();
    // Display_init();
    // SDSPI_init();

    /* add_device() should be called once and is used for all media types */
    add_device(fatfsPrefix, _MSA, ffcio_open, ffcio_close, ffcio_read,
        ffcio_write, ffcio_lseek, ffcio_unlink, ffcio_rename);

    /* Open the display for output */
    // display = Display_open(Display_Type_UART, NULL);
    // if (display == NULL) {
    //     /* Failed to open display driver */
    //     while (1);
    // }

    /* Initialize real-time clock */
    clock_settime(CLOCK_REALTIME, &ts);

    /* Turn on user LED */
    // GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);

    Display_printf(display, 0, 0, "Starting the fatsd example\n");

    /* Mount and register the SD Card */
    SDSPI_Params_init(&sdspiParams);
    sdspiHandle = SDSPI_open(Board_SDSPI0, DRIVE_NUM, &sdspiParams);
    if (sdspiHandle == NULL) {
        Display_printf(display, 0, 0, "Error starting the SD card\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "Drive %u is mounted\n", DRIVE_NUM);
    }

    /* Try to open the source file */
    // src = fopen(inputfile, "r");
    // if (!src) {
    //     Display_printf(display, 0, 0, "Creating a new file \"%s\"...", inputfile);

    //     /* Open file for both reading and writing */
    //     src = fopen(inputfile, "w+");
    //     if (!src) {
    //         Display_printf(display, 0, 0, "Error: \"%s\" could not be created.\n"
    //                       "Please check the Getting Started Guide "
    //                       "if additional jumpers are necessary.\n",
    //                       inputfile);
    //         Display_printf(display, 0, 0, "Aborting...\n");
    //         while (1);
    //     }

    //     fwrite(textarray, 1, strlen(textarray), src);
    //     fflush(src);

    //     /* Reset the internal file pointer */
    //     rewind(src);

    //     Display_printf(display, 0, 0, "done\n");
    // }
    // else {
    //     Display_printf(display, 0, 0, "Using existing copy of \"%s\"\n", inputfile);
    // }

    /* Create a new file object for the file copy */
    dst = fopen(outputfile, "w");
    if (!dst) {
        Display_printf(display, 0, 0, "Error opening \"%s\"\n", outputfile);
        Display_printf(display, 0, 0, "Aborting...\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "Starting file copy\n");
    }

    // /*  Copy the contents from the src to the dst */
    // while (true) {
    //     /*  Read from source file */
    //     bytesRead = fread(cpy_buff, 1, CPY_BUFF_SIZE, src);
    //     if (bytesRead == 0) {
    //         break; /* Error or EOF */
    //     }

    //     /*  Write to dst file */
    //     bytesWritten = fwrite(cpy_buff, 1, bytesRead, dst);
    //     if (bytesWritten < bytesRead) {
    //         Display_printf(display, 0, 0, "Disk Full\n");
    //         break; /* Error or Disk Full */
    //     }

    //     /*  Update the total number of bytes copied */
    //     totalBytesCopied += bytesWritten;
    // }


    int i, retc;

    pthread_barrier_wait(&barrier);

    for (i=0; i<10; i++) {
        // Barrier until data is ready
        retc = sem_wait(&semAccelData);
        if (retc == -1) while (1);

//         Display_printf(display, 0, 0, "Writing from buffer %d", (i%2));

        /*  Write to dst file */
        if (!(i%2)) {   /* signal indicates which buffer should be read */
            bytesWritten = fwrite(accelDataBuffer0, sizeof *accelDataBuffer0, ACCEL_DATA_BUF_COUNT, dst);
//            Display_printf(display, 0, 0, "%d,%d,%d", accelDataBuffer0[0], accelDataBuffer0[1], accelDataBuffer0[2]);
        } else {
            bytesWritten = fwrite(accelDataBuffer1, sizeof *accelDataBuffer1, ACCEL_DATA_BUF_COUNT, dst);
        }
//        bytesWritten = fwrite(textarray, 1, sizeof(textarray), dst);
        // Display_printf(display, 0, 0, "bytesWritten = %d", bytesWritten);
        if (bytesWritten < ACCEL_DATA_BUF_COUNT) {
            Display_printf(display, 0, 0, "Disk Full");
            break; /* Error or Disk Full */
        }

        fflush(dst);

        /*  Update the total number of bytes copied */
        totalBytesCopied += bytesWritten;
        // Display_printf(display, 0, 0, "Writing from buffer %d complete", (i%2));
    }

    // fflush(dst);

    /* Get the filesize of the source file */
    // fseek(src, 0, SEEK_END);
    // filesize = ftell(src);
    // rewind(src);

    /* Close both inputfile[] and outputfile[] */
    // fclose(src);
    fclose(dst);

    // Display_printf(display, 0, 0, "File \"%s\" (%u B) copied to \"%s\" (Wrote %u B)\n",
    //               inputfile, filesize, outputfile, totalBytesCopied);
    Display_printf(display, 0, 0, "(Wrote %u B) to \"%s\" \n", totalBytesCopied, outputfile);

    /* Now output the outputfile[] contents onto the console */
    // dst = fopen(outputfile, "r");
    // if (!dst) {
    //     Display_printf(display, 0, 0, "Error opening \"%s\"\n", outputfile);
    //     Display_printf(display, 0, 0, "Aborting...\n");
    //     while (1);
    // }

    /* Print file contents */
    // while (true) {
    //     /* Read from output file */
    //     bytesRead = fread(cpy_buff, 1, CPY_BUFF_SIZE, dst);
    //     if (bytesRead == 0) {
    //         break; /* Error or EOF */
    //     }
    //     cpy_buff[bytesRead] = '\0';
    //     /* Write output */
    //     Display_printf(display, 0, 0, "%s", cpy_buff);
    // }

    /* Close the file */
    // fclose(dst);

    pthread_barrier_wait(&barrier);

    /* Stopping the SDCard */
    SDSPI_close(sdspiHandle);
    Display_printf(display, 0, 0, "Drive %u unmounted\n", DRIVE_NUM);

    return (NULL);
}

/*
 *  ======== fatfs_getFatTime ========
 */
int32_t fatfs_getFatTime(void)
{
    time_t seconds;
    uint32_t fatTime;
    struct tm *pTime;

    /*
     *  TI time() returns seconds elapsed since 1900, while other tools
     *  return seconds from 1970.  However, both TI and GNU localtime()
     *  sets tm tm_year to number of years since 1900.
     */
    seconds = time(NULL);

    pTime = localtime(&seconds);

    /*
     *  localtime() sets pTime->tm_year to number of years
     *  since 1900, so subtract 80 from tm_year to get FAT time
     *  offset from 1980.
     */
    fatTime = ((uint32_t)(pTime->tm_year - 80) << 25) |
        ((uint32_t)(pTime->tm_mon) << 21) |
        ((uint32_t)(pTime->tm_mday) << 16) |
        ((uint32_t)(pTime->tm_hour) << 11) |
        ((uint32_t)(pTime->tm_min) << 5) |
        ((uint32_t)(pTime->tm_sec) >> 1);

    return ((int32_t)fatTime);
}
