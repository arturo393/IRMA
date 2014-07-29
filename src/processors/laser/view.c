#include "view.h"
const float resolucion = 2; //PIXEL POR CETIMETROS

Pose get_pose_global(Pose anterior, Pose diferencia) {
    Pose k_dp;
    k_dp.x = anterior.x + (diferencia.x * cos(anterior.th) - diferencia.y * sin(anterior.th));
    k_dp.y = anterior.y + (diferencia.x * sin(anterior.th) + diferencia.y * cos(anterior.th));
    k_dp.th = anterior.th + diferencia.th;
    return k_dp;
}

Pose get_dif_pose(Pose anterior, Pose actual) {
    float k_dx = actual.x - anterior.x;
    float k_dy = actual.y - anterior.y;
    float k_dth = actual.th - anterior.th;
    Pose k_dp;
    k_dp.x = (k_dx * cos(-anterior.th) - k_dy * sin(-anterior.th));
    k_dp.y = (k_dx * sin(-anterior.th) + k_dy * cos(-anterior.th));
    k_dp.th = k_dth;
    return k_dp;
}

PM_TYPE error_pose(Pose diferencia, PMScan *scan_anterior, PMScan *scan_actual) {

    scan_actual->rx = diferencia.x;
    scan_actual->ry = diferencia.y;
    scan_actual->th = diferencia.th;
    return pm_error_index2(scan_anterior, scan_actual);
}
void set_data(CImg<unsigned char>& img,unsigned char mapa[IMG_ANCHO][IMG_ALTO] ){
    
}
void set_img_scan(PMScan *imagen, Pose pose, CImg<unsigned char>& img, PMScan *primero) {
    float angulo = 0.0;
    for (unsigned int h = 0; h<sizeof (imagen->r); h++) {
        angulo = h * M_PI / 512 + PM_FI_MIN + pose.th;
        int n = 0;
        float sum_r = 0;
        for (int i = 0; i < 400 * resolucion; i++) {
            int x = (i) * cos(angulo) + pose.x * resolucion + IMG_ANCHO / 2;
            int y = (i) * sin(angulo) + pose.y * resolucion + IMG_ALTO / 2;
            int ptr = (int) img(x, y, 0, 0);
            if (ptr == 0) {
                n++;
                if (0 > y || y > IMG_ALTO || x < 0 || x > IMG_ANCHO) {
                    imagen->r[h] = primero->r[h];
                    //imagen->r[h] = 0;
                } else {
                    sum_r += i;
                    for (int drr = 1; drr < 30 * resolucion; drr++) {
                        ptr = (int) img(x + (drr) * cos(angulo), y + (drr) * sin(angulo), 0, 0);
                        if (ptr == 0) {
                            sum_r += i + drr;
                            n++;
                        }
                    }
                }
            }
            if (n > 0) {
                float promedio = (sum_r / resolucion) / n;
                if (sum_r == 0) {
                    imagen->r[h] = primero->r[h];
                } else if (primero->r[h] > 30 && primero->r[h] < 4000) {
                    imagen->r[h] = promedio;
                } else {
                    imagen->r[h] = promedio;
                }

                break;
            }

        }
    }
}

void set_img_scan_promedio(PMScan *imagen, Pose pose, CImg<unsigned char>& img, PMScan *primero) {
    float angulo = 0.0;
    for (unsigned int h = 0; h<sizeof (imagen->r); h++) {
        angulo = h * M_PI / 512 + PM_FI_MIN + pose.th;
        int n = 0;
        float sum_r = 0;
        int min = -1;
        int max = -1;
        for (int i = 0; i < 400 * resolucion; i++) {
            int x = (i) * cos(angulo) + pose.x * resolucion + IMG_ANCHO / 2;
            int y = (i) * sin(angulo) + pose.y * resolucion + IMG_ALTO / 2;
            int ptr = (int) img(x, y, 0, 0);

            if (ptr == 0) {
                n++;
                if (0 > y || y > IMG_ALTO || x < 0 || x > IMG_ANCHO) {
                    imagen->r[h] = primero->r[h];
                    break;
                    //imagen->r[h] = 0;
                } else {
                    if (min == -1)
                        min = i;
                    sum_r += i;
                    max = i;
                }
            } else {
                if (n > 4 * resolucion || max - i > 3 * resolucion) {
                    if (primero->r[h] > 30 && primero->r[h] < 4000) {
                        imagen->r[h] = (0.5 * (sum_r / resolucion) / n + 0.5 * primero->r[h]);
                    } else {
                        imagen->r[h] = (sum_r / resolucion) / n;
                    }

                    break;
                }
            }
        }
    }
}

