
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "fitsio.h"
#include "utils.h"

void strToJD(const char *jdStr, jl_day &jd);

double calJulianDay(const char *jdStr) {

    jl_day jd;
    strToJD(jdStr, jd);
    int y, m, B;
    y = jd.year;
    m = jd.month;
    if (jd.month <= 2) {
        y = jd.year - 1;
        m = jd.month + 12;
    }

    // Correct for the lost days in Oct 1582 when the Gregorian calendar 
    // replaced the Julian calendar. 
    B = -2;
    if (jd.year > 1582 || (jd.year == 1582 && (jd.month > 10 || (jd.month == 10 && jd.day >= 15)))) {
        B = y / 400 - y / 100;
    }

    return (floor(365.25 * y) +
            floor(30.6001 * (m + 1)) + B + 1720996.5 +
            jd.day + (double)jd.hour / 24.0 + (double)jd.minute / 1440.0 + (double)jd.second / 86400.0);

}

/**
 * 
 * @param jdStr '2013-01-11T18:26:22' /YYYY-MM-DDThh:mm:ss  UT
 * @param cdate
 */
void strToJD(const char *jdStr, jl_day &jd){
    sscanf(jdStr, "%d-%d-%dT%d:%d:%d", 
            &jd.year, &jd.month, &jd.day, &jd.hour, &jd.minute, &jd.second);
}

void readFitsAttrStr(char *fitsName, char *strName, char *strVal) {

    fitsfile *fptr; /* pointer to the FITS file, defined in fitsio.h */
    int status = 0;

    if (fits_open_file(&fptr, fitsName, READONLY, &status)) {
        printf("Open file :%s error!\n", fitsName);
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
        return;
    }

    /* read the NAXIS1 and NAXIS2 keyword to get table size */
    if (fits_read_key_str(fptr, strName, strVal, NULL, &status)) {
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
        return;
    }

    if (fits_close_file(fptr, &status)) {
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
        return;
    }
}

void readFitsAttrFloat(char *fitsName, char *strName, float &fltVal) {

    fitsfile *fptr; /* pointer to the FITS file, defined in fitsio.h */
    int status = 0;

    if (fits_open_file(&fptr, fitsName, READONLY, &status)) {
        printf("Open file :%s error!\n", fitsName);
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
        return;
    }

    /* read the NAXIS1 and NAXIS2 keyword to get table size */
    if (fits_read_key_flt(fptr, strName, &fltVal, NULL, &status)) {
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
        return;
    }

    if (fits_close_file(fptr, &status)) {
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
        return;
    }
}

