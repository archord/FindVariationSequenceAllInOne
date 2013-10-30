/* 
 * File:   FindVariationSequence.cpp
 * Author: xy
 * 
 * Created on October 21, 2013, 10:11 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "fitsio.h"
#include "FindVariationSequence.h"
#include "utils.h"
#include "CircleList.h"

FindVariationSequence::FindVariationSequence() {
}

FindVariationSequence::FindVariationSequence(const FindVariationSequence& orig) {
}

FindVariationSequence::~FindVariationSequence() {
}

void FindVariationSequence::batchMatch(char *dataDir, char *reffName, char *stdfName,
        char *objListfName, float errorBox, char *vsOutDir, char *outDir2,
        int headN, float trms, float trmsRmv) {

    printf("starting match...\n");

    int i = 0;
    int stdStarNum = 0;
    int refStarNum = 0;
    char *tmpfName = (char *) malloc(sizeof (char)*MaxStringLength);
    sprintf(tmpfName, "%s/%s", dataDir, stdfName);
    cm_star *std = readStdFile(tmpfName, stdStarNum);
    sprintf(tmpfName, "%s/%s", dataDir, reffName);
    cm_star *ref = readRefFile(tmpfName, refStarNum);
    matchStandradStar(std, ref, errorBox);

    /**/
    int objFileNum = 0;
    sprintf(tmpfName, "%s/%s", dataDir, objListfName);
    char **objfNames = getAllObjFileName(tmpfName, objFileNum);

    vs_result *vsRst = (vs_result *) malloc(sizeof (vs_result) * refStarNum);
    cm_star *nextStar = ref;
    int objIdx = 0;
    while (nextStar) {
        vsRst[objIdx].refX = nextStar->pixx;
        vsRst[objIdx].refY = nextStar->pixy;
        vsRst[objIdx].refmag = nextStar->mag;
        vsRst[objIdx].stdX = nextStar->match->pixx;
        vsRst[objIdx].stdY = nextStar->match->pixy;
        vsRst[objIdx].stdmag = nextStar->match->mag;
        vsRst[objIdx].rcdlst = (match_mag_rcd *) malloc(sizeof (match_mag_rcd) * objFileNum);
        memset(vsRst[objIdx].rcdlst, 0, sizeof (match_mag_rcd) * objFileNum);
        vsRst[objIdx].rcdNum = 0;
        objIdx++;
        nextStar = nextStar->next;
    }

    float maxMem = (sizeof (cm_star) * stdStarNum + sizeof (cm_star) * refStarNum +
            sizeof (cm_star) * refStarNum + sizeof (vs_result) * refStarNum +
            30 * objFileNum + sizeof (match_mag_rcd) * refStarNum * objFileNum) / (1024 * 1024.0);
    float maxMem1K = (sizeof (cm_star) * stdStarNum + sizeof (cm_star) * refStarNum +
            sizeof (cm_star) * refStarNum + sizeof (vs_result) * refStarNum +
            30 * 1000 + sizeof (match_mag_rcd) * refStarNum * 1000) / (1024 * 1024.0);
    printf("%d object file, need memory %.3fMB\n", objFileNum, maxMem);
    printf("if 1000 object file, need memory %.3fMB\n", maxMem1K);

    float minZoneLen = errorBox * TimesOfErrorRadius;
    float searchRds = errorBox;
    Partition *refZones = new Partition(errorBox, minZoneLen, searchRds);
    Partition *objZones = new Partition(errorBox, minZoneLen, searchRds);
    refZones->partitonStarField(ref, refStarNum);
    ref = NULL;

    int tap = 0;
    if (objFileNum < 40)
        tap = 2;
    else if (objFileNum < 100) {
        tap = 5;
    } else {
        tap = objFileNum / 20;
    }

    for (i = 0; i < objFileNum; i++) {
        int objStarNum = 0;
        char fitsName[MaxStringLength];
        int fitsNameIdx = strchr(objfNames[i], '.') - objfNames[i] + 4;
        strncpy(fitsName, objfNames[i], fitsNameIdx);
        fitsName[fitsNameIdx] = '\0';
        sprintf(tmpfName, "%s/%s", dataDir, fitsName);
        double time = getImageTime(tmpfName);

        sprintf(tmpfName, "%s/%s", dataDir, objfNames[i]);
        cm_star *obj = readObjFile(tmpfName, objStarNum);
        cm_star *obj2 = copyStarList(obj);
        objZones->partitonStarField(obj2, refStarNum);
        obj2 = NULL;
        nextStar = obj;
        while (nextStar) {
            refZones->getMatchStar(nextStar);
            nextStar = nextStar->next;
        }
        /*print match result or OT*/

        nextStar = obj;
        while (nextStar) {
            if (NULL != nextStar->match && nextStar->mag < MAX_MAG) {
                cm_star *tRefStar = nextStar->match;
                cm_star *tStdStar = tRefStar->match;
                objZones->getMatchStar(tStdStar);
                if (NULL != tStdStar->match) {
                    long curMatchCnt = vsRst[tRefStar->id].rcdNum;
                    match_mag_rcd *curMatchPeer = vsRst[tRefStar->id].rcdlst;
                    curMatchPeer[curMatchCnt].time = time;
                    curMatchPeer[curMatchCnt].refMchMag = nextStar->mag;
                    curMatchPeer[curMatchCnt].refMchMagErr = nextStar->mage;
                    curMatchPeer[curMatchCnt].stdMchMag = tStdStar->match->mag;
                    curMatchPeer[curMatchCnt].stdMchMagErr = tStdStar->match->mage;
                    vsRst[nextStar->match->id].rcdNum++;
                }
            }
            nextStar = nextStar->next;
        }
        freeStarList(obj);
        objZones->freeZoneArray();

        if ((i < 3) || (i % tap == 0) || (i == objFileNum - 1))
            printf("done %dth star list\n", i + 1);
    }
    refZones->freeZoneArray();

    printf("match All file done, starting write result to file...\n");

    int newFileCount = 0;
    int breakfile = 0;
    char *outfName = (char *) malloc(sizeof (char)*MaxStringLength);
    char *outfName2 = (char *) malloc(sizeof (char)*MaxStringLength);
    for (i = 0; i < refStarNum; i++) {
        if (0 != vsRst[i].rcdNum) {
            long curMatchCnt = vsRst[i].rcdNum;
            match_mag_rcd *curMatchPeer = vsRst[i].rcdlst;
            int j;
            for (j = 0; j < curMatchCnt; j++) {
                //correct the object star
                curMatchPeer[j].refMchCorrect = curMatchPeer[j].refMchMag +
                        vsRst[i].stdmag - curMatchPeer[j].stdMchMag;
            }

            //calculate the prev N avg and rms
            //寻找突变事件，出现连续的多个mag值，大于前N个mag值的（K倍的rms加avg）
            //avg的计算需要过滤单独的mag值，大于前N个mag值的（K倍的rms加avg）
            CircleList *clst = new CircleList(headN);
            for (j = 0; j < curMatchCnt - 1; j++) {
                if (j >= headN) {
                    curMatchPeer[j].aheadNAvg = clst->getAvg();
                    curMatchPeer[j].aheadNRms = clst->getRms();
                }
                if (j < headN) {
                    clst->add(curMatchPeer[j].refMchCorrect);
                } else {
                    float cmpValue = curMatchPeer[j].aheadNAvg +
                            trmsRmv * curMatchPeer[j].aheadNRms;
                    //过滤单个峰值
                    if (!((curMatchPeer[j - 1].refMchCorrect < cmpValue) &&
                            (curMatchPeer[j].refMchCorrect >= cmpValue) &&
                            (curMatchPeer[j + 1].refMchCorrect < cmpValue))) {
                        clst->add(curMatchPeer[j].refMchCorrect);
                    }
                }
            }
            curMatchPeer[curMatchCnt - 1].aheadNAvg = clst->getAvg();
            curMatchPeer[curMatchCnt - 1].aheadNRms = clst->getRms();

#ifdef WITH_VS
            sprintf(outfName, "%s/%s_%f_%f_%.3f.cat", vsOutDir, OUTFILE_PREFIX,
                    vsRst[i].refX, vsRst[i].refY, vsRst[i].refmag);
            FILE *fp = fopen(outfName, "w");
            fprintf(fp, "#reference star x\n");
            fprintf(fp, "#reference star y\n");
            fprintf(fp, "#reference star mag\n");
            fprintf(fp, "#compare star x\n");
            fprintf(fp, "#compare star y\n");
            fprintf(fp, "#compare star mag\n");
            fprintf(fp, "%f\n", vsRst[i].refX);
            fprintf(fp, "%f\n", vsRst[i].refY);
            fprintf(fp, "%f\n", vsRst[i].refmag);
            fprintf(fp, "%f\n", vsRst[i].stdX);
            fprintf(fp, "%f\n", vsRst[i].stdY);
            fprintf(fp, "%f\n", vsRst[i].stdmag);
            fprintf(fp, "\n");
            fprintf(fp, "#total star: %ld\n", vsRst[i].rcdNum);
            fprintf(fp, "#time objectMag objectMagErr compareMag "
                    "compareMagErr objCrtMag aheadNavg aheadNrms\n");

            for (j = 0; j < curMatchCnt; j++) {
                fprintf(fp, "%.8f %f %f %f %f %f %f %f\n", curMatchPeer[j].time,
                        curMatchPeer[j].refMchMag, curMatchPeer[j].refMchMagErr,
                        curMatchPeer[j].stdMchMag, curMatchPeer[j].refMchMagErr,
                        curMatchPeer[j].refMchCorrect,
                        curMatchPeer[j].aheadNAvg, curMatchPeer[j].aheadNRms);
            }
            fclose(fp);
#endif
            int breakflag = 0;
            if (curMatchCnt > headN) {
                FILE *fp2 = NULL;
                sprintf(outfName2, "%s/%s_%f_%f_%.3f.cat.break", outDir2,
                        OUTFILE_PREFIX, vsRst[i].refX, vsRst[i].refY, vsRst[i].refmag);
                for (j = 0; j < curMatchCnt; j++) {
                    if (j >= headN) {
                        if (curMatchPeer[j].refMchCorrect > curMatchPeer[j].aheadNAvg +
                                trms * curMatchPeer[j].aheadNRms) {
                            breakflag++;
                        } else {
                            breakflag = 0;
                        }
                        //连续多个大于avg+k*rms
                        if (breakflag == 2) {
                            if (NULL == fp2) {
                                breakfile++;
                                fp2 = fopen(outfName2, "w");
                                fprintf(fp2, "#time objectMag objectMagErr compareMag "
                                        "compareMagErr objCrtMag aheadNavg aheadNrms\n");
                            }
                            fprintf(fp2, "%.8f %f %f %f %f %f %f %f\n", curMatchPeer[j - 1].time,
                                    curMatchPeer[j - 1].refMchMag, curMatchPeer[j - 1].refMchMagErr,
                                    curMatchPeer[j - 1].stdMchMag, curMatchPeer[j - 1].refMchMagErr,
                                    curMatchPeer[j - 1].refMchCorrect,
                                    curMatchPeer[j - 1].aheadNAvg, curMatchPeer[j - 1].aheadNRms);
                            fprintf(fp2, "%.8f %f %f %f %f %f %f %f\n", curMatchPeer[j].time,
                                    curMatchPeer[j].refMchMag, curMatchPeer[j].refMchMagErr,
                                    curMatchPeer[j].stdMchMag, curMatchPeer[j].refMchMagErr,
                                    curMatchPeer[j].refMchCorrect,
                                    curMatchPeer[j].aheadNAvg, curMatchPeer[j].aheadNRms);
                        } else if (breakflag > 2) {
                            fprintf(fp2, "%.8f %f %f %f %f %f %f %f\n", curMatchPeer[j].time,
                                    curMatchPeer[j].refMchMag, curMatchPeer[j].refMchMagErr,
                                    curMatchPeer[j].stdMchMag, curMatchPeer[j].refMchMagErr,
                                    curMatchPeer[j].refMchCorrect,
                                    curMatchPeer[j].aheadNAvg, curMatchPeer[j].aheadNRms);
                        }
                    }
                }
                if (NULL != fp2) {
                    fclose(fp2);
                }
            }
            newFileCount++;
            clst->freeList();
        }
    }
    printf("total generate %d variation sequence file\n", newFileCount);
    printf("total generate %d break file\n", breakfile);

    free(outfName);
    free(outfName2);
    freeAllFileName(objfNames, objFileNum);

    for (i = 0; i < refStarNum; i++) {
        free(vsRst[i].rcdlst);
    }
    free(vsRst);

    freeStarList(std);
    freeStarList(ref);
    printf("write file done\n");

}

