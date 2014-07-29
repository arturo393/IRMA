#include "ekf.h"

int isnan_float(float f) {
    return (f != f);
}
float vd_historial[201];
float vi_historial[201];
void ekf_set_valores(
    struct ekf *ekf_valores,
    struct Pose pose_anterior,
    struct Pose pose_anterior_laser,
    int l_eje,
    float vr,float vl,float dth, float dx, float dy)
{
        Pose p_x_k_ = get_new_pose_model(vr, vl, 0.25, l_eje, pose_anterior);
        float v = (vr + vl) / 2;

        ekf_valores->x_k_[0][0] = p_x_k_.x;
        ekf_valores->x_k_[1][0] = p_x_k_.y;
        ekf_valores->x_k_[2][0] = p_x_k_.th;

        //TODO: REALIZAR TRANSFORMACIÓN Y SUMAR A POSE DE KALMAN        

        //float tth = segundo->th;
        ekf_valores->z_k[2][0] = dth + pose_anterior_laser.th;
        //float xx = segundo->rx * cos(tempth) - segundo->ry * sin(tempth);
        ekf_valores->z_k[0][0] =
                (pose_anterior_laser.x
                + dx * cos(pose_anterior_laser.th) - dy * sin(pose_anterior_laser.th))
                - cos(ekf_valores->z_k[2][0])*15.5;
        //float yy = segundo->rx * sin(tempth) + segundo->ry * cos(tempth);
        ekf_valores->z_k[1][0] =
                (pose_anterior_laser.y
                + dx * sin(pose_anterior_laser.th) + dy * cos(pose_anterior_laser.th))
                - sin(ekf_valores->z_k[2][0])*15.5;

        ekf_valores->f_k[0][0] = 1.0;
        ekf_valores->f_k[0][1] = 0.0;
        ekf_valores->f_k[0][2] = -0.25 * v * sin(pose_anterior.th);
        ekf_valores->f_k[1][0] = 0.0;
        ekf_valores->f_k[1][1] = 1.0;
        ekf_valores->f_k[1][2] = 0.25 * v * cos(pose_anterior.th);
        ekf_valores->f_k[2][0] = 0.0;
        ekf_valores->f_k[2][1] = 0.0;
        ekf_valores->f_k[2][2] = 1.0;

        /*  Segun v y W*/
        //        ekf_valores->f_v[0][0] = 0.25 * cosf(ekf_valores.x_k_[2][0]);
        //        ekf_valores->f_v[0][1] = 0;
        //        ekf_valores->f_v[1][0] = 0.25 * sinf(ekf_valores.x_k_[2][0]);
        //        ekf_valores->f_v[1][1] = 0;
        //        ekf_valores->f_v[2][0] = 0;
        //        ekf_valores->f_v[2][1] = 0.25;

        /*  Segun vd y vi */
        ekf_valores->f_v[0][0] = 0.25 * (1.0 / 2.0) * cos(pose_anterior.th);
        ekf_valores->f_v[0][1] = 0.25 * (1.0 / 2.0) * cos(pose_anterior.th);
        ekf_valores->f_v[1][0] = 0.25 * (1.0 / 2.0) * sin(pose_anterior.th);
        ekf_valores->f_v[1][1] = 0.25 * (1.0 / 2.0) * sin(pose_anterior.th);
        ekf_valores->f_v[2][0] = -0.25 * (1.0 /l_eje);
        ekf_valores->f_v[2][1] = 0.25 * (1.0 / l_eje);
}
float get_vi_old(float v_i) {
    float v;
    if (v_i < 0)
        v = -v_i;
    else
        v = v_i;


    float valor = v / 1.9;
    if (v <= 20) {
        valor = v / 1.1;
    } else if (v <= 40) {
        valor = (1.1 + ((40.0 / 33.0 - 1.1) / (40.0 - 20.0))*(v - 20.0));
        valor = v / valor;
    } else if (v <= 60) {
        //valor = v / 1.58;
        valor = (40.0 / 33.0 + ((60.0 / 43.0 - 40.0 / 33.0) / (60.0 - 40.0))*(v - 40.0));
        valor = v / valor;
    } else if (v <= 80) {
        valor = (1.28 + ((1.51 - 1.28) / (80.0 - 60.0))*(v - 60.0));
        valor = v / valor;

    } else if (v < 100) {
        valor = v / (1.58 + ((1.8 - 1.58) / (100.0 - 60.0))*(v - 60.0));
    } else if (v >= 100)
        valor = v / 1.8;
    if (v_i < 0)
        return -valor;
    else
        return valor;
}

