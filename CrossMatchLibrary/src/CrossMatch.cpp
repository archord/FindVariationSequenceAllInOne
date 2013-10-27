/* 
 * File:   CrossMatch.cpp
 * Author: xy
 * 
 * Created on October 18, 2013, 8:48 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CrossMatch.h"

CrossMatch::CrossMatch() {

    refNum = 0;
    objNum = 0;
    refStarList = NULL;
    objStarList = NULL;
    refStarListNoPtn = NULL;
    objStarListNoPtn = NULL;
    zones = NULL;
}

CrossMatch::CrossMatch(const CrossMatch& orig) {
}

CrossMatch::~CrossMatch() {
}

void CrossMatch::match(char *refName, char *objName, float errorBox) {

    float minZoneLen = errorBox * TimesOfErrorRadius;
    float searchRds = errorBox;

    refStarList = readStarFile(refName, refNum);
    objStarList = readStarFile(objName, objNum);

    zones = new Partition(errorBox, minZoneLen, searchRds);
    zones->partitonStarField(refStarList, refNum);
    refStarList = NULL;

    cm_star *nextStar = objStarList;
    while (nextStar) {
        zones->getMatchStar(nextStar);
        nextStar = nextStar->next;
    }

#ifdef PRINT_CM_DETAIL
    printf("partition match done!\n");
#endif
}

void CrossMatch::match(cm_star *ref, int refNum, cm_star *obj, float errorBox) {

    float minZoneLen = errorBox * TimesOfErrorRadius;
    float searchRds = errorBox;

    zones = new Partition(errorBox, minZoneLen, searchRds);
    zones->partitonStarField(ref, refNum);
    ref = NULL;

    cm_star *nextStar = obj;
    while (nextStar) {
        zones->getMatchStar(nextStar);
        nextStar = nextStar->next;
    }

#ifdef PRINT_CM_DETAIL
    printf("partition match done!\n");
#endif
}

/**
 * circulate each star on 'refList', find the nearest on as the match star of objStar
 * the matched star is stored on obj->match, 
 * the distance between two stars is stored on obj->error
 * @param ref
 * @param obj
 */
void CrossMatch::matchNoPartition(char *refName, char *objName, float errorBox) {

    int refNum, objNum;
    refStarListNoPtn = readStarFile(refName, refNum);
    objStarListNoPtn = readStarFile(objName, objNum);

    cm_star *tObj = objStarListNoPtn;

    while (tObj) {
        float tError = 0.0;
        float minError = getLineDistance(refStarListNoPtn, tObj);
        tObj->match = refStarListNoPtn;
        tObj->error = minError;
        cm_star *tRef = refStarListNoPtn->next;
        while (tRef) {
            tError = getLineDistance(tRef, tObj);
            if (tError < minError) {
                minError = tError;
                tObj->match = tRef;
                tObj->error = tError;
            }
            tRef = tRef->next;
        }
        tObj = tObj->next;
    }

    //freeStarList(refStarListNoPtn);
    //freeStarList(objStarListNoPtn);
#ifdef PRINT_CM_DETAIL
    printf("no partition match done!\n");
#endif
}

/**
 * the matched star is stored on obj->match, 
 * the distance between two stars is stored on obj->error
 * @param ref
 * @param obj
 */
void CrossMatch::matchNoPartition(cm_star *ref, cm_star *obj, float errorBox) {

    cm_star *tObj = obj;

    while (tObj) {
        float tError = 0.0;
        float minError = getLineDistance(ref, tObj);
        tObj->match = refStarListNoPtn;
        tObj->error = minError;
        cm_star *tRef = ref->next;
        while (tRef) {
            tError = getLineDistance(tRef, tObj);
            if (tError < minError) {
                minError = tError;
                tObj->match = tRef;
                tObj->error = tError;
            }
            tRef = tRef->next;
        }
        tObj = tObj->next;
    }
}

/**
 * default only read first three column(X, Y, mag)
 * @param fName
 * @param starNum
 * @return 
 */
