/***********************************
This is our GPS library

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information
All text above must be included in any redistribution
****************************************/
#include <Adafruit_GPS.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <xdc/std.h>
//#include <unistd.h>
//#include <windows.h>

// #include <xdc/runtime/System.h>
#include <stddef.h>
#include <string.h>

/* TI-RTOS Header files */
// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

// how long are max NMEA lines to parse?
#define MAXLINELENGTH 120

// CCS understands the bool data type
// typedef int boolean;
// #define true 1
// #define false 0

// we double buffer: read one line in and leave one for the main program
volatile char line1[MAXLINELENGTH];
volatile char line2[MAXLINELENGTH];
// our index into filling the current line
volatile uint8_t lineidx=0;
// pointers to the double buffers
volatile char *currentline;
volatile char *lastline;
volatile bool recvdflag;
volatile bool inStandbyMode;
uint8_t hour, minute, seconds, year, month, day;
uint8_t dataBuf[5];
float floatBuf[5];
uint16_t milliseconds;
// Floating point latitude and longitude value in degrees.
float latitude, longitude;
// Fixed point latitude and longitude value with degrees stored in units of 1/100000 degrees,
// and minutes stored in units of 1/100000 degrees.  See pull #13 for more details:
//   https://github.com/adafruit/Adafruit-GPS-Library/pull/13
int32_t latitude_fixed, longitude_fixed;
float latitudeDegrees, longitudeDegrees;
float geoidheight, altitude;
float speed, angle, magvariation, HDOP;
char lat, lon, mag;
bool fix;
bool paused;
uint8_t fixquality, satellites;
char cmd[50];
bool GPSECHO = false;

  // read a Hex value and return the decimal equivalent
  uint8_t Adafruit_GPS_parseHex(char c) {
      if (c < '0')
        return 0;
      if (c <= '9')
        return c - '0';
      if (c < 'A')
         return 0;
      if (c <= 'F')
         return (c - 'A')+10;
      // if (c > 'F')
      return 0;
  }