float get_vd_old(float v_d) {
    float v;
    if (v_d < 0)
        v = -v_d;
    else
        v = v_d;

    float valor = v / 1.9;
    if (v <= 20) {
        valor = v / 1.06;
    } else if (v <= 40) {
        valor = (1.06 + (((40.0 / 26.0) - 1.06) / (40.0 - 20.0))*(v - 20.0));
        valor = v / valor;
    } else if (v <= 60) {
        //valor = v / 1.58;
        valor = ((40.0 / 26.0) + ((60.0 / 41.0 - (40.0 / 26.0)) / (60 - 40.0))*(v - 40));
        valor = (v / valor)-5;
    } else if (v <= 80) {
        valor = (1.25 + ((1.51 - 1.25) / (80 - 60))*(v - 60));
        valor = v / valor;

    } else if (v < 100) {
        valor = v / (1.58 + ((1.8 - 1.58) / (100 - 60))*(v - 60));
    } else if (v >= 100)
        valor = v / 1.8;
    if (v_d < 0)
        return -valor;
    else
        return valor;
}
void set_historial() {
    for (int i = -100; i <= 100; i++) {
        vd_historial[100 + i] = get_vd_old((float) i);
        vi_historial[100 + i] = get_vi_old((float) i);
    }
}

void set_v_histo(int vd, int vi, struct Pose pose_actual, struct Pose pose_anterior, float l_eje) {
    if (-100 <= vd && vd <= 100 && -100 <= vi && vi <= 100) {
        if (vd == 0 && vi != 0)
            vi_historial[100 + vi] = vi_historial[100 + vi]*0.8 - 0.2 * (pose_actual.th - pose_anterior.th) * l_eje / 0.25;
        else if (vd != 0 && vi == 0)
            vd_historial[100 + vd] = vd_historial[100 + vd]*0.8 + 0.2 * (pose_actual.th - pose_anterior.th) * l_eje / 0.25;
    }
}

float get_vi_histo(int v) {
    if (-100 < v && v < 100)
        return vi_historial[v + 100];
    else if (v < -100) {
        return vi_historial[0];
    } else
        return vi_historial[200];
}

float get_vd_histo(int v) {
    if (-100 < v && v < 100)
        return vd_historial[v + 100];
    else if (v < -100) {
        return vd_historial[0];
    } else
        return vd_historial[200];
}

void get_new_pose(float vd, float vi, float intervalo, float d_eje, PMScan *datos) {
    float dt = 0.001;
    int intervalos = intervalo / dt;
    float th0 = 0.0;
    float x0 = -cos(0.0)*15;
    float y0 = -sin(0.0)*15;
    for (int i = 0; i < intervalos; i++) {
        th0 = get_new_dir(vd, vi, th0, d_eje, dt);
        x0 = get_new_x(vd, vi, th0, x0, dt);
        y0 = get_new_y(vd, vi, th0, y0, dt);
    }
    datos->th = th0;
    datos->rx = x0 + cos(th0)*15;
    datos->ry = y0 + sin(th0)*15;
}

