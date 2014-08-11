#ifndef _IRMA3_EKF_H_
#define _IRMA3_EKF_H_

#include <math.h>
#include "polar_matching/polar_matching/polar_match.h"
void set_v_histo(int vd,int vi,struct Pose actual,struct Pose anterior,float l_eje);
struct ekf {
    float p_k[3][3];
    float p_k_[3][3];
    float p_k_1[3][3];
    float f_k[3][3];
    float f_k_t[3][3];
    float f_v[3][2];
    float f_v_t[2][3];
    float q_k_1[2][2];
    float h[3][3];
    float r[3][3];
    float k_k[3][3];
    float y_k[3][1];
    float z_k[3][1];
    float x_k[3][1];
    float x_k_[3][1];
};
void set_historial();
float get_vi_old(float v_i);
float get_vd_old(float v_i);
struct Pose {
    float x;
    float y;
    float th;
};
float get_vi(int v);
float get_vd(int v);
void ekf_pk_cpy(struct ekf *ekf_valores);
Pose get_new_pose_model(float vd, float vi, float intervalo, float d_eje, struct Pose pose);
//Inicialización de valores para ekf
void ekf_init(struct ekf *ekf_valores);
//Calculo de PK
void ekf_calc_pk(struct ekf *valores);
void ekf_calc_yk(struct ekf *valores);
//Entrega la siguiente posición de X
float get_new_x(float vd, float vi, float dir, float x, float dt);
//Entrega la siguiente posición de y
float get_new_y(float vd, float vi, float dir, float y, float dt);
//Entrega la siguiente posición de dirección
float get_new_dir(float vd, float vi, float dir, float d_eje, float dt);
void get_new_pose(float vd, float vi, float intervalo, float d_eje, PMScan *datos);
void ekf_calc_kk(struct ekf *valores);
void ekf_calc_1(struct ekf *valores);
//Establece valores iniciales del Filtro Extendido de Kalman
void ekf_set_valores(
    struct ekf *ekf_valores,
    struct Pose pose_anterior,
    struct Pose pose_anterior_laser,
    int l_eje,
    float vr,
    float vl,
    float dth,
    float dx,
    float dy);
#endif
