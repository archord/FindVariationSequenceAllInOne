/* 
 * File:   Partition.h
 * Author: xy
 *
 * Created on October 18, 2013, 8:47 AM
 */

#ifndef PARTITION_H
#define	PARTITION_H

#include "cmhead.h"

class Partition {
private:
    float maxx;
    float maxy;
    float minx;
    float miny;
    float fieldWidth;           //星表视场的宽度
    float fieldHeight;          //星表视场的高度

    float errRadius;            //两颗星匹配的最小距离
    float searchRadius;         //搜索匹配分区时的矩形搜索区域（边长为2*searchRadius）
    float minZoneLength;        //最小分区长度 = 3*errRadius
    float zoneInterval;         //实际分区长度
    float zoneIntervalRecp;     //实际分区长度

    int zoneXnum;               //分区在X方向上的个数
    int zoneYnum;               //分区在Y方向上的个数
    int totalZone;              //分区的总个数
    int totalStar;              //星的总数
    cm_zone *zoneArray;         //分区数组

public:
    Partition();
    Partition(const Partition& orig);
    Partition(float errBox, float minZoneLen, float searchRds);
    virtual ~Partition();

    void partitonStarField(cm_star *starList, int starNum);
    void getMatchStar(cm_star *objStar);
    void printZoneDetail(char *fName);
    void freeZoneArray();
    
    void setSearchRadius(float searchRadius);
    float getSearchRadius() const;
    void setErrRadius(float errRadius);
    float getErrRadius() const;
    void setMinZoneLength(float minZoneLength);
    float getMinZoneLength() const;

private:
    cm_star *searchSimilarStar(long zoneIdx, cm_star *star);
    long *getStarSearchZone(cm_star *star, long &sZoneNum);
    long getZoneIndex(cm_star * star);
    void getMinMaxXY(cm_star *starList);
    void addStarToZone(cm_star *star, long zoneIdx);
    void freeStarList(cm_star *starList);
};

#endif	/* PARTITION_H */