bool Adafruit_GPS_parse(char *nmea) {
  // do checksum check

  // first look if we even have one
  if (nmea[strlen(nmea)-4] == '*') {
    uint16_t sum = Adafruit_GPS_parseHex(nmea[strlen(nmea)-3]) * 16;
    sum += Adafruit_GPS_parseHex(nmea[strlen(nmea)-2]);

    // check checksum
    int i;
    for (i = 2; i < (strlen(nmea)-4); i++) {
      sum ^= nmea[i];
    }
    if (sum != 0) {
      // bad checksum :(
      return false;
    }
  }
  int32_t degree;
  long minutes;
  char degreebuff[10];
  // look for a few common sentences
  if (strstr(nmea, "$GPGGA")) {
    // found GGA
    char *p = nmea;
    // get time
    p = strchr(p, ',')+1;
    float timef = atof(p);
    uint32_t time = timef;
    hour = time / 10000;
    minute = (time % 10000) / 100;
    seconds = (time % 100);

    milliseconds = fmod(timef, 1.0) * 1000;

    // parse out latitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 2);
      p += 2;
      degreebuff[2] = '\0';
      degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      minutes = 50 * atol(degreebuff) / 3;
      latitude_fixed = degree + minutes;
      latitude = degree / 100000 + minutes * 0.000006F;
      latitudeDegrees = (latitude-100*(int)(latitude/100))/60.0;
      latitudeDegrees += (int)(latitude/100);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      if (p[0] == 'S') latitudeDegrees *= -1.0;
      if (p[0] == 'N') lat = 'N';
      else if (p[0] == 'S') lat = 'S';
      else if (p[0] == ',') lat = 0;
      else return false;
    }

    // parse out longitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 3);
      p += 3;
      degreebuff[3] = '\0';
      degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      minutes = 50 * atol(degreebuff) / 3;
      longitude_fixed = degree + minutes;
      longitude = degree / 100000.0 + minutes * 0.000006F;
      longitudeDegrees = (longitude-100*(int)(longitude/100.0))/60.0;
      longitudeDegrees += (int)(longitude/100.0);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      if (p[0] == 'W') longitudeDegrees *= -1.0;
      if (p[0] == 'W') lon = 'W';
      else if (p[0] == 'E') lon = 'E';
      else if (p[0] == ',') lon = 0;
      else return false;
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      fixquality = atoi(p);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      satellites = atoi(p);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      HDOP = atof(p);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      altitude = atof(p);
    }

    p = strchr(p, ',')+1;
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      geoidheight = atof(p);
    }
    return true;
  }
  if (strstr(nmea, "$GPRMC")) {
   // found RMC
    char *p = nmea;

    // get time
    p = strchr(p, ',')+1;
    float timef = atof(p);
    uint32_t time = timef;
    hour = time / 10000;
    minute = (time % 10000) / 100;
    seconds = (time % 100);

    milliseconds = fmod(timef, 1.0) * 1000;

    p = strchr(p, ',')+1;
    // Serial.println(p);
    if (p[0] == 'A')
      fix = true;
    else if (p[0] == 'V')
      fix = false;
    else
      return false;

    // parse out latitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 2);
      p += 2;
      degreebuff[2] = '\0';
      long degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      long minutes = 50 * atol(degreebuff) / 3;
      latitude_fixed = degree + minutes;
      latitude = degree / 100000 + minutes * 0.000006F;
      latitudeDegrees = (latitude-100*(int)(latitude/100))/60.0;
      latitudeDegrees += (int)(latitude/100);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      if (p[0] == 'S') latitudeDegrees *= -1.0;
      if (p[0] == 'N') lat = 'N';
      else if (p[0] == 'S') lat = 'S';
      else if (p[0] == ',') lat = 0;
      else return false;
    }

    // parse out longitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 3);
      p += 3;
      degreebuff[3] = '\0';
      degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      minutes = 50 * atol(degreebuff) / 3;
      longitude_fixed = degree + minutes;
      longitude = degree / 100000 + minutes * 0.000006F;
      longitudeDegrees = (longitude-100*(int)(longitude/100))/60.0;
      longitudeDegrees += (int)(longitude/100);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      if (p[0] == 'W') longitudeDegrees *= -1.0;
      if (p[0] == 'W') lon = 'W';
      else if (p[0] == 'E') lon = 'E';
      else if (p[0] == ',') lon = 0;
      else return false;
    }
    // speed
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      speed = atof(p);
    }

    // angle
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      angle = atof(p);
    }

    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      uint32_t fulldate = atof(p);
      day = fulldate / 10000;
      month = (fulldate % 10000) / 100;
      year = (fulldate % 100);
    }
    // we dont parse the remaining, yet!
    return true;
  }

  return false;
}

char Adafruit_GPS_read(UART_Handle uart) {
  char c = 0;

  if (paused) return c;

//  {
//    if(!gpsHwSerial->available()) return c;
//    c = gpsHwSerial->read();
//  }

  UART_read(uart, &c, 1);

  if (c == '\n') {
    currentline[lineidx] = 0;

    if (currentline == line1) {
      currentline = line2;
      lastline = line1;
    } else {
      currentline = line1;
      lastline = line2;
    }

    //Serial.println("----");
    //Serial.println((char *)lastline);
    //Serial.println("----");
    lineidx = 0;
    recvdflag = true;
  }

  currentline[lineidx++] = c;
  if (lineidx >= MAXLINELENGTH)
    lineidx = MAXLINELENGTH-1;

  return c;
}

//#if defined(__AVR__) && defined(USE_SW_SERIAL)
//// Constructor when using SoftwareSerial or NewSoftSerial
//#if ARDUINO >= 100
//Adafruit_GPS::Adafruit_GPS(SoftwareSerial *ser)
//#else
//Adafruit_GPS::Adafruit_GPS(NewSoftSerial *ser)
//#endif
//{
//  common_init();     // Set everything to common state, then...
//  gpsSwSerial = ser; // ...override gpsSwSerial with value passed.
//}
//#endif

