/* 
 * File:   main.cpp
 * Author: xy
 *
 * Created on October 21, 2013, 10:04 AM
 */

#include <cstdlib>
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "FindVariationSequence.h"
//#include "utils.h"
//#include "CircleList.h"

void printHelp();

int main(int argc, char** argv) {

    if (argc < 11) {
        printHelp();
        return 1;
    }

    float errorBox = atof(argv[5]);
    int prevN = atoi(argv[8]);
    float rmsTimes = atof(argv[9]);
    float rmsTimesRemove = atof(argv[10]);
    FindVariationSequence *varseq = new FindVariationSequence();
    varseq->batchMatch(argv[1], argv[2], argv[3], argv[4], errorBox, argv[6],
            argv[7], prevN, rmsTimes, rmsTimesRemove);
    //printf("%.8f\n", varseq->getImageTime("20130111_177d550752_60d851283-420.fit"));

//    CircleList *clst = new CircleList(5);
//    float tmp[] = {3, 4, 7, 2, 7, 4, 9, 6, 1, 8, 5, 9,6,3,5};
//    for (int i = 0; i < 15; i++) {
//        clst->add(tmp[i]);
//        printf("%f sum=%.2f avg=%.2f ", tmp[i], clst->getSum(), clst->getAvg());
//        printf("rms=%f\n", clst->getRms());
//    }

    return 0;
}

void printHelp() {
    printf("Usage:\n");
    printf("\tFindVariationSequence dataDir reffName stdfName objListfName errorBox\n"
            "\t\toutDir1 outDir2 prevN rmsTimes rmsTimesRemove\n");
    printf("\tdataDir       :data directory, include reference file, compare file, object\n"
            "\t              file name list file, all object file and object's origin fit file\n");
    printf("\treffName      :reference file name\n");
    printf("\tcmpfName      :compare  file name\n");
    printf("\tobjListfName  :object file name list file name\n");
    printf("\terrorBox      :the distance match two star\n");
    printf("\toutDir1       :star variation sequence output directory\n");
    printf("\toutDir2       :star break sequence output directory\n");
    printf("\tprevN         :use current star's previous N star to search star's break sequence\n");
    printf("\trmsTimes      :if current star's mag bigger than previous N star's {average value plus\n"
            "\t             \"rmsTimes\" rms}, this star will be regard as break star\n");
    printf("\trmsTimesRemove:\n");
    printf("\tFindVariationSequence data referance.cat standrad.cat objfilelist.cat 0.7 result result2 30 3 10\n");
}