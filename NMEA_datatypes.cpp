#include "NMEA_datatypes.h"
#include <string.h>
#include <stdlib.h>

#include "Arduino.h"

void NMEA_Time::set_time(char* field) {
  char* frac_field = strchr(field, '.');

  this->hour = (field[0] - '0') * 10 + field[1] - '0';
  this->min  = (field[2] - '0') * 10 + field[3] - '0';;
  this->sec  = (field[3] - '0') * 10 + field[4] - '0';;
  this->msec = atof(frac_field);
}

NMEA_Time::NMEA_Time() {
  this->hour = 0;
  this->min  = 0;
  this->sec  = 0;
  this->msec = 0;
  this->day  = 0;
  this->month= 0;
  this->year = 0;
}

void NMEA_Time::set_date(char* field) {
  this->day   = (field[0] - '0') * 10 + field[1] - '0'; 
  this->month = (field[2] - '0') * 10 + field[3] - '0'; 
  this->year  = (field[4] - '0') * 10 + field[5] - '0'; 
}

NMEA_Loc::NMEA_Loc() {
  deg = NAN;
}

void NMEA_Loc::set(char* field) {
  char* int_field = field;
  char* frac_field = strchr(field, '.');

  div_t d = div(atoi(int_field), 100);
  this->deg = d.quot;

  this->deg += d.rem/60.0 + strtod(frac_field, 0)/60.0;
}

NMEA_GPS::NMEA_GPS() {
  lat_hemi  = false;
  lon_hemi  = false;
  fix_qual  = 255;
  n_sat     = 0;
  horiz_dop = NAN;
  alt       = NAN;
  geoid_sep = NAN;
  diff_age  = NAN;
  speed     = NAN;
  CMG       = NAN;
  diff_id   = 0;
}

void NMEA_GPS::parse_GGA(char* nmea) {
  char* last; 
  strtok_r(nmea, ",", &last);
  this->time.set_time(strtok_r(0, ",", &last)); // time
  this->lat.set(strtok_r(0, ",", &last));  // lat
  this->lat_hemi  = (strtok_r(0, ",", &last)[0] == 'N');  // hemi
  this->lon.set(strtok_r(0, ",", &last));  //lon
  this->lon_hemi  = (strtok_r(0, ",", &last)[0] == 'E');  // hemi
  this->fix_qual  = atoi(strtok_r(0, ",", &last));
  this->n_sat     = atoi(strtok_r(0, ",", &last));
  this->horiz_dop = atof(strtok_r(0, ",", &last));
  this->alt       = atof(strtok_r(0, ",", &last));
  this->geoid_sep = atof(strtok_r(0, ",", &last));
  this->diff_age  = atof(strtok_r(0, ",", &last));
  this->diff_id   = 0; // Need example of this.
}

void NMEA_GPS::parse_RMC(char* nmea) {
  char* last; 
  strtok_r(nmea, ",", &last);
  this->time.set_time(strtok_r(0, ",", &last)); // time
  this->warning = (strtok_r(0, ",", &last)[0] == 'V');    // receiver warning
  this->lat.set(strtok_r(0, ",", &last));  // lat
  this->lat_hemi  = (strtok_r(0, ",", &last)[0] == 'N');  // hemi
  this->lon.set(strtok_r(0, ",", &last));  //lon
  this->lon_hemi  = (strtok_r(0, ",", &last)[0] == 'E');  // hemi
  this->speed     = atof(strtok_r(0, ",", &last)); // speed
  this->CMG       = atof(strtok_r(0, ",", &last));
  this->time.set_date(strtok_r(0, ",", &last));
  this->mag_var   = atof(strtok_r(0, ",", &last));
}

void NMEA_GPS::to_string(char** pstr, size_t buflen) {
  snprintf(*pstr, buflen, "Time: %d:%d:%d %d %c, %d %c", time.hour, time.min, time.sec, lat.deg, (lat_hemi?'N':'S'), lon.deg, (lon_hemi?'W':'E'));
} 
