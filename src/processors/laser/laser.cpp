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


using namespace cimg_library;

//Guarda las Ultimas 1000 poses
Pose poses_laser[1000];
Pose poses_robot[1000];
CImgDisplay disp;
CImg<unsigned char> img(IMG_ANCHO, IMG_ALTO, 1, 4, 0);
int LaserProcessor::init() {
    printf("Laser Area\n");
    pCDAExecutive = (ExecutiveData *) cda.getMemPtr(EXECUTIVE_AREA);
    pCDALaser = (LaserData *) cda.getMemPtr(LASER_AREA);
    img.fill(127); // Set pixel values to 0 (color : black)
    pCtrl = &(pCDALaser->ctrl); // Todos los procesadores tienen que definir pCtrl
    sem_area = LASER_AREA; // Todos los procesadores tienen que definir sem_area
    pCDALaser->sensar = true;
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
/*
vector<Geom2D::Point> get_laser_points(vector<double> r) {

    vector<Geom2D::Point> puntos;
    for (int j = 0; j < r.size(); j++) {
        Point punto;
        punto.x = cos(-M_PI + (M_PI / 360.0) * j) * r[j];
        punto.y = sin(-M_PI + (M_PI / 360.0) * j) * r[j];
        puntos.push_back(punto);
    }
    return puntos;

}*/

timeval ultimo;
double temprx = 0.0;
double tempry = 0.0;
double tempth = 0.0;

char filename[100];
char filename_old[100];
FILE *pFileMatlab;
int timestamp_old = 0;

PMScan *tercero;


int LaserProcessor::step() {

    
    bool sensar = pCDALaser -> sensar;
    int estado = pCDALaser -> estado;
    timeval actual;
    //gettimeofday(&actual, NULL);
    //long dif = (ultimo.tv_sec - actual.tv_sec)*1000 + (ultimo.tv_usec - actual.tv_usec);
    //if (sensar || dif > 150) {
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
        // Get the data

        int n = urg.capture(data, &timestamp);

        // Display
        //fprintf(stdout, "timestamp: %ld\n", timestamp);
        pCDALaser->timestamp = timestamp;

        fprintf(stdout, "Archivo:%06d\n", (int) tv.tv_usec);
        sprintf(filename, "%s/%s_%06d", outstr_folder, outstr_file, (int) tv.tv_usec);
        /*if (primera_medicion)
            cabezera_matlab(outstr_folder, tv.tv_usec, tv.tv_usec, outstr_file, filename, filename, estado);
        else
            cabezera_matlab(outstr_folder, tv.tv_usec, timestamp_old, outstr_file, filename, filename_old, estado);
        */
        FILE * pFile;
        pFile = fopen(filename, "w");

        fprintf(stdout, "X:%f\nY:%f\nD:%f\n", pCDALaser->x, pCDALaser->y, pCDALaser->dir);


        fprintf(pFile, "vR:\t%d\n", pCDALaser->vr);
        fprintf(pFile, "SL:\t%d\n", pCDALaser->vl);


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
            // The distance data that are less than urg_minDistance() are shown as
            // invalide values
            //fprintf(stdout, "%d:%ld, ", j, data[j]); // index, length [mm]
        }
        if (primera_medicion) {
            pm_preprocessScan(primero);
            primera_medicion = false;
        }

        
        primero->rx = 0;
        primero->ry = 0;
        primero->th = 0;

        pm_preprocessScan(segundo);
        bool matchFailed = false;
        try {
            primero->rx = 0;
            primero->th = 0;
            primero->ry = 0;
            
//          segundo->rx = 0;
//          segundo->th = 0;
//          segundo->ry = 0;
            float vr = pCDALaser->vr;
            float vl = pCDALaser->vl;
//            if (vr > 0) {
//                vr = 0.0005 * pow(vr, 3) + 0.0706 * pow(vr, 2) - 2.2914 * vr + 37.625;
//            }
//            if (vl > 0) {
//                vl = 0.0005 * pow(vl, 3) + 0.0706 * pow(vl, 2) - 2.2914 * vl + 37.625;
//            }
            get_new_pose(vr/1.4,vl/1.4,0.25,29.4,segundo);
                       
            //pm_psm(primero, segundo, pFileMatlab);

            pm_psm(primero, segundo, NULL);
            //PM_TYPE err_idx = pm_error_index2(primero, segundo);

            //cout << "% P-S % err: " << pCDALaser->vr << ";"<< pCDALaser->vl << ";" << err_idx << " ; " << segundo->rx << " ; " << segundo->ry << " ; " << segundo->th << "\n";
            //fprintf(pFileMatlab, "%% P-S;vr;%d;vl;%d;err;%f;dx;%f;dy;%f;dh;%f\n", pCDALaser->vr, pCDALaser->vl, err_idx, segundo->rx, segundo->ry, segundo->th);
        } catch (int err) {
            cerr << "Error caught: failed match." << endl;
            matchFailed = true;
        }

        PM_TYPE err_idx = pm_error_index2(primero, segundo);
        cout << " % err: " << err_idx << "\n";
        if (matchFailed || err_idx > 5.0) {
            printf("Nueva Prueba.\n");
            try {
                Pose temp;
                temp.x = segundo->rx;
                temp.y = segundo->ry;
                temp.th = segundo->th;
                //fprintf(stdout, "Need Switching reference scan.\n");                
                segundo->rx = 0;
                segundo->ry = 0;
                segundo->th = 0;
                primero->rx = 0;
                primero->ry = 0;
                primero->th = 0;
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

        fprintf(stdout, "\n");
        if (pCDAExecutive->on_moving) {
            fprintf(pFile, "SP:\t%f\n", pCDAExecutive->exec_speed);
            fprintf(pFile, "St:\t%f\n", pCDAExecutive->exec_steering);
        } else {
            fprintf(pFile, "SP:\t0\n");
            fprintf(pFile, "St:\t0\n");
        }
        
        // TRANSFORMACIONES
        float xx = segundo->rx * cos(tempth) - segundo->ry * sin(tempth);
        float yy = segundo->rx * sin(tempth) + segundo->ry * cos(tempth);
        float tth = segundo->th;
        segundo->rx = xx;
        segundo->ry = yy;
        segundo->th = tth;
       /* final_matlab(
                tv.tv_usec,
                filename,
                outstr_file,
                segundo);*/
        temprx += xx;        
        tempry += yy;
        tempth += tth;

        n_pose++;
        poses_laser[n_pose].x = temprx;
        poses_laser[n_pose].y = tempry;
        poses_laser[n_pose].th = tempth;

        poses_robot[n_pose].x = temprx - cos(tempth)*15.5;
        poses_robot[n_pose].y = tempry - sin(tempth)*15.5;
        poses_robot[n_pose].th = tempth;     
        
        //set_image(segundo, poses_laser[n_pose], img);
        set_image_color_blanco(segundo, poses_laser[n_pose], img);
        CImg<unsigned char> img_temp(img);
        dibujar_robot(segundo, poses_laser[n_pose], poses_robot[n_pose], img_temp);
        dibujar_poses(poses_laser, poses_robot, n_pose, img_temp);
        img_temp.draw_text(50,5,"PosX = %g\nPosY:%g\nPosDir:%g",
                white,NULL,1,temprx,tempry,tempth);
        disp.display(img_temp);

        tercero = segundo;
        segundo = primero;
        primero = tercero;

        timestamp_old = tv.tv_usec;
        fclose(pFile);
        strcpy(filename_old, filename);
        pCDALaser -> x = temprx;
        pCDALaser -> y = tempry;
        pCDALaser -> dir = tempth;
        pCDALaser -> sensar = false;
        cimg_forXY(img,xx,yy){
            pCDALaser -> map[xx][yy] = img(xx,yy,0);
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
    disp.display(img);

    poses_laser[0].x = temprx;
    poses_laser[0].y = tempry;
    poses_laser[0].th = tempth;

    poses_robot[0].x = temprx - cos(tempth)*15.5;
    poses_robot[0].y = tempry - sin(tempth)*15.5;
    poses_robot[0].th = tempth;

    laser.execute();    

    return (0);
}

