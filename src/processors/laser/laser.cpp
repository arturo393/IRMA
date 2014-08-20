//* **********************************************************************
// * IRMA2: Laser Process Header
//* **********************************************************************/
//* ******************************************************************
// Title: laser.cpp
// Authors: Tomas Arredondo, Victor Magaña
// Version: v.0.0.1
// Date: 09/15/2009
//
// First aproach of the laser used on IRMA-2 Project
//
// Version: v.0.0.2
// Copyright (C) 2013 by Victor Magaña
// Contact: vmagana@gmail.com
// Date: Agosto 2013
//
// DISCLAIMER:
// No liability is assumed by the author for any use made of this program.
//
// DISTRIBUTION:
// Any use may be made of this program, as long as the clear
// acknowledgment is made to the author in code and runtime executables
//
//*******************************************************************
/******************************************************************************
 *
 *   Copyright (c) 2009 Tomas Arredondo, Víctor Magaña
 *		<tomas.arredondo@usm.cl>
 *
 *   This program is open-source software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License version 3 as
 *   published by the Free Software Foundation.
 *
 *   See README and LICENSE for more details.
 *
 ******************************************************************************/

//*******************************************************************
// Library Declaration
#include "laser.h"
//#define PLOT_ALL

using namespace cimg_library;

//Guarda las Ultimas 1000 poses para psm, modelo (cinematico) y ekf
    
Pose poses_psm_laser[1000];
Pose poses_psm[1000];
Pose poses_modelo[1000];
Pose poses_modelo_laser[1000];
Pose poses_ekf[1000];
Pose poses_ekf_laser[1000];
//Posiciones Iniciales
double temprx = 0.0;
double tempry = 0.0;
double tempth = 0.0;