Pose get_new_pose_model(float vd, float vi, float intervalo, float d_eje, struct Pose pose) {
    Pose new_pose;
    float dt = 0.001;
    float th0 = pose.th;
    float x0 = (float) pose.x;
    float y0 = (float) pose.y;
    int intervalos = intervalo / dt;
    float th1 = th0;
    float x1 = x0;
    float y1 = y0;
    for (int i = 0; i < intervalos; i++) {
        th1 = get_new_dir(vd, vi, th1, d_eje, dt);
        x1 = get_new_x(vd, vi, th1, x1, dt);
        y1 = get_new_y(vd, vi, th1, y1, dt);
    }

    new_pose.th = th1;
    new_pose.x = x1;
    new_pose.y = y1;
    return new_pose;
}

/*
0.006044       , 0.000531       ,0.000188
        [0.000528       ,0.002903       ,0.000053
        [0.000189       ,0.000054       ,0.000015
 */

void ekf_init(struct ekf *ekf_valores) {
    set_historial();
    //P_k Inicial
    /*
        ekf_valores->p_k[0][0] = 0.006623;
        ekf_valores->p_k[0][1] = 0.000953;
        ekf_valores->p_k[0][2] = 0.000204;
        ekf_valores->p_k[1][0] = 0.000946;
        ekf_valores->p_k[1][1] = 0.003529;
        ekf_valores->p_k[1][2] = 0.000077;
        ekf_valores->p_k[2][0] = 0.000205;
        ekf_valores->p_k[2][1] = 0.000078;
        ekf_valores->p_k[2][2] = 0.000015;
     */

    /*
        ekf_valores->p_k[0][0] = 0.006675;
        ekf_valores->p_k[0][1] = 0.000998;
        ekf_valores->p_k[0][2] = 0.000205;
        ekf_valores->p_k[1][0] = 0.000990;
        ekf_valores->p_k[1][1] = 0.003587;
        ekf_valores->p_k[1][2] = 0.000079;
        ekf_valores->p_k[2][0] = 0.000206;
        ekf_valores->p_k[2][1] = 0.000080;
        ekf_valores->p_k[2][2] = 0.000015;
     */
    /*
    ekf_valores->p_k[0][0]=0.074156;
    ekf_valores->p_k[0][1]=0.002439;
    ekf_valores->p_k[0][2]=-0.000002;
    ekf_valores->p_k[1][0]=0.002367;
    ekf_valores->p_k[1][1]=0.078419;
    ekf_valores->p_k[1][2]=0.000157;
    ekf_valores->p_k[2][0]=-0.000008;
    ekf_valores->p_k[2][1]=0.000156;
    ekf_valores->p_k[2][2]=0.000216;
     */
    /*
    ekf_valores->p_k[0][0]=0.113360;
    ekf_valores->p_k[0][1]=0.003596;
    ekf_valores->p_k[0][2]=0.000001;
    ekf_valores->p_k[1][0]=0.003330;
    ekf_valores->p_k[1][1]=0.149514;
    ekf_valores->p_k[1][2]=0.000156;
    ekf_valores->p_k[2][0]=-0.000008;
    ekf_valores->p_k[2][1]=0.000156;
    ekf_valores->p_k[2][2]=0.000216;
     */
    //set_historial();
    /*
        ekf_valores->p_k[0][0] = 2.0;
        ekf_valores->p_k[0][1] = 2.0;
        ekf_valores->p_k[0][2] = 2.0;
        ekf_valores->p_k[1][0] = 2.0;
        ekf_valores->p_k[1][1] = 2.0;
        ekf_valores->p_k[1][2] = 2.0;
        ekf_valores->p_k[2][0] = 2.0;
        ekf_valores->p_k[2][1] = 2.0;
        ekf_valores->p_k[2][2] = 2.0;
     */
/*
    ekf_valores->p_k[0][1]=2.0;
    ekf_valores->p_k[0][2]=2.0;
    ekf_valores->p_k[1][0]=2.0;
    ekf_valores->p_k[1][1]=2.0;
    ekf_valores->p_k[1][2]=2.0;
    ekf_valores->p_k[2][0]=2.0;
    ekf_valores->p_k[2][1]=2.0;
    ekf_valores->p_k[2][2]=2.0;
*/

ekf_valores->p_k[0][0]=2.658942;    
ekf_valores->p_k[0][1]=0.366914;
ekf_valores->p_k[0][2]=0.004276;
ekf_valores->p_k[1][0]=0.622768;
ekf_valores->p_k[1][1]=1.171261;
ekf_valores->p_k[1][2]=0.003539;
ekf_valores->p_k[2][0]=0.000000;
ekf_valores->p_k[2][1]=0.002030;
ekf_valores->p_k[2][2]=0.003248;

    
    
/*
    ekf_valores->p_k[0][0] = 2.0;
    ekf_valores->p_k[0][1] = 2.0;
    ekf_valores->p_k[0][2] = 2.0;
    ekf_valores->p_k[1][0] = 2.0;
    ekf_valores->p_k[1][1] = 2.0;
    ekf_valores->p_k[1][2] = 2.0;
    ekf_valores->p_k[2][0] = 2.0;
    ekf_valores->p_k[2][1] = 2.0;
    ekf_valores->p_k[2][2] = 2.0;
*/


    //H (Matriz identidad)
    ekf_valores->h[0][0] = 1.0;
    ekf_valores->h[0][1] = 0.0;
    ekf_valores->h[0][2] = 0.0;
    ekf_valores->h[1][0] = 0.0;
    ekf_valores->h[1][1] = 1.0;
    ekf_valores->h[1][2] = 0.0;
    ekf_valores->h[2][0] = 0.0;
    ekf_valores->h[2][1] = 0.0;
    ekf_valores->h[2][2] = 1.0;

    ekf_valores->q_k_1[0][0] = 120.0;
    ekf_valores->q_k_1[0][1] = 0.0;
    ekf_valores->q_k_1[1][0] = 0.0;
    ekf_valores->q_k_1[1][1] = 120.0;

    /*
        ekf_valores->r[0][0] = 0.25;
        ekf_valores->r[0][1] = 0.090901098;
        ekf_valores->r[0][2] = 0.0081791901;
        ekf_valores->r[1][0] = 0.090901098;
        ekf_valores->r[1][1] = 0.193481394;
        ekf_valores->r[1][2] = 0.0030702745;
        ekf_valores->r[2][0] = 0.0081791901;
        ekf_valores->r[2][1] = 0.0030702745;
        ekf_valores->r[2][2] = 0.0004;
     */
    /*
        ekf_valores->r[0][0] = 0.14;
        ekf_valores->r[0][1] = -0.03308;
        ekf_valores->r[0][2] = -0.0039685;
        ekf_valores->r[1][0] = -0.03308;
        ekf_valores->r[1][1] = 0.056111;
        ekf_valores->r[1][2] = 0.0017307;
        ekf_valores->r[2][0] = -0.0039685;
        ekf_valores->r[2][1] = 0.0017307;
        ekf_valores->r[2][2] = 0.0004;
     */
    ekf_valores->r[0][0] = 5.0;
    ekf_valores->r[0][1] = 0.0;
    ekf_valores->r[0][2] = 0.0;
    ekf_valores->r[1][0] = 0.0;
    ekf_valores->r[1][1] = 5.0;
    ekf_valores->r[1][2] = 0.0;
    ekf_valores->r[2][0] = 0.0;
    ekf_valores->r[2][1] = 0.0;
    ekf_valores->r[2][2] = 0.005;
}

