
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "cmhead.h"

float getLineDistance(cm_star *p1, cm_star *p2) {
    float xdiff = p1->pixx - p2->pixx;
    float ydiff = p1->pixy - p2->pixy;
    float dist = sqrt(xdiff * xdiff + ydiff * ydiff);
    return dist;
}

template<typename T>
inline T getMax(const T& a, const T& b) {
    return a > b ? a : b;
}

long countFileLines(char *fName) {

    FILE *fp = fopen(fName, "r");

    if (fp == NULL) {
        return 0;
    }

    long lineNum = 0;
    char line[MaxStringLength];
    while (fgets(line, MaxStringLength, fp) != NULL) {
        lineNum++;
    }
    fclose(fp);
    return lineNum;
}