void dibujar_poses(struct Pose poses_laser[], struct Pose poses_robot[], int n_pose, CImg<unsigned char>& img) {
    for (int j = 1; j <= n_pose; j++) {
        img.draw_line(
                IMG_ANCHO / 2 + (poses_laser[j - 1].x) * resolucion,
                IMG_ALTO / 2 + (poses_laser[j - 1].y) * resolucion,
                IMG_ANCHO / 2 + (poses_laser[j].x) * resolucion,
                IMG_ALTO / 2 + (poses_laser[j].y * resolucion), verde);
    };
    for (int j = 1; j <= n_pose; j++) {
        img.draw_circle(
                IMG_ANCHO / 2 + resolucion * (poses_robot[j - 1].x),
                IMG_ALTO / 2 + resolucion * (poses_robot[j - 1].y),3,azul,1,2);
        img.draw_line(
                IMG_ANCHO / 2 + resolucion * (poses_robot[j - 1].x),
                IMG_ALTO / 2 + resolucion * (poses_robot[j - 1].y),
                IMG_ANCHO / 2 + resolucion * (poses_robot[j].x),
                IMG_ALTO / 2 + resolucion * (poses_robot[j].y), azul);
    };
}

void dibujar_robot(PMScan *segundo, struct Pose pose_laser, struct Pose pose_robot, CImg<unsigned char>& img) {
    CImg<int> points(4, 2);

    int x0 = IMG_ANCHO / 2 + (pose_robot.x + cos(pose_robot.th - M_PI / 2)*(29 / 2)) * resolucion;
    int y0 = IMG_ALTO / 2 + (pose_robot.y + sin(pose_robot.th - M_PI / 2)*(29 / 2)) * resolucion;
    int x1 = IMG_ANCHO / 2 + (pose_robot.x + cos(pose_robot.th - M_PI / 2)*((29 / 2) + 4)) * resolucion;
    int y1 = IMG_ANCHO / 2 + (pose_robot.y + sin(pose_robot.th - M_PI / 2)*((29 / 2) + 4)) * resolucion;
    int thePoints[] = {
        x0 + cos(pose_robot.th)*(5 * resolucion), (y0 + sin(pose_robot.th)*5 * resolucion),
        x0 + cos(pose_robot.th)*(-5 * resolucion), (y0 + sin(pose_robot.th)*(-5 * resolucion)),
        x1 + cos(pose_robot.th)*(-5 * resolucion), (y1 + sin(pose_robot.th)*(-5 * resolucion)),
        x1 + cos(pose_robot.th)*(5 * resolucion), (y1 + sin(pose_robot.th)*(5 * resolucion)),
    };
    int *iterator = thePoints;

    cimg_forX(points, i) {

        points(i, 0) = *(iterator++);
        points(i, 1) = *(iterator++);
    };
    //img_temp.draw_circle(x0, y0, 5, rojo);
    img.draw_polygon(points, rojo);

    x0 = IMG_ANCHO / 2 + (pose_robot.x + cos(pose_robot.th - M_PI / 2)*(-29 / 2)) * resolucion;
    y0 = IMG_ALTO / 2 + (pose_robot.y + sin(pose_robot.th - M_PI / 2)*(-29 / 2)) * resolucion;
    x1 = IMG_ANCHO / 2 + (pose_robot.x + cos(pose_robot.th - M_PI / 2)*((-29 / 2) - 4)) * resolucion;
    y1 = IMG_ANCHO / 2 + (pose_robot.y + sin(pose_robot.th - M_PI / 2)*((-29 / 2) - 4)) * resolucion;
    int thePoints_2[] = {
        x0 + cos(pose_robot.th)*5 * resolucion, (y0 + sin(pose_robot.th)*5 * resolucion),
        x0 + cos(pose_robot.th)*(-5 * resolucion), (y0 + sin(pose_robot.th)*(-5 * resolucion)),
        x1 + cos(pose_robot.th)*(-5 * resolucion), (y1 + sin(pose_robot.th)*(-5 * resolucion)),
        x1 + cos(pose_robot.th)*(5 * resolucion), (y1 + sin(pose_robot.th)*(5 * resolucion)),
    };
    int *iterator_2 = thePoints_2;

    cimg_forX(points, i) {
        points(i, 0) = *(iterator_2++);
        points(i, 1) = *(iterator_2++);
    }
    img.draw_polygon(points, rojo);
    for (int j = 0; j < 8; j++) {
        img.draw_line(
                IMG_ANCHO / 2 + (pose_robot.x + cos(pose_robot.th + ((j * M_PI) / 4) + M_PI / 8)*20.8) * resolucion,
                IMG_ALTO / 2 + (pose_robot.y + sin(pose_robot.th + ((j * M_PI) / 4) + M_PI / 8)*20.8) * resolucion,
                IMG_ANCHO / 2 + (pose_robot.x + cos(pose_robot.th + (((j + 1) * M_PI) / 4) + M_PI / 8)*20.8) * resolucion,
                IMG_ALTO / 2 + (pose_robot.y + sin(pose_robot.th + (((j + 1) * M_PI) / 4) + M_PI / 8)*20.8) * resolucion,
                rojo);
    }

    img.draw_circle(
        pose_laser.x * resolucion + IMG_ANCHO / 2,
        pose_laser.y * resolucion + IMG_ALTO / 2, 3, rojo, 1.0, 2);
    img.draw_line(
            IMG_ANCHO / 2 + pose_laser.x*resolucion,
            IMG_ALTO / 2 + pose_laser.y*resolucion,
            IMG_ANCHO / 2 + pose_robot.x*resolucion,
            IMG_ALTO / 2 + pose_robot.y*resolucion,
            rojo
            );
}

