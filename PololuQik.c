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

// Pololu Qik library modified by Bridge Inspection Robot Team

#include "PololuQik.h"

//UART_Handle uart;
char cmd[5]; // serial command buffer

void PololuQik_init(UART_Handle uart) {
    cmd[0] = 0xAA;
    UART_write(uart, cmd, 1);
}

/*
void PololuQik_init(unsigned int resetPin_index) {
    GPIO_write(resetPin_index, 0);
    for(i=10000; i>0; i--); // Delay

    GPIO_write(GPIO_index, 1);
    uart = UART_open(Board_UART1, &uartParams); // speed should be set in uartParams
    UART_write(uart, 0xAA, 1);
}
*/

char PololuQik_getFirmwareVersion(UART_Handle uart) {
    char c;
    cmd[0] = QIK_GET_FIRMWARE_VERSION;
    UART_write(uart, cmd, 1);
    UART_read(uart, &c, 1);
    return c;
}

unsigned char PololuQik_getErrors(UART_Handle uart) {
    unsigned char c;
    cmd[0] = QIK_GET_ERROR_BYTE;
    UART_write(uart, cmd, 1);
    UART_read(uart, &c, 1);
    return c;
}

unsigned char PololuQik_getConfigurationParameter(UART_Handle uart, unsigned char parameter) {
    cmd[0] = QIK_GET_CONFIGURATION_PARAMETER;
    cmd[1] = parameter;
    unsigned char c;
    UART_write(uart, cmd, 2);
    UART_read(uart, &c, 1);
    return c;
}

unsigned char PololuQik_setConfigurationParameter(UART_Handle uart, unsigned char parameter, unsigned char value) {
    cmd[0] = QIK_SET_CONFIGURATION_PARAMETER;
    cmd[1] = parameter;
    cmd[2] = value;
    cmd[3] = 0x55;
    cmd[4] = 0x2A;
    unsigned char c;
    UART_write(uart, cmd, 5);
    UART_read(uart, &c, 1);
    return c;
}

void PololuQik_setM0Speed(UART_Handle uart, int speed) {
    int reverse = 0;
    if (speed < 0) {
        speed = -speed; // make speed a positive quantity
        reverse = 1; // preserve the direction
    }
    if (speed > 255) {
        speed = 255;
    }

    if (speed > 127) {
        // 8-bit mode: actual speed is (speed + 128)
        cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE_8_BIT : QIK_MOTOR_M0_FORWARD_8_BIT;
        cmd[1] = speed - 128;
    } else {
        cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE : QIK_MOTOR_M0_FORWARD;
        cmd[1] = speed;
    }

    UART_write(uart, cmd, 2);
}

void PololuQik_setM1Speed(UART_Handle uart, int speed) {
    int reverse = 0;

    if (speed < 0) {
        speed = -speed; // make speed a positive quantity
        reverse = 1; // preserve the direction
    }

    if (speed > 255) {
        speed = 255;
    }

    if (speed > 127) {
        // 8-bit mode: actual speed is (speed + 128)
        cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE_8_BIT : QIK_MOTOR_M1_FORWARD_8_BIT;
        cmd[1] = speed - 128;
    } else {
        cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE : QIK_MOTOR_M1_FORWARD;
        cmd[1] = speed;
    }

    UART_write(uart, cmd, 2);
}

void PololuQik_setSpeeds(UART_Handle uart, int m0Speed, int m1Speed) {
    PololuQik_setM0Speed(uart, m0Speed);
    PololuQik_setM1Speed(uart, m1Speed);
}

// 2s9v1

void PololuQik_setM0Coast(UART_Handle uart) {
    cmd[0] = QIK_2S9V1_MOTOR_M0_COAST;
    UART_write(uart, cmd, 1);
}

void PololuQik_setM1Coast(UART_Handle uart) {
    cmd[0] = QIK_2S9V1_MOTOR_M1_COAST;
    UART_write(uart, cmd, 1);}

void PololuQik_setCoasts(UART_Handle uart) {
  PololuQik_setM0Coast(uart);
  PololuQik_setM1Coast(uart);
}

/*
PololuQik::PololuQik(unsigned char receivePin, unsigned char transmitPin, unsigned char resetPin) : SoftwareSerial(receivePin, transmitPin)
{
  _resetPin = resetPin;
}

void PololuQik::init(long speed)   // default value of speed is 9600
{
  // reset the qik
  digitalWrite(_resetPin, LOW);
  pinMode(_resetPin, OUTPUT); // drive low
  delay(1);
  pinMode(_resetPin, INPUT); // return to high-impedance input (reset is internally pulled up on qik)
  delay(10);

  begin(speed);
  write(0xAA); // allow qik to autodetect baud rate
}

char PololuQik::getFirmwareVersion()
{
  listen();
  write(QIK_GET_FIRMWARE_VERSION);
  while (available() < 1);
  return read();
}

byte PololuQik::getErrors()
{
  listen();
  write(QIK_GET_ERROR_BYTE);
  while (available() < 1);
  return read();
}

byte PololuQik::getConfigurationParameter(byte parameter)
{
  listen();
  cmd[0] = QIK_GET_CONFIGURATION_PARAMETER;
  cmd[1] = parameter;
  write(cmd, 2);
  while (available() < 1);
  return read();
}

byte PololuQik::setConfigurationParameter(byte parameter, byte value)
{
  listen();
  cmd[0] = QIK_SET_CONFIGURATION_PARAMETER;
  cmd[1] = parameter;
  cmd[2] = value;
  cmd[3] = 0x55;
  cmd[4] = 0x2A;
  write(cmd, 5);
  while (available() < 1);
  return read();
}

void PololuQik::setM0Speed(int speed)
{
  boolean reverse = 0;

  if (speed < 0)
  {
    speed = -speed; // make speed a positive quantity
    reverse = 1; // preserve the direction
  }

  if (speed > 255)
    speed = 255;

  if (speed > 127)
  {
    // 8-bit mode: actual speed is (speed + 128)
    cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE_8_BIT : QIK_MOTOR_M0_FORWARD_8_BIT;
    cmd[1] = speed - 128;
  }
  else
  {
    cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE : QIK_MOTOR_M0_FORWARD;
    cmd[1] = speed;
  }

  write(cmd, 2);
}

void PololuQik::setM1Speed(int speed)
{
  boolean reverse = 0;

  if (speed < 0)
  {
    speed = -speed; // make speed a positive quantity
    reverse = 1; // preserve the direction
  }

  if (speed > 255)
    speed = 255;

  if (speed > 127)
  {
    // 8-bit mode: actual speed is (speed + 128)
    cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE_8_BIT : QIK_MOTOR_M1_FORWARD_8_BIT;
    cmd[1] = speed - 128;
  }
  else
  {
    cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE : QIK_MOTOR_M1_FORWARD;
    cmd[1] = speed;
  }

  write(cmd, 2);
}

void PololuQik::setSpeeds(int m0Speed, int m1Speed)
{
  setM0Speed(m0Speed);
  setM1Speed(m1Speed);
}

// 2s9v1

void PololuQik2s9v1::setM0Coast()
{
  write(QIK_2S9V1_MOTOR_M0_COAST);
}

void PololuQik2s9v1::setM1Coast()
{
  write(QIK_2S9V1_MOTOR_M1_COAST);
}

void PololuQik2s9v1::setCoasts()
{
  setM0Coast();
  setM1Coast();
}
*/
