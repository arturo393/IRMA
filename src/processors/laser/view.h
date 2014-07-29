#ifndef _IRMA3_VIEW_H_
#define _IRMA3_VIEW_H_

#include "ekf.h"
#include "polar_matching/polar_matching/polar_match.h"
#include "CImg.h"


using namespace cimg_library;

const int IMG_ANCHO = 1600;
const int IMG_ALTO = 1600;
const PM_TYPE PM_FI_MIN = -1.3008157081 - M_PI / 4; //-1.3008157081;
const unsigned char white[] = {255, 255, 255, 255}; // Define a purple color
const unsigned char black[] = {0, 0, 0, 255}; // Define a purple color    
const unsigned char rojo[] = {255, 0, 0, 255};
const unsigned char azul[] = {0, 0, 255, 255};
const unsigned char verde[] = {0, 255, 255, 255};
Pose get_pose_global(Pose nuevo,Pose diferencia);
void set_image_color(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img,const unsigned char color[]);
Pose get_dif_pose(Pose anterior, Pose actual);
PM_TYPE error_pose(Pose dif_pose,PMScan *scan_anterior, PMScan *scan_actual);
void set_img_scan(PMScan *imagen,Pose pose,CImg<unsigned char>& img,PMScan *primero);
void set_img_scan_promedio(PMScan *imagen,Pose pose,CImg<unsigned char>& img,PMScan *primero);
void set_image_color(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img);
void set_image_color_blanco(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img,const unsigned char color[]);
void set_image_color_blanco(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img);
void set_image(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img);
void set_image_promedio(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img);
void dibujar_robot(PMScan *segundo, struct Pose pose_laser, struct Pose pose_robot, CImg<unsigned char>& img);
void dibujar_poses(struct Pose poses_laser[], struct Pose poses_robot[], int n_pose, CImg<unsigned char>& img);
void set_data(CImg<unsigned char>& img,unsigned char mapa[IMG_ANCHO][IMG_ALTO] );
#endif