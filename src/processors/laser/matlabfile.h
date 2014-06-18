/* 
 * File:   matlabfile.h
 * Author: victor
 *
 * Created on 15 de marzo de 2014, 08:33 PM
 */

#ifndef MATLABFILE_H
#define	MATLABFILE_H
#include "polar_matching/polar_matching/polar_match.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
void cabezera_matlab(const char* directorio, long timestamp, long timestamp_old, const char* time, const char* archivo, const char* archivo_old);
void final_matlab(
        int timestamp,
        const char* archivo,
        const char* time,
        PMScan *segundo,
        float x,
        float y);


#endif	/* MATLABFILE_H */

