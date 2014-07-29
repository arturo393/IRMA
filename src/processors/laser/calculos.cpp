#include "calculos.h"

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
        )
{
    
////Posición de Robot
    poses_robot[0].x = rx - cos(th)*15.5;
    poses_robot[0].y = ry - sin(th)*15.5;
    poses_robot[0].th = th;

    poses_laser[0].x = rx;
    poses_laser[0].y = ry;
    poses_laser[0].th = th;
//// Posición Mejor
    poses_mejor[0].x = poses_robot[0].x;
    poses_mejor[0].y = poses_robot[0].y;
    poses_mejor[0].th = poses_robot[0].th;

    poses_mejorlaser[0].x = rx;
    poses_mejorlaser[0].y = ry;
    poses_mejorlaser[0].th = th;
//// Posición Modelo Cinematico
    poses_modelo[0].x = poses_robot[0].x;
    poses_modelo[0].y = poses_robot[0].y;
    poses_modelo[0].th = poses_robot[0].th;
    
    poses_modelo_laser[0].x = rx;
    poses_modelo_laser[0].y = ry;
    poses_modelo_laser[0].th = th;
//// Posición EKF
    poses_ekf[0].x = poses_robot[0].x;
    poses_ekf[0].y = poses_robot[0].y;
    poses_ekf[0].th = poses_robot[0].th;

    poses_ekf_laser[0].x = rx;
    poses_ekf_laser[0].y = ry;
    poses_ekf_laser[0].th = th;
}

void calculos(PMScan *primero, PMScan *segundo){
       

       


}