void set_image_color(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img, const unsigned char color[]) {
    float base = PM_FI_MIN + pose.th;
    for (unsigned int h = 1; h < sizeof (segundo->r) && h < PM_L_POINTS; h++) {
        float angulo1 = ((float) h - 1) * M_PI / 512 + base;
        float angulo2 = ((float) h) * M_PI / 512 + PM_FI_MIN + pose.th;
        if ((segundo->r[h - 1] < 500) && (segundo->r[h - 1] > 30) && !((h > 23 && h < 50) || (h > 642 && h < 669))) {
            float dx1 = ((segundo->r[h - 1] * cos(angulo1)) - (segundo->r[h] * cos(angulo2))) * resolucion;
            float dy1 = ((segundo->r[h - 1] * sin(angulo1)) - (segundo->r[h] * sin(angulo2))) * resolucion;
            float distancia = sqrt(dx1 * dx1 + dy1 * dy1);
            if (distancia < 5.0 * resolucion) {
                int x = IMG_ANCHO / 2 + (int) (segundo->r[h - 1] * resolucion * cos(angulo1)) + pose.x*resolucion;
                int y = IMG_ALTO / 2 +  (int) (segundo->r[h - 1] * resolucion * sin(angulo1)) + pose.y*resolucion;
                img.draw_line(
                        x,
                        y,
                        IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2) * resolucion + pose.x * resolucion),
                        IMG_ALTO / 2 + (int)  (segundo->r[h] * sin(angulo2) * resolucion + pose.y * resolucion), color);

            }
        }

    }
}