void ekf_calc_pk(struct ekf *valores) {
    float resultado[3][3];
    //Multiplicar Matriz F_k * P_{k-1}
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            resultado[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                resultado[i][j] += (valores->k_k[i][nn]) * (valores->h[nn][j]);
            }
            resultado[i][j] = valores->h[i][j] - resultado[i][j];
        }
    }
    //Multiplicar Matriz F_k * P_{k-1}
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            valores->p_k[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                valores->p_k[i][j] += (resultado[i][nn]) * (valores->p_k_[nn][j]);
            }
            if (isnan(valores->p_k[i][j])==1 || valores->p_k[i][j] < 0.0000001)
                valores->p_k[i][j] = 0.0000001;
        }
    }


}

float get_new_x(float vd, float vi, float dir, float x, float dt) {
    return ((1.0 / 2.0) * (vd + vi) * cos(dir) * dt) +x;
}

float get_new_y(float vd, float vi, float dir, float y, float dt) {
    return ((1.0 / 2.0) * (vd + vi) * sin(dir) * dt) +y;
}

float get_new_dir(float vd, float vi, float dir, float d_eje, float dt) {
    return (1.0 / d_eje)*(vi - vd) * dt + dir;
}

void ekf_pk_cpy(struct ekf *ekf_valores) {
    ekf_valores->p_k_1[0][0] = ekf_valores->p_k[0][0];
    ekf_valores->p_k_1[0][1] = ekf_valores->p_k[0][1];
    ekf_valores->p_k_1[0][2] = ekf_valores->p_k[0][2];
    ekf_valores->p_k_1[1][0] = ekf_valores->p_k[1][0];
    ekf_valores->p_k_1[1][1] = ekf_valores->p_k[1][1];
    ekf_valores->p_k_1[1][2] = ekf_valores->p_k[1][2];
    ekf_valores->p_k_1[2][0] = ekf_valores->p_k[2][0];
    ekf_valores->p_k_1[2][1] = ekf_valores->p_k[2][1];
    ekf_valores->p_k_1[2][2] = ekf_valores->p_k[2][2];
}