// Constructor when using HardwareSerial
//Adafruit_GPS_Adafruit_GPS(HardwareSerial *ser) {
//  common_init();  // Set everything to common state, then...
//  gpsHwSerial = ser; // ...override gpsHwSerial with value passed.
//}

// Initialization code used by all constructor types
void Adafruit_GPS_common_init(void) {
//#if defined(__AVR__) && defined(USE_SW_SERIAL)
//  gpsSwSerial = NULL; // Set both to NULL, then override correct
//#endif
//  gpsHwSerial = NULL; // port pointer in corresponding constructor
  recvdflag   = false;
  paused      = false;
  lineidx     = 0;
  currentline = line1;
  lastline    = line2;

  hour = minute = seconds = year = month = day =
    fixquality = satellites = 0; // uint8_t
  lat = lon = mag = 0; // char
  fix = false; // bool
  milliseconds = 0; // uint16_t
  latitude = longitude = geoidheight = altitude =
    speed = angle = magvariation = HDOP = 0.0; // float
}

void Adafruit_GPS_begin(uint32_t baud, UART_Handle uart)
{
//#if defined(__AVR__) && defined(USE_SW_SERIAL)
//  if(gpsSwSerial)
//    gpsSwSerial->begin(baud);
//  else
//#endif
//    gpsHwSerial->begin(baud);
    // System_printf("Initializing GPS\n");
    Adafruit_GPS_common_init();
    cmd[0] = baud;
    UART_write(uart, cmd, 1);

  _delay_cycles((48000 - 1) * 10); // (48000 - 1) = 1 ms
//    Sleep(10);
}

void Adafruit_GPS_sendCommand(const char *str, UART_Handle uart) {
    int i;
    for (i = 0; i < sizeof(str); i++) {
        cmd[i] = str[i];
    }
    UART_write(uart, cmd, sizeof(str));
}

bool Adafruit_GPS_newNMEAreceived(void) {
  return recvdflag;
}

void Adafruit_GPS_pause(bool p) {
  paused = p;
}

char *Adafruit_GPS_lastNMEA(void) {
  recvdflag = false;
  return (char *)lastline;
}



bool Adafruit_GPS_waitForSentence(const char *wait4me, uint8_t max) {
  char str[20];

  uint8_t i=0;
  while (i < max) {
    if (Adafruit_GPS_newNMEAreceived()) {
      char *nmea = Adafruit_GPS_lastNMEA();
      strncpy(str, nmea, 20);
      str[19] = 0;
      i++;

      if (strstr(str, wait4me))
    return true;
    }
  }

  return false;
}


// Standby Mode Switches
bool Adafruit_GPS_standby(UART_Handle uart) {
  if (inStandbyMode) {
    return false;  // Returns false if already in standby mode, so that you do not wake it up by sending commands to GPS
  }
  else {
    inStandbyMode = true;
    Adafruit_GPS_sendCommand(PMTK_STANDBY, uart);
    //return waitForSentence(PMTK_STANDBY_SUCCESS);  // don't seem to be fast enough to catch the message, or something else just is not working
    return true;
  }
}

void Adafruit_GPS_parseSetup(UART_Handle uart) {
      // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
      // also spit it out
//      Serial.println("Adafruit GPS library basic test!");

    // System_printf("Setting up parsing\n");
      // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
//      Adafruit_GPS_begin(9600, uart);
      // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
      Adafruit_GPS_sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA, uart);
      // uncomment this line to turn on only the "minimum recommended" data
      //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
      // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
      // the parser doesn't care about other sentences at this time
      // Set the update rate
      Adafruit_GPS_sendCommand(PMTK_SET_NMEA_UPDATE_1HZ, uart); // 1 Hz update rate
      // For the parsing code to work nicely and have time to sort thru the data, and
      // print it out we don't suggest using anything higher than 1 Hz

      // Request updates on antenna status, comment out to keep quiet
      Adafruit_GPS_sendCommand(PGCMD_ANTENNA, uart);

      _delay_cycles((48000 - 1) * 1000); // (48000 - 1) = 1 ms
