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
    poses_laser[0].x = rx;
    poses_laser[0].y = ry;
    poses_laser[0].th = th;

    poses_robot[0].x = rx - cos(th)*15.5;
    poses_robot[0].y = ry - sin(th)*15.5;
    poses_robot[0].th = th;

    poses_modelo[0].x = rx - cos(th)*15.5;
    poses_modelo[0].y = ry - sin(th)*15.5;
    poses_modelo[0].th = th;

    poses_mejor[0].x = poses_robot[0].x;
    poses_mejor[0].y = poses_robot[0].y;
    poses_mejor[0].th = poses_robot[0].th;

    poses_mejorlaser[0].x = rx;
    poses_mejorlaser[0].y = ry;
    poses_mejorlaser[0].th = th;

    poses_modelo_laser[0].x = rx;
    poses_modelo_laser[0].y = ry;
    poses_modelo_laser[0].th = th;

    poses_ekf[0].x = rx - cos(th)*15.5;
    poses_ekf[0].y = ry - sin(th)*15.5;
    poses_ekf[0].th = th;

    poses_ekf_laser[0].x = rx;
    poses_ekf_laser[0].y = ry;
    poses_ekf_laser[0].th = th;
}

void calculos(PMScan *primero,
        PMScan *segundo){
        primero->rx = 0;
        primero->ry = 0;
        primero->th = 0;
        pm_preprocessScan(segundo);


        PM_TYPE err_idx;
        bool matchFailed = false;
        if (n_pose > 0) {
            Pose pose;
            Pose temp;
            temp.x = segundo->rx;
            temp.y = segundo->ry;
            temp.th = segundo->th;
            float xx_a = segundo->rx * cos(poses_laser[n_pose].th) - segundo->ry * sin(poses_laser[n_pose].th);
            float yy_a = segundo->rx * sin(poses_laser[n_pose].th) + segundo->ry * cos(poses_laser[n_pose].th);
            pose.th = segundo->th + poses_laser[n_pose].th;
            pose.x = xx_a + poses_laser[n_pose].x;
            pose.y = yy_a + poses_laser[n_pose].y;

            set_img_scan(img_scan, poses_ekf_laser[n_pose], img_mejor_pose, primero);

            try {
                img_scan->rx = 0;
                img_scan->th = 0;
                img_scan->ry = 0;
                //                segundo->rx = 0;
                //                segundo->ry = 0;
                //                segundo->th = 0;
                //pm_psm(primero, segundo, pFileMatlab);
                pm_preprocessScan(img_scan);
                pm_preprocessScan(segundo);
                pm_psm(img_scan, segundo, NULL);
                PM_TYPE err_idx_a = pm_error_index2(img_scan, segundo);
                /*
                pose.x = segundo->rx;
                pose.y = segundo->ry;
                pose.th = segundo->th;
                segundo->rx = temp.x;
                segundo->ry = temp.y;
                segundo->th = temp.th;
                 * */

                cout << " % err(img): " << err_idx_a << "\n";
            } catch (int err) {
                cerr << "Error " << err << "caught : failed match." << endl;
                matchFailed = true;
            }
        }
#define IMG_SCAN
#ifdef IMG_SCAN
        else {
#endif
            try {
                /*
                segundo->rx = 0;
                segundo->th = 0;
                segundo->ry = 0;
                 */
                primero->rx = 0;
                primero->th = 0;
                primero->ry = 0;
                //pm_psm(primero, segundo, pFileMatlab);
                pm_preprocessScan(primero);
                pm_preprocessScan(segundo);
                pm_psm(primero, segundo, NULL);
                //PM_TYPE err_idx = pm_error_index2(primero, segundo);

                /* cout << "P-S % err: " << err_idx << " ; " << segundo->rx << " ; " << segundo->ry << " ; " << segundo->th << "\n";
                 segundo->rx = 0;
                 segundo->th = 0;
                 segundo->ry = 0;
                 primero->rx = 0;
                 primero->th = 0;
                 primero->ry = 0;
                 pm_psm(segundo, primero);
                 err_idx = pm_error_index2(segundo, primero);*/
                //cout << "% P-S % err: " << err_idx << " ; " << segundo->rx << " ; " << segundo->ry << " ; " << segundo->th << "\n";
            } catch (int err) {
                cerr << "Error " << err << "caught : failed match." << endl;
                matchFailed = true;
            }
            err_idx = pm_error_index2(primero, segundo);
            cout << " % err: " << err_idx << "\n";
            if (matchFailed || err_idx > 1) {
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


#ifdef MATLAB
        final_matlab(
                atol(archivos[i].substr(archivos[i].length() - 6, 6).c_str()),
                archivos[i].c_str(),
                archivos[i].substr(archivos[i].length() - 13, 6).c_str(),
                segundo,
                temp_xx,
                temp_yy);
#endif


        char texto[100];
        sprintf(texto, "(%d,%d) X: %f Y: %f TH:%f", (int) temprx, (int) tempry, xx, yy, tth);

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

        poses_modelo[n_pose] = get_new_pose_model(get_vd(vr), get_vi(vl), 0.25, L_EJE, poses_modelo[n_pose - 1]);

        poses_modelo_laser[n_pose].x = poses_modelo[n_pose].x + cos(poses_modelo[n_pose].th)*15.5;
        poses_modelo_laser[n_pose].y = poses_modelo[n_pose].y + sin(poses_modelo[n_pose].th)*15.5;
        poses_modelo_laser[n_pose].th = poses_modelo[n_pose].th;

        Pose p_x_k_ = get_new_pose_model(get_vd(vr), get_vi(vl), 0.25, L_EJE, poses_ekf[n_pose - 1]);
        float v = (get_vd(vr) + get_vi(vl)) / 2;

        ekf_valores.x_k_[0][0] = p_x_k_.x;
        ekf_valores.x_k_[1][0] = p_x_k_.y;
        ekf_valores.x_k_[2][0] = p_x_k_.th;

        //TODO: REALIZAR TRANSFORMACIÓN Y SUMAR A POSE DE KALMAN        

        //float tth = segundo->th;
        ekf_valores.z_k[2][0] = dth + poses_ekf_laser[n_pose - 1].th;
        //float xx = segundo->rx * cos(tempth) - segundo->ry * sin(tempth);
        ekf_valores.z_k[0][0] =
                (poses_ekf_laser[n_pose - 1].x
                + dx * cos(poses_ekf_laser[n_pose - 1].th) - dy * sin(poses_ekf_laser[n_pose - 1].th))
                - cos(ekf_valores.z_k[2][0])*15.5;
        //float yy = segundo->rx * sin(tempth) + segundo->ry * cos(tempth);
        ekf_valores.z_k[1][0] =
                (poses_ekf_laser[n_pose - 1].y
                + dx * sin(poses_ekf_laser[n_pose - 1].th) + dy * cos(poses_ekf_laser[n_pose - 1].th))
                - sin(ekf_valores.z_k[2][0])*15.5;

        ekf_valores.f_k[0][0] = 1.0;
        ekf_valores.f_k[0][1] = 0.0;
        ekf_valores.f_k[0][2] = -0.25 * v * sin(poses_ekf[n_pose - 1].th);
        ekf_valores.f_k[1][0] = 0.0;
        ekf_valores.f_k[1][1] = 1.0;
        ekf_valores.f_k[1][2] = 0.25 * v * cos(poses_ekf[n_pose - 1].th);
        ekf_valores.f_k[2][0] = 0.0;
        ekf_valores.f_k[2][1] = 0.0;
        ekf_valores.f_k[2][2] = 1.0;

        /*  Segun v y W*/
        //        ekf_valores.f_v[0][0] = 0.25 * cosf(ekf_valores.x_k_[2][0]);
        //        ekf_valores.f_v[0][1] = 0;
        //        ekf_valores.f_v[1][0] = 0.25 * sinf(ekf_valores.x_k_[2][0]);
        //        ekf_valores.f_v[1][1] = 0;
        //        ekf_valores.f_v[2][0] = 0;
        //        ekf_valores.f_v[2][1] = 0.25;

        /*  Segun vd y vi */
        ekf_valores.f_v[0][0] = 0.25 * (1.0 / 2.0) * cos(poses_ekf[n_pose - 1].th);
        ekf_valores.f_v[0][1] = 0.25 * (1.0 / 2.0) * cos(poses_ekf[n_pose - 1].th);
        ekf_valores.f_v[1][0] = 0.25 * (1.0 / 2.0) * sin(poses_ekf[n_pose - 1].th);
        ekf_valores.f_v[1][1] = 0.25 * (1.0 / 2.0) * sin(poses_ekf[n_pose - 1].th);
        ekf_valores.f_v[2][0] = -0.25 * (1.0 / L_EJE);
        ekf_valores.f_v[2][1] = 0.25 * (1.0 / L_EJE);

        ekf_pk_cpy(&ekf_valores);
        ekf_calc_1(&ekf_valores);
        ekf_calc_kk(&ekf_valores);
        ekf_calc_yk(&ekf_valores);
        ekf_calc_pk(&ekf_valores);

        poses_ekf[n_pose].x = ekf_valores.x_k[0][0];
        poses_ekf[n_pose].y = ekf_valores.x_k[1][0];
        poses_ekf[n_pose].th = ekf_valores.x_k[2][0];
        set_v_histo(vr, vl, poses_ekf[n_pose - 1], poses_ekf[n_pose], L_EJE);

        poses_ekf_laser[n_pose].x = poses_ekf[n_pose].x + cos(poses_ekf[n_pose].th)*15.5;
        poses_ekf_laser[n_pose].y = poses_ekf[n_pose].y + sin(poses_ekf[n_pose].th)*15.5;
        poses_ekf_laser[n_pose].th = poses_ekf[n_pose].th;

        //CALCULAR ERRORES Y DEJAR EL MENOR EN MEJOR
        if (n_pose > 0) {

            Pose diferencia_kalman = get_dif_pose(poses_ekf_laser[n_pose - 1], poses_ekf_laser[n_pose]);
            Pose diferencia_psm = get_dif_pose(poses_laser[n_pose - 1], poses_laser[n_pose]);
            PM_TYPE error_kalman = error_pose(diferencia_kalman, img_scan, segundo);
            printf("\nError Kalman:(error) %f", error_kalman);
            PM_TYPE error_psm = error_pose(diferencia_psm, img_scan, segundo);
            printf("\nError PSM:(error) %f", error_psm);
            if (error_kalman < error_psm) {
                poses_mejorlaser[n_pose] = get_pose_global(poses_mejorlaser[n_pose - 1], diferencia_kalman);

            } else {
                poses_mejorlaser[n_pose] = get_pose_global(poses_mejorlaser[n_pose - 1], diferencia_psm);
            }
            poses_mejor[n_pose].x = poses_mejorlaser[n_pose].x - cos(poses_mejorlaser[n_pose].th)*15.5;
            poses_mejor[n_pose].y = poses_mejorlaser[n_pose].y - sin(poses_mejorlaser[n_pose].th)*15.5;
            poses_mejor[n_pose].th = poses_mejorlaser[n_pose].th;
        }

        ////
        
        set_image_color_blanco(segundo, poses_laser[n_pose], img,black);
        set_image(segundo, poses_laser[n_pose], img);
        CImg<unsigned char> img_temp(img);
        dibujar_robot(segundo, poses_laser[n_pose], poses_robot[n_pose], img_temp);
        dibujar_poses(poses_laser, poses_robot, n_pose, img_temp);


        set_image(segundo, poses_modelo_laser[n_pose], img_modelo);
        CImg<unsigned char> img_temp_2(img_modelo);
        dibujar_robot(segundo, poses_modelo_laser[n_pose], poses_modelo[n_pose], img_temp_2);
        dibujar_poses(poses_modelo_laser, poses_modelo, n_pose, img_temp_2);
        //img_temp.draw_line(x0, y0, x1, y1, azul);

        set_image(segundo, poses_ekf_laser[n_pose], img_mejor_pose);
        set_image(segundo, poses_ekf_laser[n_pose], img_mejor_pose_blanco);
        if (n_pose > 1)
            set_image_color_blanco(img_scan, poses_ekf_laser[n_pose - 1], img_mejor_pose_blanco, black);
        CImg<unsigned char> img_temp_mejor_pose(img_mejor_pose_blanco);        
        set_image_color(segundo, poses_ekf_laser[n_pose], img_mejor_pose,azul);
        set_image_color(primero, poses_ekf_laser[n_pose - 1], img_temp_mejor_pose, verde);
        set_image_color(img_scan, poses_ekf_laser[n_pose - 1], img_temp_mejor_pose, rojo);
        dibujar_robot(segundo, poses_ekf_laser[n_pose], poses_ekf[n_pose], img_temp_mejor_pose);
        dibujar_poses(poses_ekf_laser, poses_ekf, n_pose, img_temp_mejor_pose);

        set_image(segundo, poses_mejorlaser[n_pose], img_EKF);
        CImg<unsigned char> img_temp_3(img_EKF);
        dibujar_robot(segundo, poses_mejorlaser[n_pose], poses_mejor[n_pose], img_temp_3);
        dibujar_poses(poses_mejorlaser, poses_mejor, n_pose, img_temp_3);

        //img_temp.draw_line(x0, y0, x1, y1, azul);
        //if (view_data) {
        if(false){
            char text2[200];
            struct stat st = {0};
            // Imagenes de Laser        
            if (stat("MEJOR_POSE", &st) == -1) {
                mkdir("MEJOR_POSE", 0700);
            }
            sprintf(text2,
                    "MEJOR_POSE/%s.png",
                    archivos[i].substr(archivos[i].find("/"), archivos[i].length() - 1).c_str()
                    );
            img_temp_mejor_pose.save(text2);
            if (stat("MAP_PSM", &st) == -1) {
                mkdir("MAP_PSM", 0700);
            }
            sprintf(text2,
                    "MAP_PSM/%s.png",
                    archivos[i].substr(archivos[i].find("/"), archivos[i].length() - 1).c_str()
                    );
            img_temp.save(text2);
            // Imagenes Odometría
            if (stat("MAP_ODO", &st) == -1) {
                mkdir("MAP_ODO", 0700);
            }
            sprintf(text2,
                    "MAP_ODO/%s.png",
                    archivos[i].substr(archivos[i].find("/"), archivos[i].length() - 1).c_str()
                    );
            img_temp_2.save(text2);
            // Imagenes EKF        
            if (stat("MAP_EKF", &st) == -1) {
                mkdir("MAP_EKF", 0700);
            }
            sprintf(text2,
                    "MAP_EKF/%s.png",
                    archivos[i].substr(archivos[i].find("/"), archivos[i].length() - 1).c_str()
                    );
            img_temp_3.save(text2);

            //disp.display(img);
        }
        
        tercero = segundo;
        segundo = primero;
        primero = tercero;

}