float get_vi(int v_i) {
    return get_vi_histo(v_i);
}

float get_vd(int v_d) {
    return get_vd_histo(v_d);
}

void ekf_calc_yk(struct ekf *valores) {
    valores->y_k[0][0] = valores->z_k[0][0] - valores->x_k_[0][0];
    valores->y_k[1][0] = valores->z_k[1][0] - valores->x_k_[1][0];
    valores->y_k[2][0] = valores->z_k[2][0] - valores->x_k_[2][0];

    //Multiplicar Matriz F_k * P_{k-1}
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 1; j++) {
            valores->x_k[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                valores->x_k[i][j] += (valores->k_k[i][nn]) * (valores->y_k[nn][j]);
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 1; j++) {
            valores->x_k[i][j] += valores->x_k_[i][j];
        }
    }

}

void ekf_calc_kk(struct ekf *valores) {
    float resultado[3][3];
    float resultado_h[3][3];
    float resultado_a[3][3];
    //MULTIPLICAR (P_K*H^T)
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            resultado_a[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                resultado_a[i][j] += (valores->p_k_[i][nn])*(valores->h[nn][j]);
            }
        }
    }
    //MULTIPLICAR (H*P_K*H^T)
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            resultado[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                resultado[i][j] += (valores->h[i][nn])*(valores->p_k_[nn][j]);
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            resultado_h[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                resultado_h[i][j] += (resultado[i][nn]) * (valores->h[nn][j]);
            }
        }
    }
    float resultado_2[3][3] = {
        {resultado_h[0][0] + valores->r[0][0], resultado_h[0][1] + valores->r[0][1], resultado_h[0][2] + valores->r[0][2]},
        {resultado_h[1][0] + valores->r[1][0], resultado_h[1][1] + valores->r[1][1], resultado_h[1][2] + valores->r[1][2]},
        {resultado_h[2][0] + valores->r[2][0], resultado_h[2][1] + valores->r[2][1], resultado_h[2][2] + valores->r[2][2]}
    };
    float resultado_3[3][2] = {
        {0, 0},
        {0, 0},
        {0, 0}
    };
    float resultado_4[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };

    double a = resultado_2[0][0]*(resultado_2[1][1] * resultado_2[2][2] - resultado_2[2][1] * resultado_2[1][2]);
    double b = resultado_2[0][1]*(resultado_2[1][0] * resultado_2[2][2] - resultado_2[2][0] * resultado_2[1][2]);
    double c = resultado_2[0][2]*(resultado_2[1][0] * resultado_2[2][1] - resultado_2[2][0] * resultado_2[1][1]);

    double det = a - b + c;

    //Transpuesta de resultado_2 en resultado 3

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            resultado_3[j][i] = resultado_2[i][j];
        }
    }

    //Resultado de Inversa

    resultado_4[0][0] = (resultado_3[1][1] * resultado_3[2][2] - resultado_3[2][1] * resultado_3[1][2]) / det;
    resultado_4[0][1] = (-(resultado_3[1][0] * resultado_3[2][2] - resultado_3[2][0] * resultado_3[1][2])) / det;
    resultado_4[0][2] = (resultado_3[1][0] * resultado_3[2][1] - resultado_3[2][0] * resultado_3[1][1]) / det;
    resultado_4[1][0] = -(resultado_3[0][1] * resultado_3[2][2] - resultado_3[2][1] * resultado_3[0][2]) / det;
    resultado_4[1][1] = (resultado_3[0][0] * resultado_3[2][2] - resultado_3[2][0] * resultado_3[0][2]) / det;
    resultado_4[1][2] = -(resultado_3[0][0] * resultado_3[2][1] - resultado_3[2][0] * resultado_3[0][1]) / det;
    resultado_4[2][0] = (resultado_3[0][1] * resultado_3[1][2] - resultado_3[1][1] * resultado_3[0][2]) / det;
    resultado_4[2][1] = -(resultado_3[0][0] * resultado_3[1][2] - resultado_3[1][0] * resultado_3[1][2]) / det;
    resultado_4[2][2] = (resultado_3[0][0] * resultado_3[1][1] - resultado_3[1][0] * resultado_3[0][1]) / det;

    //Multiplicar Matriz F_k * P_{k-1}
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            valores->k_k[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                valores->k_k[i][j] += (resultado_a[i][nn]) * (resultado_4[nn][j]);
            }
        }
    }

}

