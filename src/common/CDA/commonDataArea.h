/***********************************************************************
 * IRMA2: CDA Shared Mem Header
 ************************************************************************/
/******************************************************************************
 *
 *   Copyright (c) 2009 Tomas Arredondo     <tomas.arredondo@usm.cl>
 *
 *   This program is open-source software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License version 3 as
 *   published by the Free Software Foundation.
 *
 *   See README and LICENSE for more details.
 *
 ******************************************************************************/
#ifndef _IRMA2_COMMON_DATA_AREA_H_
#define _IRMA2_COMMON_DATA_AREA_H_

#include <sys/shm.h>
#include "../irmaErrors.h"
#include "../process.h"
#include "../semaphores.h"
#include "closeNavDataArea.h"
#include "longNavDataArea.h"
#include "featureNavDataArea.h"
#include "executiveDataArea.h"
#include "mapperDataArea.h"
#include "monitorDataArea.h"
#include "networkDataArea.h"
#include "cognitiveDataArea.h"
#include "numCogDataArea.h"
#include "camBlobDataArea.h"
#include "laserDataArea.h"

class CDA {
private:
    bool m_creator;
    int nShmId; // ID of the shared memory segment.
    char* pShmAddr[NUM_PROCESS_AREAS]; // address of shared memory segment.
    struct shmid_ds pShmDesc;
    Semaphores *sem;
    key_t key;

public:
    CDA(bool creator = false); //Constructor
    ~CDA(); //Destructor
    char *getMemPtr(enum_process_areas i);
    unsigned short getAttchdProcesses();

    int lockArea(enum_process_areas i, enum_sem_areas area = data_1);
    int unlockArea(enum_process_areas i, enum_sem_areas area = data_1);
};

// Pointers NOT ALLOWED in this structure!
// Do not CHANGE the ORDER of this structure!

struct CommonDataArea {
    struct CloseNavData closeNav;
    struct LongNavData longNav;
    struct FeatureNavData featureNav;
    struct ExecutiveData exec;
    struct MapperData mapper;
    struct MonitorData mon;
    struct NetworkData netNav;
    struct ObjCognitiveData objCog;
    struct NumCogData numCog;
    struct CamBlobData camBlob;
    struct LaserData laser;
};

CDA::CDA(bool creator) {
    char i;
    m_creator = creator;
    sem = new Semaphores(creator);
    key = 100;
    if (creator == true) {
        // read shared memory segment with size of CommonDataArea
        nShmId = shmget(key, sizeof (CommonDataArea), IPC_CREAT | IPC_EXCL | 0766);
        if (nShmId == -1) {
            fprintf(stdout, "commonDataArea CDA::CDA creator shmget error\n");

            fprintf(stdout, "Do you want to recover the Common Data Area? (y/n)\n");
            i = getchar();
            getchar();

            if (i == 'y' || i == 'Y') {
                //liberates the memory in case it was already taken
                nShmId = shmget(key, sizeof (CommonDataArea), 0766);
                pShmAddr[0] = (char *) shmat(nShmId, NULL, 0);
                shmdt(pShmAddr[0]);
                shmctl(nShmId, IPC_RMID, &pShmDesc);
                fprintf(stdout, "Common Data Area has been recovered.\n\n");
                fprintf(stdout, "Common Data Area will be available\n");
                fprintf(stdout, "next time you run this program.\n");
            } else if (i == 'n' || i == 'N')
                fprintf(stdout, "Common Data Area has not been recovered!\n");


            else {
                fprintf(stdout, "Wrong Selection. Nothing has been done.\n\n");
                fprintf(stdout, "To recover the Common Data Area, run this program again\n");
                fprintf(stdout, "and make sure to choose the right answer.\n");

            }

            exit(IRMA2_CDA_ERR_SHMGET);
        }
    } else {
        // CommonDataArea has already been created. Only it's address is needed
        nShmId = shmget(key, sizeof (CommonDataArea), 0766);
        if (nShmId == -1) {
            fprintf(stdout, "commonDataArea CDA::CDA shmget error\n");
            exit(IRMA2_CDA_ERR_SHMGET);
        }
    }

    // attach the shared memory segment to our process's address space.
    pShmAddr[0] = (char *) shmat(nShmId, NULL, 0); // pointer to closeNav
    if (!pShmAddr[0]) // operation failed.
    {
        fprintf(stdout, "commonDataArea CDA::CDA shmat error\n");
        exit(IRMA2_CDA_ERR_SHMAT);
    }

    // For all areas set the index for each area on the shared memory segment.
    pShmAddr[1] = (pShmAddr[0] + sizeof (CloseNavData)); // pointer to longNav
    pShmAddr[2] = (pShmAddr[1] + sizeof (LongNavData)); // pointer to featureNav
    pShmAddr[3] = (pShmAddr[2] + sizeof (FeatureNavData)); // pointer to exec
    pShmAddr[4] = (pShmAddr[3] + sizeof (ExecutiveData)); // pointer to mapper
    pShmAddr[5] = (pShmAddr[4] + sizeof (MapperData)); // pointer to mon
    pShmAddr[6] = (pShmAddr[5] + sizeof (MonitorData)); // pointer to netNav
    pShmAddr[7] = (pShmAddr[6] + sizeof (NetworkData)); // pointer to objCog
    pShmAddr[8] = (pShmAddr[7] + sizeof (ObjCognitiveData)); // pointer to numCog
    pShmAddr[9] = (pShmAddr[8] + sizeof (NumCogData)); // pointer to camBlobL
    pShmAddr[10] = (pShmAddr[9] + sizeof (CamBlobData)); // pointer to laser


}

CDA::~CDA() {
    delete sem;
    printf("CDA Destroy\n");

    // detach the shared memory segment from our process's address space.
    if (shmdt(pShmAddr[0]) == -1) {
        fprintf(stdout, "commonDataArea CDA::~CDA shmdt error\n");
        exit(IRMA2_CDA_ERR_SHMDT);
    }

    // de-allocate the shared memory segment.
    if (m_creator == true) {
        if (shmctl(nShmId, IPC_RMID, &pShmDesc) == -1) {
            fprintf(stdout, "commonDataArea CDA::~CDA shmctl error\n");
        }
    }

}

char *CDA::getMemPtr(enum_process_areas i) {
    return pShmAddr[i];
}

int CDA::lockArea(enum_process_areas i, enum_sem_areas area) {
    sem->lock(i, area);
    return (IRMA2_OK);
}

int CDA::unlockArea(enum_process_areas i, enum_sem_areas area) {
    sem->unlock(i, area);
    return (IRMA2_OK);
}

unsigned short CDA::getAttchdProcesses() {
    shmctl(nShmId, IPC_STAT, &pShmDesc);
    return pShmDesc.shm_nattch;

}


#endif
