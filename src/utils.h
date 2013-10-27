/* 
 * File:   utils.h
 * Author: xy
 *
 * Created on October 22, 2013, 7:54 PM
 */

#ifndef UTILS_H
#define	UTILS_H

typedef struct JULIAN_DAY{
    int year; /*!< Years. All values are valid */
    int month; /*!< Months. Valid values : 1 (January) - 12 (December ) */
    int day; /*!< Days. Valid values 1 - 28,29,30,31 Depends on month .*/
    int hour; /*!< Hours. Valid values 0 - 23. */
    int minute; /*!< Minutes. Valid values 0 - 59. */
    int second; /*!< Seconds. Valid values 0 - 59.99999.... */
} jl_day;

double calJulianDay(const char *jdStr);
void readFitsAttrStr(char *fitsName, char *strName, char *strVal);
void readFitsAttrFloat(char *fitsName, char *strName, float &fltVal);

#endif	/* UTILS_H */

