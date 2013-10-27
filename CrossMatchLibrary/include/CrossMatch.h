/* 
 * File:   CrossMatch.h
 * Author: xy
 *
 * Created on October 18, 2013, 8:48 AM
 */

#ifndef CROSSMATCH_H
#define	CROSSMATCH_H

#include "Partition.h"

class CrossMatch {
public:

    static const int TimesOfErrorRadius = 10; //length of search radius

    CrossMatch();
    CrossMatch(const CrossMatch& orig);
    virtual ~CrossMatch();

    cm_star *readStarFile(char *fName, int &starNum);
    cm_star *copyStarList(cm_star *starlst);
    void match(char *refName, char *objName, float errorBox);
    void match(cm_star *ref, int refNum, cm_star *obj, float errorBox);
    void compareResult(char *refName, char *objName, char *outName, float errorBox);
    void matchNoPartition(char *refName, char *objName, float errorBox);
    void matchNoPartition(cm_star *ref, cm_star *obj, float errorBox);
    void printMatchedRst(char *outfName, float errorBox);
    void printMatchedRst(char *outfName, cm_star *starList, float errorBox);
    void printOTStar(char *outfName, float errorBox);
    void printAllStarList(char *outfName, cm_star *starList, float errorBox);
    void freeStarList(cm_star *starList);
    void freeAllMemory();
    
private:
    
    int refNum;
    int objNum;
    cm_star *refStarList;
    cm_star *objStarList;
    cm_star *refStarListNoPtn;
    cm_star *objStarListNoPtn;
    
protected:
    float errRadius;            //两颗星匹配的最小距离
    Partition *zones;
};

#endif	/* CROSSMATCH_H */

