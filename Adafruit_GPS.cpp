/***********************************
This is our GPS library

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information
All text above must be included in any redistribution
****************************************/
#ifdef __AVR__
  // Only include software serial on AVR platforms (i.e. not on Due).
  #include <SoftwareSerial.h>
#endif
#include <Adafruit_GPS.h>

// Use a buffered read class to separate the GPS handling
// from the double buffering details.
#include "buffered_read.h"
#include "NMEA_datatypes.h"
#include <string.h>
#include <stdlib.h>

// how long are max NMEA lines to parse?
// timreddy: 82 characters, including leading $ and CR/LF
#define NMEA_0183_LINE_LEN 82
#define N_BUFFERS 4

unsigned int Adafruit_GPS::NMEA_checksum(char *nmea) {
  //
  // see if there is a checksum
  //
  char* nmea_end = nmea + strlen(nmea) - 5;
  if(*nmea_end != '*') { 
    return false;
  }

  //pre-increment to skip initial $
  unsigned int checksum = 0; 
  nmea++;
  for(; nmea < nmea_end; nmea++) {
    checksum ^= *nmea;
  }

  return (checksum == strtol(nmea_end+1, NULL, 16));
} 

boolean Adafruit_GPS::parse(char *nmea) {
  // do checksum check
  if(!NMEA_checksum(nmea)) { 
    return false; 
  }
  
  // look for a few common sentences
  //Serial.println(nmea);
  if (strstr(nmea, "$GPGGA")) {
    data.parse_GGA(nmea);
    return true;
  }
  if (strstr(nmea, "$GPRMC")) {
    data.parse_RMC(nmea);
    return true;
  }

  return false;
}

char Adafruit_GPS::read(void) {
  char c = 0;
  
  if (paused) return c;

#ifdef __AVR__
  if(gpsSwSerial) {
    if(!gpsSwSerial->available()) return c;
    c = gpsSwSerial->read();
  } else 
#endif
  {
    if(!gpsHwSerial->available()) return c;
    c = gpsHwSerial->read();
  }

  //Serial.print(c);

  buffer.write_line(&c, 1);

  if(c == '\n') {
    recvdflag = true;
  }
  return c;
}

#ifdef __AVR__
// Constructor when using SoftwareSerial or NewSoftSerial
#if ARDUINO >= 100
Adafruit_GPS::Adafruit_GPS(SoftwareSerial *ser) : buffer(NMEA_0183_LINE_LEN * N_BUFFERS)
#else
Adafruit_GPS::Adafruit_GPS(NewSoftSerial *ser)  : buffer(NMEA_0183_LINE_LEN * N_BUFFERS)
#endif
{
  common_init();     // Set everything to common state, then...
  gpsSwSerial = ser; // ...override gpsSwSerial with value passed.
}
#endif

// Constructor when using HardwareSerial
Adafruit_GPS::Adafruit_GPS(HardwareSerial *ser) : buffer(NMEA_0183_LINE_LEN * N_BUFFERS) {
  common_init();  // Set everything to common state, then...
  gpsHwSerial = ser; // ...override gpsHwSerial with value passed.
}

// Initialization code used by all constructor types
void Adafruit_GPS::common_init(void) {
#ifdef __AVR__
  gpsSwSerial = NULL; // Set both to NULL, then override correct
#endif
  gpsHwSerial = NULL; // port pointer in corresponding constructor
  recvdflag   = false;
  paused      = false;

  //
  // use calloc to init to 0's
  //
  if(!(this->NMEAline = (char*) calloc(NMEA_0183_LINE_LEN, sizeof(char)))) {
    exit(1);
  }
}

void Adafruit_GPS::begin(uint16_t baud)
{
#ifdef __AVR__
  if(gpsSwSerial) 
    gpsSwSerial->begin(baud);
  else 
    gpsHwSerial->begin(baud);
#endif

  delay(10);
}

void Adafruit_GPS::sendCommand(const char *str) {
#ifdef __AVR__
  if(gpsSwSerial) 
    gpsSwSerial->println(str);
  else    
#endif
    gpsHwSerial->println(str);
}

boolean Adafruit_GPS::newNMEAreceived(void) {
  return recvdflag;
}

void Adafruit_GPS::pause(boolean p) {
  paused = p;
}

char *Adafruit_GPS::lastNMEA(void) {
  recvdflag = false;
  unsigned int n_recv = (unsigned int) buffer.read_line(this->NMEAline, NMEA_0183_LINE_LEN, '\n');
  return (char *)this->NMEAline;
}

boolean Adafruit_GPS::waitForSentence(const char *wait4me, uint8_t max) {
  char str[20];

  uint8_t i=0;
  while (i < max) {
    if (newNMEAreceived()) { 
      char *nmea = lastNMEA();
      strncpy(str, nmea, 20);
      str[19] = 0;
      i++;

      if (strstr(str, wait4me))
	return true;
    }
  }

  return false;
}

boolean Adafruit_GPS::LOCUS_StartLogger(void) {
  sendCommand(PMTK_LOCUS_STARTLOG);
  recvdflag = false;
  return waitForSentence(PMTK_LOCUS_STARTSTOPACK);
}

boolean Adafruit_GPS::LOCUS_StopLogger(void) {
  sendCommand(PMTK_LOCUS_STOPLOG);
  recvdflag = false;
  return waitForSentence(PMTK_LOCUS_STARTSTOPACK);
}

boolean Adafruit_GPS::LOCUS_ReadStatus(void) {
  sendCommand(PMTK_LOCUS_QUERY_STATUS);
  
  if (! waitForSentence("$PMTKLOG"))
    return false;

  char *response = lastNMEA();
  uint16_t parsed[10];
  uint8_t i;
  
  for (i=0; i<10; i++) parsed[i] = -1;
  
  response = strchr(response, ',');
  for (i=0; i<10; i++) {
    if (!response || (response[0] == 0) || (response[0] == '*')) 
      break;
    response++;
    parsed[i]=0;
    while ((response[0] != ',') && 
	   (response[0] != '*') && (response[0] != 0)) {
      parsed[i] *= 10;
      char c = response[0];
      if (isDigit(c))
        parsed[i] += c - '0';
      else
        parsed[i] = c;
      response++;
    }
  }
  LOCUS_serial = parsed[0];
  LOCUS_type = parsed[1];
  if (isAlpha(parsed[2])) {
    parsed[2] = parsed[2] - 'a' + 10; 
  }
  LOCUS_mode = parsed[2];
  LOCUS_config = parsed[3];
  LOCUS_interval = parsed[4];
  LOCUS_distance = parsed[5];
  LOCUS_speed = parsed[6];
  LOCUS_status = !parsed[7];
  LOCUS_records = parsed[8];
  LOCUS_percent = parsed[9];

  return true;
}

// Standby Mode Switches
boolean Adafruit_GPS::standby(void) {
  if (inStandbyMode) {
    return false;  // Returns false if already in standby mode, so that you do not wake it up by sending commands to GPS
  }
  else {
    inStandbyMode = true;
    sendCommand(PMTK_STANDBY);
    //return waitForSentence(PMTK_STANDBY_SUCCESS);  // don't seem to be fast enough to catch the message, or something else just is not working
    return true;
  }
}

boolean Adafruit_GPS::wakeup(void) {
  if (inStandbyMode) {
   inStandbyMode = false;
    sendCommand("");  // send byte to wake it up
    return waitForSentence(PMTK_AWAKE);
  }
  else {
      return false;  // Returns false if not in standby mode, nothing to wakeup
  }
}
