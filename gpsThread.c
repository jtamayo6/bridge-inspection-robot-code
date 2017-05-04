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
 *  ======== gpsThread.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
//#include <ti/drivers/GPIO.h>
//#include <ti/drivers/UART.h>

// #include <ti/display/Display.h>

/* Example/Board Header files */
#include "Board.h"

#include "Adafruit_GPS.h"

/*
 *  ======== gpsThread ========
 */
void *gpsThread(void *arg0)
{
    // char        input;
    // const char  echoPrompt[] = "Echoing characters:\r\n";
    UART_Handle uart;
    UART_Params uartParams;


    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    uart = UART_open(Board_UART2, &uartParams);
    if (uart == NULL) while (1);    // Failed to open GPS UART

    Adafruit_GPS_parseSetup(uart);
    struct gps GPS;
    while(1) {
         Adafruit_GPS_parseOutput(uart, &GPS);
         // Display_printf(display, 0, 0, "Latitude = %f Longitude = %f", GPS.lat, GPS.lon);
    }
}