void set_image_color_blanco(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img,const unsigned char color[]) {
    for (unsigned int h = 1; h < sizeof (segundo->r) && h < PM_L_POINTS; h++) {

        float angulo1 = ((float) h - 1) * M_PI / 512 + PM_FI_MIN + pose.th;
        float angulo2 = ((float) h) * M_PI / 512 + PM_FI_MIN + pose.th;
        if ((segundo->r[h - 1] < 500) && (segundo->r[h - 1] > 30) && !((h > 23 && h < 50) || (h > 642 && h < 669))) {
            float centro_x = IMG_ANCHO / 2 + pose.x * resolucion;
            float centro_y = IMG_ALTO / 2 + pose.y * resolucion;
            float dx1 = ((segundo->r[h - 1] * cos(angulo1)) - (segundo->r[h] * cos(angulo2))) * resolucion;
            float dy1 = ((segundo->r[h - 1] * sin(angulo1)) - (segundo->r[h] * sin(angulo1))) * resolucion;
            float distancia = sqrt(dx1 * dx1 + dy1 * dy1);
            if (distancia < 5.0 * resolucion) {
                int x = centro_x + (int) (segundo->r[h - 1] * cos(angulo1) * resolucion);
                int y = centro_y + (int) (segundo->r[h - 1] * sin(angulo1) * resolucion);
                img.draw_line(
                        x,
                        y,
                        IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2) * resolucion) + pose.x*resolucion,
                        IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) * resolucion + pose.y * resolucion),
                        color);
                img.draw_line(
                        centro_x,
                        centro_y,
                        x, y, white
                        );

            }
        }

    }
}
void set_image_color_blanco(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img) {
    for (unsigned int h = 1; h < sizeof (segundo->r) && h < PM_L_POINTS; h++) {

        float angulo1 = ((float) h - 1) * M_PI / 512 + PM_FI_MIN + pose.th;
        float angulo2 = ((float) h) * M_PI / 512 + PM_FI_MIN + pose.th;
        if ((segundo->r[h - 1] < 500) && (segundo->r[h - 1] > 30) && !((h > 23 && h < 50) || (h > 642 && h < 669))) {
            float centro_x = IMG_ANCHO / 2 + pose.x * resolucion;
            float centro_y = IMG_ALTO / 2 + pose.y * resolucion;
            float dx1 = ((segundo->r[h - 1] * cos(angulo1)) - (segundo->r[h] * cos(angulo2))) * resolucion;
            float dy1 = ((segundo->r[h - 1] * sin(angulo1)) - (segundo->r[h] * sin(angulo1))) * resolucion;
            float distancia = sqrt(dx1 * dx1 + dy1 * dy1);
            if (distancia < 5.0 * resolucion) {
                int x = centro_x + (int) (segundo->r[h - 1] * cos(angulo1) * resolucion);
                int y = centro_y + (int) (segundo->r[h - 1] * sin(angulo1) * resolucion);
                img.draw_line(
                        x,
                        y,
                        IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2) * resolucion) + pose.x*resolucion,
                        IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) * resolucion + pose.y * resolucion),
                        black);
                img.draw_line(
                        centro_x,
                        centro_y,
                        x, y, white
                        );

            }
        }

    }
}
void set_image_color(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img) {
    for (unsigned int h = 1; h < sizeof (segundo->r) && h < PM_L_POINTS; h++) {
        float angulo1 = ((float) h - 1) * M_PI / 512 + PM_FI_MIN + pose.th;
        float angulo2 = ((float) h) * M_PI / 512 + PM_FI_MIN + pose.th;
        if ((segundo->r[h - 1] < 500) && (segundo->r[h - 1] > 30) && !((h > 23 && h < 50) || (h > 642 && h < 669))) {
            float dx1 = ((segundo->r[h - 1] * cos(angulo1)) - (segundo->r[h] * cos(angulo2))) * resolucion;
            float dy1 = ((segundo->r[h - 1] * sin(angulo1)) - (segundo->r[h] * sin(angulo1))) * resolucion;
            float distancia = sqrt(dx1 * dx1 + dy1 * dy1);
            if (distancia < 5.0 * resolucion) {
                int x = IMG_ANCHO / 2 + (int) (segundo->r[h - 1] * resolucion * cos(angulo1)) + pose.x*resolucion;
                int y = IMG_ALTO / 2 + (int) (segundo->r[h - 1] * sin(angulo1) * resolucion) + pose.y*resolucion;
                img.draw_line(
                        x,
                        y,
                        IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2) * resolucion) + pose.x*resolucion,
                        IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) * resolucion + pose.y * resolucion), rojo);

            }
        }

    }
}