//      sleep(1);

      // Ask for firmware version
//      System_printf(PMTK_Q_RELEASE);
}

void Adafruit_GPS_parseOutput(UART_Handle uart, struct gps *GPS)
{
  char c = Adafruit_GPS_read(uart); // read data from the GPS in the 'main loop'
  // System_printf("read done\n\r");

  // if (GPSECHO) {  // if you want to debug, this is a good time to do it!
  //   if (c) UART_write(uartPC, &c, 1);
  // }
  
  if (Adafruit_GPS_newNMEAreceived()) { // if a sentence is received, we can check the checksum, parse it...
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data

    // System_printf(Adafruit_GPS_lastNMEA()); // this also sets the newNMEAreceived() flag to false
    
    // Display_printf(display, 0, 0, Adafruit_GPS_lastNMEA());
    
    // UART_write(uartXbee, Adafruit_GPS_lastNMEA(), 128);
    
    if (!Adafruit_GPS_parse(Adafruit_GPS_lastNMEA())) { // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
    }

    if (fix) {
      // sprintf(tempStr, "Latitude = %f %c, longitude = %f %c\r\n", latitude, lat, longitude, lon);
      // UART_write(uartXbee, tempStr, strlen(tempStr));

      // sprintf(tempStr, "Degrees latitude = %f, longitude = %f\r\n", latitudeDegrees, longitudeDegrees);
      sprintf(tempStr, "%f,%f,%0.2i:%0.2i:%0.2i\r\n", latitudeDegrees, longitudeDegrees, hour, minute, seconds);
      UART_write(uartXbee, tempStr, strlen(tempStr));

      // sprintf(tempStr, "Satellites = %i\r\n", satellites);
      // UART_write(uartXbee, tempStr, strlen(tempStr));

      GPS->lat = latitudeDegrees + latitude * 1.0 / 60.0;
      GPS->lon = -longitudeDegrees - longitude * 1.0 / 60.0;
    }

  // if (timer > millis()) timer = millis(); // if millis() or timer wraps around, we'll just reset it

  // approximately every 2 seconds or so, print out the current stats
//  if (millis() - timer > 2000) {
   // sleep(2);
//  _delay_cycles((48000 - 1) * 100); // (48000 - 1) = 1 ms
//    timer = millis(); // reset the timer

//
//
//          //    System_printf("\nTime: ");
////        System_printf("%i", hour); System_printf(":");
////        System_printf("%i", minute); System_printf(":");
////        System_printf("%i", seconds); System_printf(".");
////        System_printf("%i\n", milliseconds);
////        System_printf("Date: ");
////        System_printf("%i", day); System_printf("/");
////        System_printf("%i", month); System_printf("/20");
////        System_printf("%i\n", year);
////        System_printf("Fix: "); System_printf("%i", (int)fix);
////        System_printf(" quality: "); System_printf("%i\n", (int)fixquality);
//        if (fix) {
//
//          System_printf("Location: ");
//          System_printf("%f, %i", latitude, 4); System_printf("%c", lat);
//          System_printf(", ");
//          System_printf("%f, %i", longitude, 4); System_printf("%c\n", lon);
//          System_printf("Location (in degrees, works with Google Maps): ");
         // System_printf("%f, %i", latitudeDegrees, 4);
//          System_printf(", ");
//          System_printf("%f, %i\n", longitudeDegrees, 4);

//          System_printf("Speed (knots): "); System_printf("%f\n", speed);
//          System_printf("Angle: "); System_printf("%f\n", angle);
//          System_printf("Altitude: "); System_printf("%f\n", altitude);
//          System_printf("Satellites: "); System_printf("%i\n", (int)satellites);
//    }
//  }
  }
}