char **FindVariationSequence::getAllObjFileName(char *fName, int &fileNum) {

    fileNum = countFileLines(fName);

    FILE *fp = fopen(fName, "r");

    if (fp == NULL) {
        return NULL;
    }

    char **fNames = (char **) malloc(sizeof (char*)*fileNum);
    char line[MaxStringLength];
    int i = 0;
    while (fscanf(fp, "%s", line) != EOF) {
        if (0 != strlen(line)) {
            fNames[i] = (char*) malloc(sizeof (char)*(strlen(line) + 1));
            strcpy(fNames[i], line);
            i++;
        }
    }
    return fNames;
}

void FindVariationSequence::freeAllFileName(char **objfNames, int fileNum) {
    int i;
    for (i = 0; i < fileNum; i++) {
        free(objfNames[i]);
    }
    free(objfNames);
}

void FindVariationSequence::matchStandradStar(cm_star *ref, cm_star *obj, float errorBox) {

    cm_star *tObj = obj;

    while (tObj) {
        float tError = 0.0;
        float minError = 0.0;
        cm_star *tRef = ref;
        while (tRef) {
            float xdiff = fabs(tRef->pixx - tObj->pixx);
            if (xdiff > DiffStarDistance) {
                minError = getLineDistance(tRef, tObj);
                tObj->match = tRef;
                tObj->error = minError;
                tRef = tRef->next;
                break;
            }
            tRef = tRef->next;
        }
        while (tRef) {
            float xdiff = fabs(tRef->pixx - tObj->pixx);
            if (xdiff > DiffStarDistance) {
                tError = getLineDistance(tRef, tObj);
                if (tError < minError) {
                    minError = tError;
                    tObj->match = tRef;
                    tObj->error = tError;
                }
            }
            tRef = tRef->next;
        }
        tObj = tObj->next;
    }
}