void set_image(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img) {
    for (unsigned int h = 1; h < sizeof (segundo->r) && h < PM_L_POINTS; h++) {
        float angulo1 = ((float) h - 1) * M_PI / 512 + PM_FI_MIN + pose.th;
        float angulo2 = ((float) h) * M_PI / 512 + PM_FI_MIN + pose.th;
        if ((segundo->r[h - 1] < 500) && (segundo->r[h - 1] > 30) && !((h > 23 && h < 50) || (h > 642 && h < 669))) {
            //                img.draw_line(
            //                    500 + temp_xx,
            //                    500 + temp_yy,
            //                    500 + (int) (segundo->r[i] * sin(angulo2)) + temp_xx,
            //                    500 + (int) (segundo->r[i] * cos(angulo2) + temp_yy), purple);
            //            unsigned char color[] = {(segundo->pendiente_x[h]*128) % 255, (segundo->pendiente_x[h]*64) % 255, 255, 255}; // Define a purple color    
            //            unsigned char color_esquina[] = {127, 127, 255, 255}; // Define a purple color    
            //            if (segundo->tipo[h] == 2)
            //                img.draw_line(
            //                    IMG_ANCHO / 2 + (int) (segundo->r[h - 1] * cos(angulo1)) + pose.x,
            //                    IMG_ALTO / 2 + (int) (segundo->r[h - 1] * sin(angulo1)) + pose.y,
            //                    IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2)) + pose.x,
            //                    IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) + pose.y), rojo);
            //            else
            float dx1 = ((segundo->r[h - 1] * cos(angulo1)) - (segundo->r[h] * cos(angulo2))) * resolucion;
            float dy1 = ((segundo->r[h - 1] * sin(angulo1)) - (segundo->r[h] * sin(angulo1))) * resolucion;
            float distancia = sqrt(dx1 * dx1 + dy1 * dy1);
            if (distancia < 5.0 * resolucion) {
                int x = IMG_ANCHO / 2 + (int) (segundo->r[h - 1] * resolucion * cos(angulo1)) + pose.x*resolucion;
                int dx = (int) (1 * cos(angulo1));
                int dy = (int) (1 * sin(angulo1));
                int y = IMG_ALTO / 2 + (int) (segundo->r[h - 1] * sin(angulo1) * resolucion) + pose.y*resolucion;
                int p_color = (int) img(x - dx, y - dy, 0, 0)*(int) img(x, y, 0, 0)*(int) img(x + dx, y + dy, 0, 0);
                //printf("p_color %d %f",p_color,p_color/127.0);
                if (p_color != 0) {
                    img.draw_line(
                            x,
                            y,
                            IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2) * resolucion) + pose.x*resolucion,
                            IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) * resolucion + pose.y * resolucion), black);
                }
            }
            /*
                        img.draw_line(
                                IMG_ANCHO / 2 + pose.x,
                                IMG_ALTO / 2 + pose.y,
                                IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2)) + pose.x,
                                IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) + pose.y), white);
             */
            //            if (segundo->tipo[h] == 2)
            //                img.draw_circle(IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2)) + pose.x,
            //                    IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) + pose.y), 5, color_esquina);



            //                char text2[200];
            //                sprintf(text2,
            //                        "/home/victor/Documentos/SALIDA/0/L/%s_%d.png",
            //                        archivos[i].substr(9, 13).c_str(),
            //                        h
            //                        );
            //                img.save(text2);
        }

    }
}

void set_image_promedio(PMScan *segundo, struct Pose pose, CImg<unsigned char>& img) {
    for (unsigned int h = 1; h < sizeof (segundo->r) && h < PM_L_POINTS; h++) {
        float angulo1 = ((float) h - 1) * M_PI / 512 + PM_FI_MIN + pose.th;
        float angulo2 = ((float) h) * M_PI / 512 + PM_FI_MIN + pose.th;
        if ((segundo->r[h - 1] < 500) && (segundo->r[h - 1] > 30) && !((h > 23 && h < 50) || (h > 642 && h < 669))) {
            float dx1 = ((segundo->r[h - 1] * cos(angulo1)) - (segundo->r[h] * cos(angulo2))) * resolucion;
            float dy1 = ((segundo->r[h - 1] * sin(angulo1)) - (segundo->r[h] * sin(angulo1))) * resolucion;
            float distancia = sqrt(dx1 * dx1 + dy1 * dy1);
            int x = IMG_ANCHO / 2 + (int) (segundo->r[h - 1] * resolucion * cos(angulo1)) + pose.x*resolucion;
            int y = IMG_ALTO / 2 + (int) (segundo->r[h - 1] * sin(angulo1) * resolucion) + pose.y*resolucion;
            if (distancia < 4.0 * resolucion) {

                float x2 = IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2) * resolucion) + pose.x*resolucion;
                float y2 = IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) * resolucion + pose.y * resolucion);
                float dx = (x2 - x) / (int) distancia;
                float dy = (y2 - y) / (int) distancia;
                for (int dr = 0; dr < (int) distancia; dr++) {
                    if (img(x + dr * dx, y + dr * dy, 0) > 10)
                        img(x + dr * dx, y + dr * dy, 0) += -10;
                }
                //int p_color = (int) img(x - dx, y - dy, 0, 0)*(int) img(x, y, 0, 0)*(int) img(x + dx, y + dy, 0, 0);
                //printf("p_color %d %f",p_color,p_color/127.0);
                /*
                                if (p_color != 0) {
                                    img.draw_line(
                                            x,
                                            y,
                                            IMG_ANCHO / 2 + (int) (segundo->r[h] * cos(angulo2) * resolucion) + pose.x*resolucion,
                                            IMG_ALTO / 2 + (int) (segundo->r[h] * sin(angulo2) * resolucion + pose.y * resolucion), black);
                                }
                 */
            } else {
                img(x, y, 0) = img(x, y, 1) = img(x, y, 3);
            }
        }

    }
}