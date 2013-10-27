/* 
 * File:   CircleList.h
 * Author: xy
 *
 * Created on October 22, 2013, 8:54 PM
 */

#ifndef CIRCLELIST_H
#define	CIRCLELIST_H


class CircleList {
public:
    CircleList(int len);
    virtual ~CircleList();
    
    void add(float elm);
    void listSum();
    void listAvg();
    void listRms();
    void freeList();
    float getRms() const;
    float getAvg() const;
    float getSum() const;
private:
    int length;
    int curElmNum;
    int curIdx;
    float *circleList;
    float sum;
    float avg;
    float rms;
    float lastElm;

};

#endif	/* CIRCLELIST_H */