cm_star *CrossMatch::readStarFile(char *fName, int &starNum) {

    FILE *fp = fopen(fName, "r");
    if (fp == NULL) {
        return NULL;
    }

    starNum = 0;
    cm_star *starList = NULL;
    cm_star *tStar = NULL;
    cm_star *nextStar = NULL;
    char line[MaxStringLength];

    if (fgets(line, MaxStringLength, fp) != NULL) {
        nextStar = (cm_star *) malloc(sizeof (cm_star));
        nextStar->id = starNum;
        nextStar->next = NULL;
        sscanf(line, "%f%f%f", &nextStar->pixx, &nextStar->pixy, &nextStar->mag);
        starList = nextStar;
        tStar = nextStar;
        starNum++;
    }

    while (fgets(line, MaxStringLength, fp) != NULL) {
        nextStar = (cm_star *) malloc(sizeof (cm_star));
        nextStar->id = starNum;
        nextStar->next = NULL;
        sscanf(line, "%f%f%f", &nextStar->pixx, &nextStar->pixy, &nextStar->mag);
        tStar->next = nextStar;
        tStar = nextStar;
        starNum++;
    }

#ifdef PRINT_CM_DETAIL
    printf("%s\t%d stars\n", fName, starNum);
#endif
    return starList;
}

cm_star *CrossMatch::copyStarList(cm_star *starlst) {

    cm_star *tHead = NULL;
    cm_star *tStar = NULL;
    cm_star *origlst = starlst;
    
    if (NULL != origlst) {
        tHead = (cm_star *) malloc(sizeof (cm_star));
        memcpy(tHead, origlst, sizeof (cm_star));
        origlst = origlst->next;
        tStar = tHead;
    }

    while (NULL != origlst) {
        tStar->next = (cm_star *) malloc(sizeof (cm_star));
        memcpy(tStar->next, origlst, sizeof (cm_star));
        origlst = origlst->next;
        tStar = tStar->next;
    }
    return tHead;
}

void CrossMatch::freeStarList(cm_star *starList) {

    if (NULL != starList) {
        cm_star *tStar = starList->next;
        while (tStar) {
            starList->next = tStar->next;
            free(tStar);
            tStar = starList->next;
        }
        free(starList);
    }
}

void CrossMatch::freeAllMemory() {

    freeStarList(refStarList);
    freeStarList(objStarList);
    freeStarList(refStarListNoPtn);
    freeStarList(objStarListNoPtn);
    if (NULL != zones) {
        zones->freeZoneArray();
    }
}

void CrossMatch::compareResult(char *refName, char *objName, char *outfName, float errorBox) {

    match(refName, objName, errorBox);
    matchNoPartition(refName, objName, errorBox);

    FILE *fp = fopen(outfName, "w");

    cm_star *tStar1 = objStarList;
    cm_star *tStar2 = objStarListNoPtn;
    int i = 0, j = 0, k = 0, m = 0, n = 0, g = 0;
    while (NULL != tStar1 && NULL != tStar2) {
        if (NULL != tStar1->match && NULL != tStar2->match) {
            i++;
            float errDiff = fabs(tStar1->error - tStar2->error);
            if (errDiff < CompareFloat)
                n++;
        } else if (NULL != tStar1->match && NULL == tStar2->match) {
            j++;
        } else if (NULL == tStar1->match && NULL != tStar2->match) {//ommit and OT
            k++;
            if (tStar2->error < errorBox)
                g++;
        } else {
            m++;
        }
        tStar1 = tStar1->next;
        tStar2 = tStar2->next;
    }
    fprintf(fp, "total star %d\n", i + j + k + m);
    fprintf(fp, "matched %d , two method same %d\n", i, n);
    fprintf(fp, "partition matched but nopartition notmatched %d\n", j);
    fprintf(fp, "nopartition matched but partition notmatched %d, small than errorBox %d\n", k, g);
    fprintf(fp, "two method are not matched %d\n", m);

    fprintf(fp, "\nX1,Y1,X1m,Y1m,err1 is the partition related info\n");
    fprintf(fp, "X2,Y2,X2m,Y2m,err2 is the nopartition related info\n");
    fprintf(fp, "X1,Y1,X2,Y2 is orig X and Y position of stars\n");
    fprintf(fp, "X1m,Y1m,X2m,Y2m is matched X and Y position of stars\n");
    fprintf(fp, "pos1,pos2 is the two method's match distance\n");
    fprintf(fp, "the following list is leaked star of partition method\n");
    fprintf(fp, "X1\tY1\tX2\tY2\tX1m\tY1m\tX2m\tY2m\tpos1\tpos2\n");
    tStar1 = objStarList;
    tStar2 = objStarListNoPtn;
    while (NULL != tStar1 && NULL != tStar2) {
        if (NULL == tStar1->match && NULL != tStar2->match && tStar2->error < errorBox) { //ommit and OT
            fprintf(fp, "%12f %12f %12f %12f %12f %12f %12f %12f %12f %12f\n",
                    tStar1->pixx, tStar1->pixy, tStar2->pixx, tStar2->pixy,
                    0.0, 0.0, tStar2->match->pixx, tStar2->match->pixy,
                    tStar1->error, tStar2->error);
        }
        tStar1 = tStar1->next;
        tStar2 = tStar2->next;
    }

    fprintf(fp, "the following list is OT\n");
    fprintf(fp, "X1\tY1\tX2\tY2\tX1m\tY1m\tX2m\tY2m\tpos1\tpos2\n");
    tStar1 = objStarList;
    tStar2 = objStarListNoPtn;
    while (NULL != tStar1 && NULL != tStar2) {
        if (NULL == tStar1->match && NULL != tStar2->match && tStar2->error > errorBox) { //ommit and OT
            fprintf(fp, "%12f %12f %12f %12f %12f %12f %12f %12f %12f %12f\n",
                    tStar1->pixx, tStar1->pixy, tStar2->pixx, tStar2->pixy,
                    0.0, 0.0, tStar2->match->pixx, tStar2->match->pixy,
                    tStar1->error, tStar2->error);
        }
        tStar1 = tStar1->next;
        tStar2 = tStar2->next;
    }

    fclose(fp);
    freeAllMemory();
}

