/************************************************************************************
 * FILE: init-cda.cpp, v.1.0.0, 07/30/2009
 * Authors: Tomas Arredondo, Gabriel Cisternas, Linus Casassa.
 *
 * IRMA-2: Init CDA Process
 *
 * Common Data Area creator for IRMA-2 Proyect
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
************************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo, Gabriel Cisternas, Linus Casassa
*		<tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/

#include "init-cda.h"
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    CDA cda(true);
    int i;

    ExecutiveData *pTemp;

    fprintf(stdout, "\nIRMA2: Init CDA Process \n");

    pTemp = (ExecutiveData *)cda.getMemPtr(EXECUTIVE_AREA);

    fprintf(stdout, "init: Load all the modules you wish to use.\n");
    fprintf(stdout, "init: When done, press 'ENTER' key to continue...\n");
    getchar();

    pTemp->ctrl.loop = run_cmd;

    do
    {
        sleep(1);

    }
    while (cda.getAttchdProcesses()>1);

    fprintf(stdout, "\nIRMA2: Init CDA Process Exit\n");
    return 0;
}