void ekf_calc_1(struct ekf *valores) {
    float resultado[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    float resultado_2[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    float resultado_3[3][2] = {
        {0, 0},
        {0, 0},
        {0, 0}
    };
    float resultado_4[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    //Multiplicar Matriz F_k * P_{k-1}
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            for (int nn = 0; nn < 3; nn++) {
                resultado[i][j] += (valores->f_k[i][nn]) * (valores->p_k_1[nn][j]);
            }
        }
    }
    //Transpuesta de F_k
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            valores->f_k_t[j][i] = valores->f_k[i][j];
        }
    }
    //Multiplicar Matriz Resultanto por Transpueta de F_k
    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            resultado_2[i][j] = 0;
            for (int nn = 0; nn < 3; nn++) {
                resultado_2[i][j] += (resultado[i][nn])*(valores->f_k_t[nn][j]);
            }
        }
    }
    //Multipicar F_v * Q_{k-1}

    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 2; j++) {
            resultado_3[i][j] = 0;
            for (int nn = 0; nn < 2; nn++) {
                resultado_3[i][j] += (valores->f_v[i][nn])*(valores->q_k_1[nn][j]);
            }
        }
    }

    //Transpuesta de F_v
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            valores->f_v_t[j][i] = valores->f_v[i][j];
        }
    }

    //Multipicar ( F_v * Q_{k-1} ) * F_v_t

    for (int i = 0; i < 3; i++) {
        //resultado[i] = (float*) malloc(p * sizeof (float));
        for (int j = 0; j < 3; j++) {
            for (int nn = 0; nn < 2; nn++) {
                resultado_4[i][j] += (resultado_3[i][nn])*(valores->f_v_t[nn][j]);
            }
        }
    }

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            valores->p_k_[i][j] = resultado_2[i][j] + resultado_4[i][j];

    return;
};