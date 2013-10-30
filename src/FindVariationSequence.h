/* 
 * File:   FindVariationSequence.h
 * Author: xy
 *
 * Created on October 21, 2013, 10:11 AM
 */

#ifndef FINDVARIATIONSEQUENCE_H
#define	FINDVARIATIONSEQUENCE_H

#include "CrossMatch.h"

typedef struct MATCH_MAG_RECORD {
    double time;
    float refMchMag;
    float refMchMagErr;
    float stdMchMag;
    float stdMchMagErr;
    float refMchCorrect;
    float aheadNAvg;
    float aheadNRms;
} match_mag_rcd;


typedef struct VS_RESULT {
    float refX;
    float refY;
    float refmag;
    float stdX;
    float stdY;
    float stdmag;
    match_mag_rcd *rcdlst;
    long rcdNum;
} vs_result;

#define MAX_MAG 30
#define OUTFILE_PREFIX "glc"
#define WITH_VS

class FindVariationSequence : CrossMatch {
public:
    FindVariationSequence();
    FindVariationSequence(const FindVariationSequence& orig);
    virtual ~FindVariationSequence();
    cm_star *readObjFile(char *fName, int &starNum);
    cm_star *readRefFile(char *fName, int &starNum);
    cm_star *readStdFile(char *fName, int &starNum);

    void batchMatch(char *dataDir, char *reffName, char *stdfName,
        char *objListfName, float errorBox, char *vsOutDir, char *outDir2,
        int headN, float trms, float trmsRmv);
    double getImageTime(char *fitsName);
    
private:
    static const int DiffStarDistance = 3;
    static const int DiffStarDistance1 = 3;

    char **getAllObjFileName(char *fName, int &fileNum);
    char **getAllObjFileName(char *fName);
    void freeAllFileName(char **objfNames, int fileNum);
    void matchStandradStar(cm_star *ref, cm_star *obj, float errorBox);
};

#endif	/* FINDVARIATIONSEQUENCE_H */

