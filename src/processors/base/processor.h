/***********************************************************************
 * IRMA2: Processor Base Class Process Header
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
#ifndef _IRMA2_PROCESSOR_H_
#define _IRMA2_PROCESSOR_H_

#include <string>
#include <stdio.h>
#include "../../common/CDA/commonDataArea.h"

//using namespace std;

class ProcessorInterface {
protected:
    struct control *pCtrl;
    CDA cda;
    enum_process_areas sem_area;
public:

    ProcessorInterface() {
        pCtrl = 0;
    }

    ~ProcessorInterface() {
    }
    virtual int init() = 0;
    virtual int step() = 0;
    virtual int cleanup() = 0;

    void execute() {

        init();

        cda.lockArea(sem_area, control);
        pCtrl->pid = getpid();
        cda.unlockArea(sem_area, control);

        int loop_cmd;
        do {
            //En la lectura no deberia traer problema el no bloquear...
            //cda.lockArea(sem_area, control);
            loop_cmd = pCtrl->loop;
            //cda.unlockArea(sem_area, control);
            switch (loop_cmd) {
                case step_cmd:
                    cda.lockArea(sem_area, control);
                    pCtrl->loop = stop_cmd;
                    cda.unlockArea(sem_area, control);
                case run_cmd:
                    step();
                    break;
                case halt_cmd:
                    break;
                case stop_cmd:
                default:
                    usleep(10000);
            }
        } while (loop_cmd != halt_cmd);

        cleanup();
    }
};

#endif
