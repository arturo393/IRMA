/***********************************************************************
* IRMA2: Semaphore Class Header
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
#ifndef _IRMA2_SEMAPHORES_H_
#define _IRMA2_SEMAPHORES_H_

#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

#include "process.h"

#define DUMMY 0

#define SEMKEY    250	 // Base KEY for the semaphore.

enum enum_sem_areas { control, debugger, data_1, data_2, data_3, data_4 };
#define SEM_AREAS 6

class Semaphores
{
private:
    int sem_set_id[NUM_PROCESS_AREAS];
    int initSem(enum_process_areas i);
    void destroySem(enum_process_areas i);
    bool creator;
public:
    Semaphores(bool creatorArg);
    ~Semaphores();
    int lock (enum_process_areas i, enum_sem_areas area);
    int unlock(enum_process_areas i, enum_sem_areas area);
};

int Semaphores::initSem(enum_process_areas i)
{
    
    int semid;
    printf("enum_process_areas %d\n",i);
    if (creator)
    {
        if ((semid = semget(SEMKEY + i, SEM_AREAS, IPC_CREAT | 0666)) != -1)
        {

        }
        else
        {
            printf("error semget(SEMKEY +i, SEM_AREAS...)/n");
        }
            //printf("SEMKEY+i=%d, semid:%d, errno:%d\n",SEMKEY+i,semid,errno);
        printf("semctl doing SETVAL\n");
//        int initValue[SEM_AREAS];
        for (int j=0; j < SEM_AREAS; j++)
        {
            if (semctl(semid, j, SETVAL, 1))  // existe 1 recurso
            {
                printf("error semctl(%d, %d, SETVAL, 1)\n",semid, j);
                for (int i=0; i < NUM_PROCESS_AREAS ; i++)
                {
                    destroySem((enum_process_areas) i);
                }
                exit(2);
            }
        }

    }
    else
    {
        if ((semid = semget(SEMKEY + i, SEM_AREAS, 0)) == -1)
        {
            printf("Error\n");
            printf("error semget(SEMKEY,%d,0)\n", SEM_AREAS);
            exit(1);
        }
    }



    return semid;

}

void Semaphores::destroySem(enum_process_areas i)
{
    if (semctl(sem_set_id[i], DUMMY, IPC_RMID, DUMMY)==-1)
    {
        printf("error semctl(semid, DUMMY, IPC_RMID, DUMMY)==-1)\n");
        //exit(5);
    }
}

Semaphores::Semaphores(bool creatorArg)
{
    creator = creatorArg;

    for (int i=0; i < NUM_PROCESS_AREAS ; i++)
    {
        if ((sem_set_id[i] = initSem((enum_process_areas) i)) == -1)
        {
            fprintf(stdout,"%s:%d Semaphores::initSem:%d error\n ",__FILE__,__LINE__,i);
            exit(1);
        }
    }
}

Semaphores::~Semaphores()
{
    if (creator)
    {
        for (int i=0; i < NUM_PROCESS_AREAS ; i++)
        {
            destroySem((enum_process_areas) i);
        }
    }
}

int Semaphores::lock (enum_process_areas i, enum_sem_areas area)
{
    static struct sembuf lock = {area, -1, SEM_UNDO};
    if (semop(sem_set_id[i], &lock, 1) == -1)
    {
        printf("error semop(semid, &lock, 1) == -1)\n");
        exit(3);
    }

    return IRMA2_OK;
}

int Semaphores::unlock(enum_process_areas i, enum_sem_areas area)
{
    static struct sembuf unlock = {area, 1, SEM_UNDO};

    if (semop(sem_set_id[i], &unlock,1)==-1)
    {
        printf("error semop(semid, &unlock, 1) == -1)\n");
        exit(4);
    }

    return IRMA2_OK;
}

#endif