void CrossMatch::printMatchedRst(char *outfName, float errorBox) {

    FILE *fp = fopen(outfName, "w");
    fprintf(fp, "Id\tX\tY\tmId\tmX\tmY\tdistance\n");

    long count = 0;
    cm_star *tStar = objStarList;
    while (NULL != tStar) {
        if (NULL != tStar->match && tStar->error < errorBox) {
            fprintf(fp, "%8ld %12f %12f %8ld %12f %12f %12f\n",
                    tStar->starId, tStar->pixx, tStar->pixy, tStar->match->starId,
                    tStar->match->pixx, tStar->match->pixy, tStar->error);
            count++;
        }
        tStar = tStar->next;
    }
    fclose(fp);

#ifdef PRINT_CM_DETAIL
    printf("matched stars %d\n", count);
#endif
}

void CrossMatch::printMatchedRst(char *outfName, cm_star *starList, float errorBox) {

    FILE *fp = fopen(outfName, "w");
    fprintf(fp, "Id\tX\tY\tmId\tmX\tmY\tdistance\n");

    long count = 0;
    cm_star *tStar = starList;
    while (NULL != tStar) {
        if (NULL != tStar->match && tStar->error < errorBox) {
            fprintf(fp, "%8ld %12f %12f %8ld %12f %12f %12f\n",
                    tStar->starId, tStar->pixx, tStar->pixy, tStar->match->starId,
                    tStar->match->pixx, tStar->match->pixy, tStar->error);
            count++;
        }
        tStar = tStar->next;
    }
    fclose(fp);

#ifdef PRINT_CM_DETAIL
    printf("matched stars %d\n", count);
#endif
}

void CrossMatch::printAllStarList(char *outfName, cm_star *starList, float errorBox) {

    FILE *fp = fopen(outfName, "w");
    fprintf(fp, "Id\tX\tY\tmId\tmX\tmY\tdistance\n");

    long count = 0;
    cm_star *tStar = starList;
    while (NULL != tStar) {
        if (NULL != tStar->match) {
            fprintf(fp, "%8ld %12f %12f %8ld %12f %12f %12f\n",
                    tStar->starId, tStar->pixx, tStar->pixy, tStar->match->starId,
                    tStar->match->pixx, tStar->match->pixy, tStar->error);
        } else {
            fprintf(fp, "%8ld %12f %12f %8d %12f %12f %12f\n",
                    tStar->starId, tStar->pixx, tStar->pixy, 0, 0.0, 0.0, tStar->error);
        }
        count++;
        tStar = tStar->next;
    }
    fclose(fp);

#ifdef PRINT_CM_DETAIL
    printf("matched stars %d\n", count);
#endif
}

void CrossMatch::printOTStar(char *outfName, float errorBox) {

    FILE *fp = fopen(outfName, "w");
    fprintf(fp, "Id\tX\tY\n");

    long count = 0;
    cm_star *tStar = objStarList;
    while (NULL != tStar) {
        if (NULL == tStar->match) {
            fprintf(fp, "%8ld %12f %12f\n",
                    tStar->starId, tStar->pixx, tStar->pixy);
            count++;
        }
        tStar = tStar->next;
    }
    fclose(fp);

#ifdef PRINT_CM_DETAIL
    printf("OT stars %d\n", count);
#endif
}