cm_star *FindVariationSequence::readObjFile(char *fName, int &starNum) {

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
        nextStar->next = NULL;
        sscanf(line, "%f%f%*f%*f%*f%*f%f%f", &nextStar->pixx, &nextStar->pixy, &nextStar->mag, &nextStar->mage);
        starList = nextStar;
        tStar = nextStar;
        starNum++;
    }

    while (fgets(line, MaxStringLength, fp) != NULL) {
        nextStar = (cm_star *) malloc(sizeof (cm_star));
        nextStar->next = NULL;
        sscanf(line, "%f%f%*f%*f%*f%*f%f%f", &nextStar->pixx, &nextStar->pixy, &nextStar->mag, &nextStar->mage);
        tStar->next = nextStar;
        tStar = nextStar;
        starNum++;
    }

#ifdef PRINT_CM_DETAIL
    printf("%s\t%d stars\n", fName, starNum);
#endif
    return starList;
}

cm_star *FindVariationSequence::readRefFile(char *fName, int &starNum) {

    return readStarFile(fName, starNum);
}

cm_star *FindVariationSequence::readStdFile(char *fName, int &starNum) {

    return readStarFile(fName, starNum);
}

double FindVariationSequence::getImageTime(char *fitsName) {

    char *starTimeStr = "DATE-OBS";
    char *expTimeStr = "EXPTIME";
    char jdStr[128];
    float expTime = 0.0;

    fitsfile *fptr; /* pointer to the FITS file, defined in fitsio.h */
    int status = 0;

    if (fits_open_file(&fptr, fitsName, READONLY, &status)) {
        printf("Open file :%s error!\n", fitsName);
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
    }

    /* read the NAXIS1 and NAXIS2 keyword to get table size */
    if (fits_read_key_str(fptr, starTimeStr, jdStr, NULL, &status)) {
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
    }

    /* read the NAXIS1 and NAXIS2 keyword to get table size */
    if (fits_read_key_flt(fptr, expTimeStr, &expTime, NULL, &status)) {
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
    }

    if (fits_close_file(fptr, &status)) {
        if (status) {
            fits_report_error(stderr, status); /* print error report */
            exit(status); /* terminate the program, returning error status */
        }
    }

    double ddate = calJulianDay(jdStr);

    return ddate + expTime / (2 * 86400.0);
}