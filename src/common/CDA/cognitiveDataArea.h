/***********************************************************************
* IRMA2: Object Cognitive CDA Shared Mem Header
************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2011 Pablo Benapres     <pbenapres@gmail.com>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/

#ifndef _OBJ_COGNITIVE_COMMON_DATA_AREA_H_
#define _OBJ_COGNITIVE_COMMON_DATA_AREA_H_

#include "controlDataArea.h"
#include "../CogMem/parameters.h"


// Pointers NOT ALLOWED in this structure!
struct ObjCognitiveData {
	struct control ctrl;     // no borrar

        unsigned char newInfo;
        int maxblobs; //maximum blobs to process from image.
	int maxresults; //maximum results to fetch from memory.

        int alpha;
        int beta;
        int gamma;
//        enum_behaviors behavior;

        //Left
        int L_nBlobs;
        int L_blobInfo[5*MAX_BLOBS];
        int L_scaledBlobs[WIDTH*HEIGHT*MAX_BLOBS*3];
//        unsigned char L_originalImg[FWIDTH*FHEIGHT*3];
//        unsigned char L_blobbedImg[FWIDTH*FHEIGHT*3];
        unsigned char L_cognitiveImg[FWIDTH*FHEIGHT*3];

        //Right
        int R_nBlobs;
        int R_blobInfo[5*MAX_BLOBS];
        int R_scaledBlobs[WIDTH*HEIGHT*MAX_BLOBS*3];
//        unsigned char R_originalImg[FWIDTH*FHEIGHT*3];
//        unsigned char R_blobbedImg[FWIDTH*FHEIGHT*3];
        unsigned char R_cognitiveImg[FWIDTH*FHEIGHT*3];

};

#endif
