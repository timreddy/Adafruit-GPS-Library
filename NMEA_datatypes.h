#ifndef NMEA_DATATYPES_H
#define NMEA_DATATYPES_H

#include <stdlib.h>
#include <stdio.h>

class NMEA_Loc {
  public:
    double deg;
  
    NMEA_Loc();
    void set(char* field);
};

class NMEA_Time {
  public:
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    double        msec;

    unsigned char day;
    unsigned char month;
    unsigned char year;

    NMEA_Time();

    void set_time(char* field);
    void set_date(char* field);
};

class NMEA_GPS {
  public:
    NMEA_Time time;          // 1

    NMEA_Loc lat;           // 2
    bool lat_hemi;          // 3 

    NMEA_Loc lon;           // 4
    bool lon_hemi;          // 5 

    unsigned char fix_qual;  // 6
    unsigned char n_sat;     // 7

    double horiz_dop;// 8
    double alt;      // 9
    double geoid_sep;// 11
    double diff_age; // 13

    double speed;
    double CMG;
    double mag_var;

    bool warning;

    unsigned int   diff_id;  // 14

    void parse_GGA(char* nmea);
    void parse_RMC(char* nmea);
    void to_string(char** pstr, size_t l);

    NMEA_GPS();
};

#endif
