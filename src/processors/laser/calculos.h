/* 
 * File:   calculos.h
 * Author: victor
 *
 * Created on 16 de junio de 2014, 03:38 PM
 */

#ifndef CALCULOS_H
#define	CALCULOS_H

#include "ekf.h"
#include "polar_matching/polar_matching/polar_match.h"
#include "CImg.h"
void calculos(PMScan *primero, PMScan *segundo);
void init_poses(Pose poses_laser[1000], 
        Pose poses_robot[1000], 
        Pose poses_modelo[1000],
        Pose poses_mejor[1000],
        Pose poses_mejorlaser[1000],
        Pose poses_modelo_laser[1000],
        Pose poses_ekf[1000],
        Pose poses_ekf_laser[1000],
        double rx,
        double ry,
        double th
        );


#endif	/* CALCULOS_H */

