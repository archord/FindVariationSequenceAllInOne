/* 
 * File:   cmhead.h
 * Author: xy
 *
 * Created on October 21, 2013, 8:39 AM
 */

#ifndef CMHEAD_H
#define	CMHEAD_H

typedef struct CM_STAR {
    long id;
    long starId;
    float alpha;
    float delta;
    float background;
    float classstar;
    float ellipticity;
    float flags;
    float mag;
    float mage;
    float magnorm;
    float fwhm;
    float magcalib;
    float magcalibe;
    float pixx;
    float pixy;
    float thetaimage;
    float vignet;
    float flux;
    float threshold;
    float error;
    struct CM_STAR *match;
    //struct CM_STAR *standrad;
    struct CM_STAR *next;
} cm_star;

typedef struct CM_ZONE {
    int starNum;
    cm_star *star;
} cm_zone;

#define FIND_MOST_LOW
#define PRINT_CM_DETAIL1

static const int MaxStringLength = 1024;
static const float CompareFloat = 0.000001;
static const long MaxMallocMemory = 2147483648l; //1GB=1073741824l 2GB=2147483648l

float getLineDistance(cm_star *p1, cm_star *p2);
template<typename T>
inline T getMax(const T& a, const T& b);
long countFileLines(char *fName);

#endif	/* CMHEAD_H */