CImgDisplay disp;
CImgDisplay disp_model;
CImgDisplay disp_psm;
CImg<unsigned char> img_EKF_blanco(IMG_ANCHO, IMG_ALTO, 1, 4, 0);
CImg<unsigned char> img_EKF(IMG_ANCHO, IMG_ALTO, 1, 4, 0); 
CImg<unsigned char> img(IMG_ANCHO, IMG_ALTO, 1, 4, 0); 
CImg<unsigned char> img_modelo(IMG_ANCHO, IMG_ALTO, 1, 4, 0);
struct ekf ekf_valores;
int LaserProcessor::init() {
    printf("Laser Area\n");
    pCDAExecutive = (ExecutiveData *) cda.getMemPtr(EXECUTIVE_AREA);
    pCDALaser = (LaserData *) cda.getMemPtr(LASER_AREA);
    img_EKF_blanco.fill(127); 
    img_EKF.fill(127); 
    img.fill(127); 
    img_modelo.fill(127);
    
    pCtrl = &(pCDALaser->ctrl); // Todos los procesadores tienen que definir pCtrl
    sem_area = LASER_AREA; // Todos los procesadores tienen que definir sem_area
    pCDALaser->sensar = true;
    pCDALaser->img_alto = IMG_ALTO;
    pCDALaser->img_ancho = IMG_ANCHO;
    pCtrl->loop = run_cmd;
    printf("Sensar %s\n", (pCDALaser->sensar) ? "SI" : "NO");
    //cda.lockArea(EXECUTIVE_AREA);
    //pCDAExecutive->exec_move_ready_flag = true;
    //cda.unlockArea(EXECUTIVE_AREA);
    // Change the port name appropriately
#ifdef WINDOWS_OS
    const char device[] = "COM3";
#else
    const char device[] = "/dev/ttyACM0";
#endif
    ////// Iicialización de PSM
    pm_init();
    ////// Inicialización de EKF
    ekf_init(&ekf_valores);
	////// Inicialización de Poses
    init_poses(
            poses_psm_laser,
            poses_psm,
            poses_modelo,
            poses_modelo_laser,
            poses_ekf,
            poses_ekf_laser,
            temprx,
            tempry,
            tempth
            );	
    fprintf(stdout, "Devise:%s\n", device);
    if (!urg.connect(device)) {
        fprintf(stdout, "UrgDevice::connect: %s\n", urg.what());
        exit(1);
    }

    return (0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

int LaserProcessor::cleanup() {
    return (0);
}

enum {
    CaptureTimes = 1
};
//-------------------------------------------------------------------
//-------------------------------------------------------------------
struct stat st = {0};

timeval ultimo;


char filename[100];
char filename_old[100];
FILE *pFileMatlab;
int timestamp_old = 0;

PMScan *tercero;
PM_TYPE err_idx;

int LaserProcessor::step() {

    
    bool sensar = pCDALaser -> sensar;
    int estado = pCDALaser -> estado;
    timeval actual;

    if (sensar) {
        gettimeofday(&ultimo, NULL);
        cda.lockArea(LASER_AREA);

        long timestamp = 0;
        vector<long> data;
        /**********************************/
        /*****GENERAR DIRECTORIO CON FECHA*/
        char outstr_folder[200];
        char outstr_file[200];

        time_t t;

        struct timeval tv;
        struct tm *tmp;
        gettimeofday(&tv, NULL);
        t = tv.tv_sec;

        tmp = localtime(&t);
        if (tmp == NULL) {
            perror("localtime");
            exit(EXIT_FAILURE);
        }
        if (strftime(outstr_folder, sizeof (outstr_folder), "%Y%2m%2d", tmp) == 0) {
            fprintf(stderr, "strftime returned 0");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "Directorio:%s\n", outstr_folder);
        if (strftime(outstr_file, sizeof (outstr_file), "%H%M%S", tmp) == 0) {
            fprintf(stderr, "strftime returned 0");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "Archivo:%s\n", outstr_file);
        if (stat(outstr_folder, &st) == -1) {
            mkdir(outstr_folder, 0700);
        }

        /***********************************/
        // Obtiene la información desde el laser

        int n = urg.capture(data, &timestamp);

        // Guarda el tiempo de medición en la memoria compartida de Laser        
        pCDALaser->timestamp = timestamp;

        fprintf(stdout, "Archivo:%06d\n", (int) tv.tv_usec);
        sprintf(filename, "%s/%s_%06d", outstr_folder, outstr_file, (int) tv.tv_usec);

        FILE * pFile;
        pFile = fopen(filename, "w");

        fprintf(stdout, "X:%f\nY:%f\nD:%f\n", pCDALaser->x, pCDALaser->y, pCDALaser->dir);

        fprintf(pFile, "vd:\t%d\n", pCDALaser->vr);
        fprintf(pFile, "vi:\t%d\n", pCDALaser->vl);

//Copia la información de la medición en la estructura utilizada por PSM
        for (int j = LASER_START; j < n; ++j) {
            if (j <= LASER_END && j >= LASER_START) {

                pCDALaser->read_data[j] = data[j];
                if (primera_medicion) {
                    primero->r[j - LASER_START] = data[j] / 10;
                    segundo->r[j - LASER_START] = primero->r[j - LASER_START];
                } else
                    segundo->r[j - LASER_START] = data[j] / 10;
                fprintf(pFile, "%ld\n", data[j]);
            }           
        }
		//Si es la primera medición realiza el preproceso de la medición, necesaria para PSM
        if (primera_medicion) {
            pm_preprocessScan(primero);
            primera_medicion = false;
        }        
		//Inicializa la posición actual en el origen
        primero->rx = 0;
        primero->ry = 0;
        primero->th = 0;
		//Realiza el preproceso de la medición actual
        pm_preprocessScan(segundo);
        bool matchFailed = false;
		
		float vr = pCDALaser->vr;
		float vl = pCDALaser->vl;
		//Copia la pose estimada en la medición actul para optimizar PSM
		//vr = Porcentaje de Velocidad Rueda Derecha
		//vi = Porcentaje de Velocidad Rueda Izquierda
		//0.25 = Segundos que dura el movimiento
		//33 = Distancia entre ejes
		//Segundo = medición actual
		get_new_pose(get_vd(vr),get_vi(vr),0.25,33,segundo);
//#define IMG_SCAN        
#ifdef IMG_SCAN    			
        if (n_pose > 0) {
			//Genera Scan en base a Grilla generada por EKF (img_EKF)
			set_img_scan(img_scan, poses_ekf_laser[n_pose], img_EKF, primero);
			try {
                img_scan->rx = 0;
                img_scan->th = 0;
                img_scan->ry = 0;
                pm_preprocessScan(img_scan);
                pm_preprocessScan(segundo);
                pm_psm(img_scan, segundo, NULL);
                err_idx = pm_error_index2(img_scan, segundo);
                cout << " % err(img): " << err_idx << "\n";
            } catch (int err) {
                cerr << "Error " << err << "caught : failed match." << endl;
                matchFailed = true;
            }
        }
		else {
#endif		
		try {

                primero->rx = 0;
                primero->th = 0;
                primero->ry = 0;

                pm_preprocessScan(primero);
                pm_preprocessScan(segundo);
                pm_psm(primero, segundo, NULL);

            } catch (int err) {
                cerr << "Error " << err << "caught : failed match." << endl;
                matchFailed = true;
            }
            err_idx = pm_error_index2(primero, segundo);
            cout << " % err: " << err_idx << "\n";
            if (matchFailed || err_idx > 2) {
                try {
                    Pose temp;

                    temp.x = segundo->rx;
                    temp.y = segundo->ry;
                    temp.th = segundo->th;
                    //fprintf(stdout, "Need Switching reference scan.\n");
                    Pose origen;
                    origen.x = 0;
                    origen.y = 0;
                    origen.th = 0;
                    Pose new_pose = get_new_pose_model(-get_vd(vr), -get_vi(vl), 0.25, L_EJE, origen);
                    segundo->rx = 0;
                    segundo->ry = 0;
                    segundo->th = 0;
                    primero->rx = (new_pose.x - temp.x) / 2;
                    primero->ry = (new_pose.y - temp.y) / 2;
                    primero->th = (new_pose.th - temp.th) / 2;
                    pm_preprocessScan(primero);
                    pm_preprocessScan(segundo);
                    pm_psm(segundo, primero, NULL);
                    PM_TYPE err_idx_2 = pm_error_index2(segundo, primero);
                    cout << " % err 2: " << err_idx_2 << "\n";
                    if (err_idx > err_idx_2) {
                        segundo->rx = -primero->rx;
                        segundo->ry = -primero->ry;
                        segundo->th = -primero->th;
                    } else {
                        segundo->rx = temp.x;
                        segundo->ry = temp.y;
                        segundo->th = temp.th;
                        err_idx = err_idx_2;
                    }
                } catch (int err) {
                    cerr << "Error " << err << "caught : failed match." << endl;
                    matchFailed = true;
                }

            }
#ifdef IMG_SCAN
        }
#endif      
//TODO: REALIZAR TRANSFORMACIÓN ANTES DE SUMAR
        float dx = segundo->rx;
        float dy = segundo->ry;
        float dth = segundo->th;
        float xx = segundo->rx * cos(tempth) - segundo->ry * sin(tempth);
        float yy = segundo->rx * sin(tempth) + segundo->ry * cos(tempth);
        float tth = segundo->th;
        segundo->rx = xx;
        segundo->ry = yy;
        segundo->th = tth;
		
        temprx += xx;        
        tempry += yy;
        tempth += tth;
        n_pose++;
        fprintf(stdout, "XX:%f\nYY:%f\nDD:%f\n", xx, yy,tth);		
        poses_psm_laser[n_pose].x = temprx;
        poses_psm_laser[n_pose].y = tempry;
        poses_psm_laser[n_pose].th = tempth;
		
        poses_psm[n_pose].x = temprx - cos(tempth)*15.5;
        poses_psm[n_pose].y = tempry - sin(tempth)*15.5;
        poses_psm[n_pose].th = tempth;
        poses_modelo[n_pose] = get_new_pose_model(get_vd(vr), get_vi(vl), 0.25, L_EJE, poses_modelo[n_pose - 1]);

        poses_modelo_laser[n_pose].x = poses_modelo[n_pose].x + cos(poses_modelo[n_pose].th)*15.5;
        poses_modelo_laser[n_pose].y = poses_modelo[n_pose].y + sin(poses_modelo[n_pose].th)*15.5;
        poses_modelo_laser[n_pose].th = poses_modelo[n_pose].th;   
        
        ekf_set_valores(
                &ekf_valores,
                poses_ekf[n_pose - 1],
                poses_ekf_laser[n_pose - 1],
				L_EJE, get_vd(vr),
				get_vi(vl),
				dth, dx, dy);   
        ekf_pk_cpy(&ekf_valores);
        ekf_calc_1(&ekf_valores);
        ekf_calc_kk(&ekf_valores);
        ekf_calc_yk(&ekf_valores);
        ekf_calc_pk(&ekf_valores);				
		
		poses_ekf[n_pose].x = ekf_valores.x_k[0][0];
        poses_ekf[n_pose].y = ekf_valores.x_k[1][0];
        poses_ekf[n_pose].th = ekf_valores.x_k[2][0];        

        poses_ekf_laser[n_pose].x = poses_ekf[n_pose].x + cos(poses_ekf[n_pose].th)*15.5;
        poses_ekf_laser[n_pose].y = poses_ekf[n_pose].y + sin(poses_ekf[n_pose].th)*15.5;
        poses_ekf_laser[n_pose].th = poses_ekf[n_pose].th;
		//Actualiza Valor de velocidad
        set_v_histo(vr, vl, poses_ekf_laser[n_pose - 1], poses_ekf_laser[n_pose], L_EJE);

		//////// Genera Grilla de Ocupacion e Imagenes para despliegue
        set_image(segundo, poses_ekf_laser[n_pose], img_EKF);
        set_image_color(segundo, poses_ekf_laser[n_pose], img_EKF, azul);
        if (n_pose > 1)
            set_image_color_blanco(img_scan, poses_ekf_laser[n_pose - 1], img_EKF_blanco, black);
        //CImg<unsigned char> img_temp_ekf(img_EKF_blanco);     
    #ifdef PLOT_ALL
        CImg<unsigned char> img_temp_ekf(img_EKF);
        set_image_color(primero, poses_ekf_laser[n_pose - 1], img_temp_ekf, verde);
        set_image_color(img_scan, poses_ekf_laser[n_pose - 1], img_temp_ekf, rojo);
        dibujar_robot(segundo, poses_ekf_laser[n_pose], poses_ekf[n_pose], img_temp_ekf);
        dibujar_poses(poses_ekf_laser, poses_ekf, n_pose, img_temp_ekf);
        unsigned char purple[] = { 255,0,255 }; // Define a purple color
        img_temp_ekf.draw_text(50,5,"EKF",
                white,NULL,1);
        //img_temp_ekf.draw_text("EKF",5,5,black,black);
		///////////////////////////////////////////////////////////        
        //img_temp_ekf.draw_text(0,0,white,black,"PSM %d",n_pose);
        ///////////
        /////////// MODELO CINEMATICO 
        set_image(segundo, poses_modelo_laser[n_pose], img_modelo);
        CImg<unsigned char> img_temp_modelo(img_modelo);
        dibujar_robot(segundo, poses_modelo_laser[n_pose], poses_modelo[n_pose], img_temp_modelo);
        dibujar_poses(poses_modelo_laser, poses_modelo, n_pose, img_temp_modelo);
         //  img_temp_psm.draw_text(50,5,"MODELO",
         //       white,NULL,1);
        //img_temp_ekf.draw_text(0,0,white,black,"CINEMATICO %d",n_pose);
        ////img_temp.draw_line(x0, y0, x1, y1, azul);

    #endif

        //// PSM LASER
        set_image_color_blanco(segundo, poses_psm_laser[n_pose], img, black);
        set_image(segundo, poses_psm_laser[n_pose], img);
        CImg<unsigned char> img_temp_psm(img);
        dibujar_robot(segundo, poses_psm_laser[n_pose], poses_psm[n_pose], img_temp_psm);
        dibujar_poses(poses_psm_laser, poses_psm, n_pose, img_temp_psm);
       // img_temp_psm.draw_text(50,5,"PSM",
       //         white,NULL,1);

    disp_psm.display(img_temp_psm);
        
#ifdef PLOT_ALL    
        disp.display(img_temp_ekf);
        disp_model.display(img_temp_modelo);
        
#endif
        tercero = segundo;
        segundo = primero;
        primero = tercero;

        timestamp_old = tv.tv_usec;
        fclose(pFile);
        strcpy(filename_old, filename);
		//Copia información de Posición en Memoria Compartida
        pCDALaser -> x = temprx;
        pCDALaser -> y = tempry;
        pCDALaser -> dir = tempth;
        pCDALaser -> sensar = false;
        if (vl != 0 || vr != 0){
            pCDALaser -> dx = xx;
            pCDALaser -> dy = yy;
            pCDALaser -> ddir = tth;
        }
		//Copia información de la grilla de ocupación en la memoria compartida
        cimg_forXY(img_temp_psm,xx,yy){
            pCDALaser -> map[xx][yy] = img_temp_psm(xx,yy,0);
        }                
        cda.unlockArea(LASER_AREA);
    }
    return (0);
}

//*******************************************************************
// LASER PROCESSOR: LaserProcessor Class Implementation: END
//*******************************************************************

//-------------------------------------------------------------------
//-------------------------------------------------------------------


int main() {
    fprintf(stdout, "\nIRMA2: Laser Process \n");
    LaserProcessor laser;        
    disp.display(img_EKF_blanco);
#ifdef PLOT_ALL    
    disp_model.display(img_modelo);
    disp_psm.display(img);
#endif
    laser.execute();    

    return (0);
}

