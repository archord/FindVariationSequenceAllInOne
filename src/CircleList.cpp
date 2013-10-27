/* 
 * File:   CircleList.cpp
 * Author: xy
 * 
 * Created on October 22, 2013, 8:54 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CircleList.h"

CircleList::CircleList(int len) {
    length = len;
    curElmNum = 0;
    curIdx = 0;
    sum = 0.0;
    avg = 0.0;
    rms = 0.0;
    lastElm = 0.0;
    circleList = (float*) malloc(sizeof (float)*length);
    memset(circleList, 0, sizeof (float)*length);
}

CircleList::~CircleList() {
    freeList();
}

void CircleList::add(float elm) {
    lastElm = circleList[curIdx];
    circleList[curIdx] = elm;
    
    listSum();
    
    if (curElmNum < length)
        curElmNum++;

    listAvg();
    listRms();

    curIdx++;
    if (curIdx >= length) {
        curIdx = curIdx % length;
    }
}

void CircleList::listSum() {
    if (curElmNum < length) {
        sum += circleList[curIdx];
    } else {
        sum += (circleList[curIdx] - lastElm);
    }
}

void CircleList::listAvg() {
    avg = sum / curElmNum;
}

void CircleList::listRms() {
    float rmsSum = 0.0;
    for (int i = 0; i < curElmNum; i++) {
        float diff = circleList[i] - avg;
        rmsSum += diff * diff;
    }
    rms = sqrt(rmsSum/curElmNum);
}

float CircleList::getRms() const {
    return rms;
}

float CircleList::getAvg() const {
    return avg;
}

float CircleList::getSum() const {
    return sum;
}

void CircleList::freeList() {
    free(circleList);
}


