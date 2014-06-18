/***********************************************************************
* IRMA2: Executive Process Header
/***********************************************************************/
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo, Gabriel Cisternas, Linus Casassa.
*   	<tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/
#ifndef _IRMA2_CRNAV_H_
#define _IRMA2_CRNAV_H_

#include <string>
#include <stdio.h>
#include "simfuzlib.h"
#include "../base/processor.h"

using namespace simfuzlib;

class CRNProcessor : public ProcessorInterface
{
	private:
		int test;
		CloseNavData *pCloseNav;
		ExecutiveData *pExecutive;
		float x[NUM_INPUTS];
		member_fns3_t in_fns;
		fuzzy_rules_t rl;
		mamdani_t mamdani;
		drive_t drive;

	public:
		CRNProcessor() : ProcessorInterface()
		{
			;
		}

		int init();
		int step();
		int cleanup();
		void initInputs(fuzzy_rules_t&);
		void initOutputs(fuzzy_rules_t& rules);
};

#endif
