//*******************************************************************
// Title: executive.cpp
// Authors: Tomas Arredondo, Gabriel Cisternas, Renato Agurto, Nicolas Navarro, Linus Casassa.
//
// First aproach of the IRMA-2 Executive Process
// Temporal management of the blackboard scheme, and software-hardware
// interaction layer for IRMA-2 proyect
//
// Authors: Tomas Arredondo, Gabriel Cisternas, Renato Agurto, Linus Casassa.
// Version: v.1.0.0
// Date: 07/30/2009
//
//
// Version: v.1.0.1
// Copyright (C) 2009 by Nicolas Navarro
// Contact: nicolas.navarro.guerrero@gmail.com or nicolas.navarro@gmx.de
// Date: November 2009
//
// DISCLAIMER:
// No liability is assumed by the author for any use made of this program.
//
// DISTRIBUTION:
// Any use may be made of this program, as long as the clear
// acknowledgment is made to the author in code and runtime executables
//*******************************************************************


//*******************************************************************
// Librery Declaration
#include "executive.h"




//*******************************************************************
// EXECUTIVE PROCESSOR: ExecutiveProcessor Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
//-------------------------------------------------------------------
struct stat st = {0};
float dt = 0.001;

float getRVel(float speed, float steering) {
    //printf("Speed MSP %f\n",speed);
    speed -= 0.5;
    //speed/=2.0;
    //speed/=4.0;
    steering -= 0.5;
    //steering/=-2.0;
    steering /= -1.0;
    float gVal = 0;
    if (steering > 0) {
        gVal = 0.5 + (speed);

    } else {
        gVal = 0.5 + (speed + steering);
    }
    gVal -= 0.5; // -0.5 a 0.5
    gVal *= 200; // -100   a 100
    return gVal;

}

float getLVel(float speed, float steering) {
    //printf("Speed MSP %f\n",speed);
    speed -= 0.5;
    //speed/=2.0;
    //speed/=4.0;
    steering -= 0.5;
    //steering/=-2.0;
    steering /= -1.0;
    float gVal = 0;
    if (steering < 0) {
        gVal = 0.5 + (speed);

    } else {
        gVal = 0.5 + (speed + steering);
    }
    gVal -= 0.5; // -0.5 a 0.5
    gVal *= 200; // -100   a 100
    return gVal;

}

void setLocalTime(char *salida) {
    timeval actual;
    gettimeofday(&actual, NULL);
    time_t t;
    t = actual.tv_sec;
    if (strftime(salida, 100, "%H%M%S", localtime(&t)) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    sprintf(salida, "%s_%06ld", salida, actual.tv_usec);
}

void ExecutiveProcessor::saveLog(string str) {
    char hora[100];
    setLocalTime(hora);
    fout << "\nt:";
    fout << hora;
    fout << str << "\n";
    fout.flush();
}

int ExecutiveProcessor::init() {


    pExecutive = (ExecutiveData *) cda.getMemPtr(EXECUTIVE_AREA);
    pNetwork = (NetworkData *) cda.getMemPtr(NETWORK_AREA);
    pMapper = (MapperData *) cda.getMemPtr(MAPPER_AREA);
    pLaser = (LaserData *) cda.getMemPtr(LASER_AREA);
    pMonitor = (MonitorData *) cda.getMemPtr(MONITOR_AREA);

    pCtrl = &(pExecutive->ctrl); // Todos los procesadores tienen que definir pCtrl
    sem_area = EXECUTIVE_AREA; // Todos los procesadores tienen que definir sem_area

    cda.lockArea(EXECUTIVE_AREA, control);
    pCtrl->loop = run_cmd;
    pExecutive->exec_move_ready_flag = true;

    cda.unlockArea(EXECUTIVE_AREA, control);

#ifdef LOG

    char filename[100];
    char outstr_folder[100];

    timeval actual;
    gettimeofday(&actual, NULL);
    time_t t;
    t = actual.tv_sec;
    if (strftime(outstr_folder, sizeof (outstr_folder), "%Y%2m%2d", localtime(&t)) == 0) {
        fprintf(stderr, "strftime returned 0 (UNO)");
        exit(EXIT_FAILURE);
    }

    if (stat(outstr_folder, &st) == -1) {
        mkdir(outstr_folder, 0700);
    }
    char outstr_file[100];
    setLocalTime(outstr_file);
    sprintf(filename, "%s/executive_%s.log", outstr_folder, outstr_file);

    fout.open(filename, std::ios::app); // open file for appending
    saveLog(string("Inicio Prueba"));

#endif
    /************************ START: Config File reading.**************************/

    ConfigFile config("config/executive.conf");
    //loadSplineModel();
    config.readInto(MAX_STEPS, "MAX_STEPS", 400);



    config.readInto(GPS_STEPS, "GPS_STEPS", 50);
    config.readInto(InitPosX, "InitPosX", 213);
    config.readInto(InitPosY, "InitPosY", 320);
    config.readInto(InitPosOrientation, "InitPosOrientation", 90);
    config.readInto(correction_mode, "correction_mode", 1);
    
    std::cout << InitPosX << std::endl;
    std::cout << InitPosY << std::endl;
    cda.lockArea(EXECUTIVE_AREA);
    pExecutive->path_length = MAX_STEPS;
    pExecutive->current_X =  InitPosX;
    pExecutive->current_Y =  InitPosY;
    cda.unlockArea(EXECUTIVE_AREA);
    /************************END: Config File reading.**************************/
    //   // Assignation of initial position and orientation
    //   cda.lockArea(EXECUTIVE_AREA);
    //      pExecutive->current_X = InitPosX;
    //      pExecutive->current_Y = InitPosY;
    //      pExecutive->current_orientation = InitPosOrientation;
    //      pExecutive->path[0][1] = InitPosX;
    //      pExecutive->path[0][2] = InitPosY;
    //      pExecutive->path[0][0] = InitPosOrientation;
    //   cda.unlockArea(EXECUTIVE_AREA);

    //   cda.lockArea(NETWORK_AREA);
    //      pNetwork->start_log_flag = true;
    //   cda.unlockArea(NETWORK_AREA);
    // fprintf(stdout, "\n move(0,0) \n");
#ifdef MANUAL_MOVE
    times = 1;
    movement_times = 0;
    temp_speed_percent = 0;
    temp_movement = 0;
#endif

    // serial->move_lrn(0,9);
    cout << "\n\n*****************\n";
    cout << "*Executive Start*\n";
    cout << "*****************\n";

    num_executed_steps = 0;
    orient_steps = 0;
    free_steps = true;

    //   if( GPS_STEPS != -1 )
    //   {
    //      fprintf(stdout, "Requesting to get GPS position...\n");
    //      //getGPSPosition();
    //      fprintf(stdout, "Request complete...\n");
    //      fprintf(stdout, "Acquired position: PosX=%i, PosY=%i, Orientation=%i\n", current_X, current_Y, current_orientation);
    //   }

    //   // Permite funcionar al mapper processor en modo stand-by.
    //   cda.lockArea(MAPPER_AREA);
    //      pMapper->ctrl.loop = run_cmd;
    //   cda.unlockArea(MAPPER_AREA);

    //   // Models loading
    //   loadLSQmodels();
    //   loadSplineModel();

    deltaXtotal = 0;
    deltaYtotal = 0;

    cda.lockArea(EXECUTIVE_AREA);
    pExecutive->exec_ready_flag = true;
    cda.unlockArea(EXECUTIVE_AREA);

    return (0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

int ExecutiveProcessor::step() {


    float speed, steer;
    int speed_percent = 0;
    int movement = 0;
    int current_nav;

    cda.lockArea(EXECUTIVE_AREA);
    move_ready_flag = pExecutive->exec_move_ready_flag;
    speed = pExecutive->exec_speed;
    steer = pExecutive->exec_steering;
    speed_percent = pExecutive->exec_speed_percent;
    movement = pExecutive->exec_movement;

    cda.unlockArea(EXECUTIVE_AREA);
    
        cda.lockArea(MONITOR_AREA);
        current_nav = pMonitor->monitor_current_nav;
        cda.unlockArea(MONITOR_AREA);
    

#ifdef MANUAL_MOVE
    if (use_manual_to_move) move_ready_flag = true;
#endif

    if (move_ready_flag == true) {
        
        
        getLaserReading(pExecutive, pLaser, 0, 0, 0);
	

#ifdef MANUAL_MOVE
        if (use_manual_to_move && times > movement_times) {
            times = 1;
            cout << "\n";
            cout << "Ingrese movimiento (forward=9 / right=1 / left=2 / back=3): ";
            cin >> temp_movement;
            cout << "Ingrese porcentaje: ";
            cin >> temp_speed_percent;
            cout << "Ingrese Repeticiones: ";
            cin >> movement_times;
            speed_percent = temp_speed_percent;
            movement = temp_movement;
        }
        if (times <= movement_times) {
            cout << "\n";
            cout << times << "/" << movement_times << " --  ";
            speed_percent = temp_speed_percent;
            movement = temp_movement;
            times++;
        }
int vR = 0;
int vL = 0;

    if (movement == 9){
        vR = speed_percent;
        vL = speed_percent;
        }
    else if(movement == 1){
        vR = -speed_percent;
        vL = speed_percent;
    }
    else if(movement == 2){
        vR = speed_percent;
        vL = -speed_percent;
    }
    else if(movement == 3){
        vR = -speed_percent;
        vL = -speed_percent;
    }
    
#else 
        int vR = getRVel(speed, steer);
        int vL = getLVel(speed, steer);
        if (speed > 1 || steer > 1) {
            vR = speed;
            vL = steer;
        }

#endif            
        

#ifdef LOG
        if (fout != NULL) {
            std::ostringstream ss;
            ss << "\t|speer:\t" << steer << "\t|speed:\t" << speed << "\t|";
            ss << "\t|vR:\t" << vR << "\t|vL:\t" << vL << "\t|";
            saveLog(ss.str());
        }
#endif

        
        
        pExecutive->on_moving = 1;
              
        if (current_nav == LRN) {
            serial->move_lrn(speed_percent, movement); //
            usleep(STEP_SIZE);
            cout << "\t";
            serial->move_lrn(0, 9);
        } else if (current_nav == CRN) {
            serial->move_v(vR, vL); //
            usleep(STEP_SIZE);
            cout << "\t";
            serial->move_v(0, 0);
        } else if (use_manual_to_move) {
            serial->move_lrn(speed_percent, movement); //
            usleep(STEP_SIZE);
            cout << "\t";
            serial->move_lrn(0, 9);
        }
        pExecutive->on_moving = 0;

#ifdef LOG
        if (fout != NULL) {
            std::ostringstream ss;
            ss << "\t|speer:\t0.5\t|speed:\t0.5\t|";
            saveLog(ss.str());
        }
#endif        

        num_executed_steps++;

        usleep(STEP_SIZE * 3); //
        getLaserReading(pExecutive, pLaser, 1, vR, vL);
        if (num_executed_steps < pExecutive->path_length) {
            // Writes a log for the robots position
            cda.lockArea(EXECUTIVE_AREA);
            // el principio es el (0,0)
            pExecutive->path[num_executed_steps][1] = pLaser->x;
            pExecutive->path[num_executed_steps][2] = pLaser->y;
            pExecutive->path[num_executed_steps][0] = pLaser->dir;
            pExecutive->current_X = pLaser->x + InitPosX;
            pExecutive->current_Y = pLaser->y + InitPosY;
            pExecutive->current_orientation = pLaser->dir;
            pExecutive->steps_number = num_executed_steps;

            cda.unlockArea(EXECUTIVE_AREA);
        }

        move_ready_flag = false;
        cda.lockArea(EXECUTIVE_AREA);
        pExecutive->exec_move_ready_flag = move_ready_flag;
        cda.unlockArea(EXECUTIVE_AREA);
    }

    return (0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

int ExecutiveProcessor::move() {
    // Move method checks that all the movement conditions are met
    bool move_ready_flag = false;
    float speed, steer;
    int speed_percent = 0;
    int movement = 0;
    int current_nav;

    cda.lockArea(EXECUTIVE_AREA);
    move_ready_flag = pExecutive->exec_move_ready_flag;
    speed = pExecutive->exec_speed;
    steer = pExecutive->exec_steering;
    speed_percent = pExecutive->exec_speed_percent;
    movement = pExecutive->exec_movement;
    cda.unlockArea(EXECUTIVE_AREA);

    cda.lockArea(MONITOR_AREA);
    current_nav = pMonitor->monitor_current_nav;
    cda.unlockArea(MONITOR_AREA);


    getLaserReading(pExecutive, pLaser, 0, 0, 0);
    // modelo se usa para indicar el orden de la localizacion por LSQ
    // 0 = cuarto orden
    // 1 = tercer orden
    // 2 = segundo orden (menor error cuadratico medio)
    // 3 = primer orden
    int modelo = 2;
    //*******************************************//
    //**NO INSERTAR NINGUN COMANDO EN ESTA ZONA**//
    //*******************************************//
    //        if (LocModel) //
    //            localizationSpline(speed, steer); //
    //        else //
    //            localizationLSQ(speed, steer, modelo); //
    //        //
#ifdef MANUAL_MOVE
    if (use_manual_to_move && times > movement_times) {
        times = 1;
        cout << "\n";
        cout << "Ingrese movimiento (forward=9 / right=1 / left=2 / back=3): ";
        cin >> temp_movement;
        cout << "Ingrese porcentaje: ";
        cin >> temp_speed_percent;
        cout << "Ingrese Repeticiones: ";
        cin >> movement_times;
        speed_percent = temp_speed_percent;
        movement = temp_movement;
    }
    if (times <= movement_times) {
        cout << "\n";
        cout << times << "/" << movement_times << " --  ";
        speed_percent = temp_speed_percent;
        movement = temp_movement;
        times++;
    }
#endif            
    int vR = getRVel(speed, steer);
    int vL = getLVel(speed, steer);
    if (speed > 1 || steer > 1) {
        vR = speed;
        vL = steer;
    }
    pExecutive->on_moving = 1;


#ifdef LOG
    if (fout != NULL) {
        std::ostringstream ss;
        ss << "\t|speer:\t" << steer << "\t|speed:\t" << speed << "\t|";
        ss << "\t|vR:\t" << vR << "\t|vL:\t" << vL << "\t|";
        saveLog(ss.str());
    }
#endif
    if (current_nav == LRN) {
        serial->move_lrn(speed_percent, movement); //
        usleep(STEP_SIZE);
        cout << "\t";
        serial->move_lrn(0, 9);
    } else if (current_nav == CRN) {
        serial->move_v(vR, vL); //
        usleep(STEP_SIZE);
        cout << "\t";
        serial->move_v(0, 0);
    } else if (use_manual_to_move) {
        serial->move_lrn(speed_percent, movement); //
        usleep(STEP_SIZE);
        cout << "\t";
        serial->move_lrn(0, 9);
    }

    pExecutive->on_moving = 0;
#ifdef LOG
    if (fout != NULL) {
        std::ostringstream ss;
        ss << "\t|speer:\t0.5\t|speed:\t0.5\t|";
        saveLog(ss.str());
    }
#endif        
    //*******************************************//

    //If the robot moved then the mapper should update th eposition and map (if applicable)
    num_executed_steps++;

    //      // Calls the GPS to re-localizate the robot
    //      orient_steps++;
    //      if( orient_steps == GPS_STEPS )
    //      {
    //         orient_steps = 0;
    //         fprintf(stdout, "Requesting to get GPS position...\n");
    //         getGPSPosition();
    //         fprintf(stdout, "Request complete...\n");
    //
    //         fprintf(stdout, "Acquired position: PosX=%i, PosY=%i, Orientation=%i\n", current_X, current_Y, current_orientation);
    //      }
    usleep(STEP_SIZE * 3); //
    getLaserReading(pExecutive, pLaser, 1, vR, vL);
    if (num_executed_steps < pExecutive->path_length) {
        // Writes a log for the robots position
        cda.lockArea(EXECUTIVE_AREA);
        // el principio es el (0,0)
        pExecutive->path[num_executed_steps][1] = pLaser->x;
        pExecutive->path[num_executed_steps][2] = pLaser->y;
        pExecutive->path[num_executed_steps][0] = pLaser->dir;
        pExecutive->current_X = pLaser->x + InitPosX;
        pExecutive->current_Y = pLaser->y + InitPosY;
        pExecutive->current_orientation = pLaser->dir;
        pExecutive->steps_number = num_executed_steps;

        cda.unlockArea(EXECUTIVE_AREA);
    }

    move_ready_flag = false;
    cda.lockArea(EXECUTIVE_AREA);
    pExecutive->exec_move_ready_flag = move_ready_flag;
    cda.unlockArea(EXECUTIVE_AREA);

    return (0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

int ExecutiveProcessor::cleanup() {
    return (0);
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::getEncoderState() {
    leftEncoder = serial->encoder(0);
    rightEncoder = serial->encoder(1);
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::getLaserReading(ExecutiveData *pExecutive, LaserData *pLaser, int estado, int vr, int vl) {
    //TODO:REVISAR ANTES DE MOVER QUE SE REALIZÓ LA LECTURA DEL LASER
    cda.lockArea(LASER_AREA);
    pLaser -> estado = estado;
    pLaser-> x = pExecutive->current_X;
    pLaser-> y = pExecutive->current_Y;
    pLaser-> dir = pExecutive->current_orientation;
    pLaser-> vr = vr;
    pLaser-> vl = vl;
    pLaser-> sensar = 1;

printf("vr %d, vl %d\n",vr,vl); 
    cda.unlockArea(LASER_AREA);
    while (true) {
        if (pLaser->sensar == 0) {
            cda.lockArea(LASER_AREA, control);
            cda.lockArea(EXECUTIVE_AREA, control);
            pExecutive->exec_adc[4] = getNormalized(pLaser->read_data[128] / 10);
            pExecutive->exec_adc[3] = getNormalized(pLaser->read_data[256] / 10);
            pExecutive->exec_adc[2] = getNormalized(pLaser->read_data[384] / 10);
            pExecutive->exec_adc[1] = getNormalized(pLaser->read_data[512] / 10);
            pExecutive->exec_adc[0] = getNormalized(pLaser->read_data[635] / 10);
#ifdef LOG
            if (fout != NULL) {
                std::ostringstream ss;
                ss << "\t|distancia:\t" << pExecutive->exec_adc[4] << "," << pExecutive->exec_adc[3] << "," << pExecutive->exec_adc[2] << "," << pExecutive->exec_adc[1] << "," << pExecutive->exec_adc[0];
                saveLog(ss.str());
            }
#endif
            cda.unlockArea(EXECUTIVE_AREA, control);
            cda.unlockArea(LASER_AREA, control);
            break;
        }
    }

}

float ExecutiveProcessor::getNormalized(float f) {
    if (f < 30)
        return 0.1;
    else if (f > 60)
        return 1;
    else
        return 0.1 + ((f - 30.0) / 60.0);
}

void ExecutiveProcessor::getSensorReadings(ExecutiveData *pExecutive) {
    fprintf(stdout, "\n getSensorReadings In \n");
    cda.lockArea(EXECUTIVE_AREA, control);
    pExecutive->exec_adc[0] = serial->sense(1);
    pExecutive->exec_adc[1] = serial->sense(2);
    pExecutive->exec_adc[2] = serial->sense(3);
    pExecutive->exec_adc[3] = serial->sense(4);
    pExecutive->exec_adc[4] = serial->sense(5);
    cda.unlockArea(EXECUTIVE_AREA, control);
    fprintf(stdout, "\n getSensorReadings Out \n");
    //	fprintf(stdout,"Sensor Readings: %f %f %f %f %f\n",
    //				pExecutive->exec_adc[0], pExecutive->exec_adc[1], pExecutive->exec_adc[2],
    //				pExecutive->exec_adc[3], pExecutive->exec_adc[4]);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::callMapper() {
    // allows the mapper to complete one cycle
    cda.lockArea(MAPPER_AREA);
    pMapper->mapping_flag = true;
    cda.unlockArea(MAPPER_AREA);

    while (pMapper->mapping_flag == true) {
        usleep(50);
    }
}


//-------------------------------------------------------------------
//*******************************************************************
//-------------------------------------------------------------------

void ExecutiveProcessor::localizationLSQ(float sp, float st, int model) {
    int i, j;
    float i11, i12, i22, i111, i112, i122, i222, i1111, i1112, i1122, i1222, i2222;

    float delta[3] = {0, 0, 0};
    int sizeof_delta = 3;

    // Input vectors.
    i11 = sp*sp;
    i12 = sp*st;
    i22 = st*st;
    i111 = pow(sp, 3);
    i112 = pow(sp, 2) * st;
    i122 = pow(st, 2) * sp;
    i222 = pow(st, 3);
    i1111 = pow(sp, 4);
    i1112 = pow(sp, 3) * st;
    i1122 = pow(sp, 2) * pow(st, 2);
    i1222 = pow(st, 3) * sp;
    i2222 = pow(st, 4);

    float vectorM4[15] = {1, sp, st, i11, i12, i22, i111, i112, i122, i222, i1111, i1112, i1122, i1222, i2222};
    int sizeof_vectorM4 = 15;

    float vectorM3[10] = {1, sp, st, i11, i12, i22, i111, i112, i122, i222};
    int sizeof_vectorM3 = 10;

    float vectorM2[6] = {1, sp, st, i11, i12, i22};
    int sizeof_vectorM2 = 6;

    float vectorM1[3] = {1, sp, st};
    int sizeof_vectorM1 = 3;

    //     modeloXX[variable_modelada][coeficiente]
    //     [variable_modelada]: 0 -> variacion de rotacion
    //                          1 -> variacion en X
    //                          2 -> variacion en Y
    //     delta[] responde al mismo orden anterior.
    //
    //     Lo siguiente corresponde a los coeficientesde cada modelo.
    //     Estos son leidos desde archivos.
    //     Los nombres de cada archivo corresponden con el
    //     nombre de las variables.
    //     modelo4[3][15]
    //     modelo3[3][10]
    //     modelo2[3][6];
    //     modelo1[3][3];
    switch (model) {
        case 0:
            for (i = 0; i < sizeof_delta; i++) {
                for (j = 0; j < sizeof_vectorM4; j++) {
                    delta[i] = delta[i]+(modelo4[i][j] * vectorM4[j]);
                }
            }
            break;
        case 1:
            for (i = 0; i < sizeof_delta; i++) {
                for (j = 0; j < sizeof_vectorM3; j++) {
                    delta[i] = delta[i]+(modelo3[i][j] * vectorM3[j]);
                }
            }
            break;
        case 2:
            for (i = 0; i < sizeof_delta; i++) {
                for (j = 0; j < sizeof_vectorM2; j++) {
                    delta[i] = delta[i]+(modelo2[i][j] * vectorM2[j]);
                }
            }
            break;
        case 3:
            for (i = 0; i < sizeof_delta; i++) {
                for (j = 0; j < sizeof_vectorM1; j++) {
                    delta[i] = delta[i]+(modelo1[i][j] * vectorM1[j]);
                }
            }
            break;
    }

    localization(delta);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::localizationSpline(float sp, float st) {
    int i, j, k;
    float delta[3];

    // Finding the correct index for the spline model array
    i = lround((100 * sp) - 20);
    j = lround((100 * st) - 20);

    for (k = 0; k < 3; k++)
        delta[k] = modeloSpline[i][j][k];

    localization(delta);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::localization(float delta[]) {
    int posX, posY, orientation;
    float deltax, deltay;

    cda.lockArea(EXECUTIVE_AREA);
    posX = pExecutive->current_X;
    posY = pExecutive->current_Y;
    orientation = pExecutive->current_orientation;
    cda.unlockArea(EXECUTIVE_AREA);

    printf("****************************************************************\n");
    printf("Position BEFORE last movement --> (x,y,orientation) = (%i,%i,%i)\n", posX, posY, orientation);

    // The model predicts the relative movement.
    // Hence, the coordinates must be rotated to fit the absolute coordinates.
    if (orientation > 180) {
        deltax = (delta[1] * cos((360 - orientation) * PI / 180)) + (delta[2] * sin((360 - orientation) * PI / 180));
        deltay = (-1 * delta[1] * sin((360 - orientation) * PI / 180)) + (delta[2] * cos((360 - orientation) * PI / 180));
    } else {
        deltax = (delta[1] * cos(-orientation * PI / 180)) + (delta[2] * sin(-orientation * PI / 180));
        deltay = (-1 * delta[1] * sin(-orientation * PI / 180)) + (delta[2] * cos(-orientation * PI / 180));
    }


    //    printf("Step-by-step absolut variations\n");
    //    printf("deltaX=%f, deltaY=%f\n",deltax,deltay);


    deltaXtotal += lround(deltax);
    deltaYtotal += lround(deltay);
    //    printf("Total absolut variations\n");
    //    printf("deltaX=%i, deltaY=%i\n",deltaXtotal,deltaYtotal);


    // Coordinates update
    posX = posX + lround(deltax);
    posY = posY + lround(deltay);

    orientation = orientation + lround(delta[0]);

    if (orientation > 360)
        orientation = orientation - 360;
    else if (orientation < 0)
        orientation = orientation + 360;

    //

    printf("Position AFTER  last movement --> (x,y,orientation) = (%i,%i,%i)\n", posX, posY, orientation);
    printf("****************************************************************");
    //

    cda.lockArea(EXECUTIVE_AREA);
    pExecutive->current_orientation = orientation;
    pExecutive->current_X = posX;
    pExecutive->current_Y = posY;
    cda.unlockArea(EXECUTIVE_AREA);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::loadLSQmodels() {
    FILE *modelos;
    int modelSize;
    int i, j, k;
    char temp[1024];

    ////     modeloXX[variable_modelada][coeficiente]
    ////     [variable_modelada]: 0 -> variacion de rotacion
    ////                          1 -> variacion en X
    ////                          2 -> variacion en Y
    ////     Lo siguiente corresponde a los coeficientesde cada modelo.
    ////     Estos son leidos desde archivos.
    ////     Los nombres de cada archivo corresponden con el
    ////     nombre de las variables.
    ////
    ////     Los archivos de modelos llevan todos los coeficientes para las tres variaciones
    ////     que se desean predecir (deltaX,deltaY,deltaRot).
    ////     Los coeficientes son todo escritos en el mismo archivo, en la misma fila.
    ////     Su orden es:
    ////                  beta0
    ////                  beta1
    ////                    ·
    ////                    ·
    ////                  betaxx -> varX
    ////                  beta0
    ////                  beta1
    ////                    ·
    ////                    ·
    ////                  betaxx -> varY
    ////                  beta0
    ////                  beta1
    ////                    ·
    ////                    ·
    ////                  betaxx -> varRot


    // All the needed files can be found int the "extra" folder.
    // k corresponds to the number of models to load.
    for (k = 0; k < 4; k++) {
        switch (k) {
            case 0:
                modelos = fopen("modelo4", "r");
                modelSize = sizeof (modelo4) / (3 * sizeof (float));
                break;
            case 1:
                modelos = fopen("modelo3", "r");
                modelSize = sizeof (modelo3) / (3 * sizeof (float));
                break;
            case 2:
                modelos = fopen("modelo2", "r");
                modelSize = sizeof (modelo2) / (3 * sizeof (float));
                break;
            case 3:
                modelos = fopen("modelo1", "r");
                modelSize = sizeof (modelo1) / (3 * sizeof (float));
                break;
        }

        if (modelos == NULL) {
            fprintf(stdout, "No se ha encontrado un archivo de modelos.\n");
            fprintf(stdout, "Compruebe su existencia.\n");
            exit(1);
        }

        switch (k) {
            case 0:
                for (i = 0; i < 3; i++)
                    for (j = 0; j < modelSize; j++)
                        fscanf(modelos, "%f", &modelo4[i][j]);
                break;

            case 1:
                for (i = 0; i < 3; i++)
                    for (j = 0; j < modelSize; j++)
                        fscanf(modelos, "%f", &modelo3[i][j]);
                break;

            case 2:
                for (i = 0; i < 3; i++)
                    for (j = 0; j < modelSize; j++)
                        fscanf(modelos, "%f", &modelo2[i][j]);
                break;

            case 3:
                for (i = 0; i < 3; i++)
                    for (j = 0; j < modelSize; j++)
                        fscanf(modelos, "%f", &modelo1[i][j]);
                break;
        }

        fclose(modelos);
    }
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::loadSplineModel() {
    FILE *modelos;
    int i, j, k;
    char temp[1024];

    ////     modeloSpĺine[entrada_sp][entrada_st][Var]
    ////     [entrada_sp]:  Rango 0-60 -> entradas para sp entre 0.2-0.8
    ////     [entrada_st]:  Rango 0-60 -> entradas para st entre 0.2-0.8
    ////     [Var]:
    ////                    0 -> valor de rotacion para las entradas indicadas
    ////                    1 -> valor de delta X para las entradas indicadas
    ////                    2 -> valor de delta Y para las entradas indicadas
    ////
    ////     Los valores son leidos desde el archivo modeloSpline, que contiene todos los valores necesarios.
    ////     Dichos valores estan escritos en orden de Rot(sp,st), VarX(sp,st), y VarY(sp,st)
    ////
    ////     Su orden es:
    ////                  Rot(0.2,0.2)
    ////                  Rot(0.2,0.21)
    ////                  Rot(0.2,0.22)
    ////                    ·
    ////                    ·
    ////                  Rot(0.8,0.8)
    ////                  VarX(0.2,0.2)
    ////                  VarX(0.2,0.21)
    ////                  VarX(0.2,0.22)
    ////                    ·
    ////                    ·
    ////                  VarX(0.8,0.8)
    ////                  VarY(0.2,0.2)
    ////                  VarX(0.2,0.21)
    ////                  VarX(0.2,0.22)
    ////                    ·
    ////                    ·
    ////                  VarX(0.8,0.8)


    // All the needed files can be found int the "extra" folder.
    // k corresponds to the number of models to load.
    modelos = fopen("modeloSpline", "r");
    if (modelos == NULL) {
        fprintf(stdout, "No se ha encontrado el archivo modeloSpline.\n");
        fprintf(stdout, "Compruebe su existencia.\n");
        exit(1);
    }
    for (k = 0; k < 3; k++) {
        for (i = 0; i < 61; i++)
            for (j = 0; j < 61; j++) {
                fscanf(modelos, "%f", &modeloSpline[i][j][k]);

            }
    }
    /*for (i = 0; i < 61; i++)
      for (j = 0; j < 61; j++)
      printf("(%d,%d)[%f,%f,%f]", i, j, modeloSpline[i][j][0], modeloSpline[i][j][1], modeloSpline[i][j][2]);
     */
    fclose(modelos);

}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::getGPSPosition() {
    int x0, y0;
    int x1, y1;

    int orient;
    float hip;
    int ca, co;

    do {
        //stores the model's prediction.
        cda.lockArea(EXECUTIVE_AREA);
        pExecutive->orientacion_previa = pExecutive->current_orientation;
        pExecutive->posX_previa = pExecutive->current_X;
        pExecutive->posY_previa = pExecutive->current_Y;
        cda.unlockArea(EXECUTIVE_AREA);

        // Moves three times straight backward and gets GPS position
        serial->move(0.5, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.2, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.5, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.2, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.5, 0.5);
        serial->move(0.2, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.5, 0.5);
        usleep(30 * STEP_SIZE);

        getCoordinate(&x0, &y0);
        //pNetwork->get_position_flag=true;
        //while (pNetwork->get_position_flag)
        //    usleep(5000);

        printf("\n");
        // Moves three times straight forward and gets GPS position
        serial->move(0.8, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.8, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.8, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.5, 0.5);
        serial->move(0.8, 0.5);
        usleep(STEP_SIZE);
        serial->move(0.5, 0.5);
        usleep(30 * STEP_SIZE);

        getCoordinate(&x1, &y1);

        // triangulate
        hip = sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2));
        ca = abs(x1 - x0);
        co = abs(y1 - y0);

        // Calculating new orientation
        // first cuadrant
        if (y1 >= y0 && x1 >= x0)
            orient = lround(acos(ca / hip)*180 / PI);
            //second cuadrant
        else if (y1 > y0 && x0 >= x1)
            orient = 180 - lround(acos(ca / hip)*180 / PI);
            //third cuadrant
        else if (x0 >= x1 && y0 >= y1)
            orient = lround(acos(ca / hip)*180 / PI) + 180;
            //fourth cuadrant
        else
            orient = 360 - lround(asin(co / hip)*180 / PI);
    } while (abs(orient - pExecutive->current_orientation) > MAX_DEV);

    //select interpolation method:
    if (correction_mode == 1) {
        adjust_interpolated(&x1, &y1, orient);
        fprintf(stdout, "Interpolation method: \n x:%i y:%i \n", x1, y1);

    } else if (correction_mode == 2) {
        adjust_constant(&x1, &y1);
        fprintf(stdout, "Constant method: \n x:%i y:%i \n", x1, y1);
    }

    cda.lockArea(EXECUTIVE_AREA);
    pExecutive->current_X = x1;
    pExecutive->current_Y = y1;
    pExecutive->current_orientation = orient;
    cda.unlockArea(EXECUTIVE_AREA);

    current_X = x1;
    current_Y = y1;
    current_orientation = orient;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::getCoordinate(int *x, int *y) {
    int x0[3];
    int y0[3];

    int errorX[3]; //error[0]->abs(x1-x0), error[1]->abs(x2-x1), error[2]->abs(x2-x0),
    int errorY[3]; //error[0]->abs(y1-y0), error[1]->abs(y2-y1), error[2]->abs(y2-y0),
    int i;

    int max_errorX, max_errorY;

    for (i = 0; i < 3; i++) {
        cda.lockArea(NETWORK_AREA);
        pNetwork->get_position_flag = true;
        cda.unlockArea(NETWORK_AREA);

        while (pNetwork->get_position_flag) usleep(100);

        cda.lockArea(EXECUTIVE_AREA);
        x0[i] = pExecutive->current_X;
        y0[i] = pExecutive->current_Y;
        cda.unlockArea(EXECUTIVE_AREA);
        printf("X[%i] = %i, Y[%i] = %i\n", i, x0[i], i, y0[i]);

        if (i == 1) {
            errorX[0] = abs(x0[1] - x0[0]);
            errorY[0] = abs(y0[1] - y0[0]);
        } else if (i == 2) {
            errorX[1] = abs(x0[2] - x0[1]);
            errorX[2] = abs(x0[2] - x0[0]);
            errorY[1] = abs(y0[2] - y0[1]);
            errorY[2] = abs(y0[2] - y0[0]);
        }
        usleep(9000);
    }

    if (errorX[0] >= errorX[1]) {
        if (errorX[0] >= errorX[2])
            max_errorX = 0;
        else
            max_errorX = 2;
    } else {
        if (errorX[1] >= errorX[2])
            max_errorX = 1;
        else
            max_errorX = 2;
    }

    if (errorY[0] >= errorY[1]) {
        if (errorY[0] >= errorY[2])
            max_errorY = 0;
        else
            max_errorY = 2;
    } else {
        if (errorY[1] >= errorY[2])
            max_errorY = 1;
        else
            max_errorY = 2;
    }

    *x = (x0[0] + x0[1] + x0[2] - x0[max_errorX]) / 2;
    *y = (y0[0] + y0[1] + y0[2] - y0[max_errorY]) / 2;
}


//-------------------------------------------------------------------
// Adjusts the GPS retrieved location (centroid) to the
// back wheel (model reference)
// the correction depends on the current location zone
//-------------------------------------------------------------------

void ExecutiveProcessor::adjust_constant(int *tempX, int *tempY) {
    if (*tempX <= 833 && *tempY <= 666) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 7;
            *tempY = *tempY - 77;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX + 109;
            *tempY = *tempY - 203;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX + 195;
            *tempY = *tempY - 86;
        } else {
            *tempX = *tempX + 94;
            *tempY = *tempY + 18;
        }
    } else if (*tempX <= 833 && *tempY > 666 && *tempY <= 1332) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX + 8;
            *tempY = *tempY - 139;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX + 116;
            *tempY = *tempY - 242;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX + 206;
            *tempY = *tempY - 126;
        } else {
            *tempX = *tempX + 116;
            *tempY = *tempY - 31;
        }
    } else if (*tempX <= 833 && *tempY > 1332) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 11;
            *tempY = *tempY - 94;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX + 87;
            *tempY = *tempY - 172;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX + 195;
            *tempY = *tempY - 77;
        } else
            *tempX = *tempX + 109;
    } else if (*tempX > 833 && *tempX <= 1666 && *tempY <= 666) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 139;
            *tempY = *tempY - 43;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX - 15;
            *tempY = *tempY - 164;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX + 105;
            *tempY = *tempY - 47;
        } else {
            *tempX = *tempX - 23;
            *tempY = *tempY + 87;
        }
    } else if (*tempX > 833 && *tempX <= 1666 && *tempY > 666 && *tempY <= 1332) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 131;
            *tempY = *tempY - 143;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX - 8;
            *tempY = *tempY - 247;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX + 112;
            *tempY = *tempY - 139;
        } else {
            *tempX = *tempX - 8;
            *tempY = *tempY - 31;
        }
    } else if (*tempX > 833 && *tempX <= 1666 && *tempY > 1332) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 101;
            *tempY = *tempY - 111;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX + 22;
            *tempY = *tempY - 189;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX + 127;
            *tempY = *tempY - 116;
        } else {
            *tempX = *tempX + 11;
            *tempY = *tempY - 29;
        }
    } else if (*tempX > 1666 && *tempY <= 666) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 229;
            *tempY = *tempY - 129;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX - 135;
            *tempY = *tempY - 220;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX - 30;
            *tempY = *tempY - 103;
        } else {
            *tempX = *tempX - 105;
            *tempY = *tempY - 21;
        }
    } else if (*tempX > 1666 && *tempY > 666 && *tempY <= 1332) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 244;
            *tempY = *tempY - 139;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX - 143;
            *tempY = *tempY - 242;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225) {
            *tempX = *tempX - 45;
            *tempY = *tempY - 147;
        } else {
            *tempX = *tempX - 150;
            *tempY = *tempY - 35;
        }
    } else if (*tempX > 1666 && *tempY <= 666) {
        if (pExecutive->orientacion_previa > 315 || pExecutive->orientacion_previa <= 45) {
            *tempX = *tempX - 199;
            *tempY = *tempY - 64;
        } else if (pExecutive->orientacion_previa > 45 && pExecutive->orientacion_previa <= 135) {
            *tempX = *tempX - 90;
            *tempY = *tempY - 155;
        } else if (pExecutive->orientacion_previa > 135 && pExecutive->orientacion_previa <= 225)
            *tempY = *tempY - 90;
        else {
            *tempX = *tempX - 113;
            *tempY = *tempY + 9;
        }
    }
    fprintf(stdout, "Constant Correction finished \n");
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void ExecutiveProcessor::setManualMove(bool manual_move) {
    use_manual_to_move = manual_move;
}

void ExecutiveProcessor::adjust_interpolated(int *tempX, int *tempY, int orientation) {
    //Define the weighting factor here
    int p = 2;

    //Define the central points of each quadrant
    float P_mid[2][10] = {
        {0, 417, 417, 417, 1250, 1250, 1250, 2083, 2083, 2083},
        {0, 334, 1000, 1666, 334, 1000, 1666, 334, 1000, 1666}
    };

    //Define correction factor tables for different angles:
    float T[2][10]; //used table of correction factors
    float T_1[2][10] = {
        { 0, -7, 8, -11, -139, -131, -101, -229, -244, -199},
        { 0, -77, -139, -94, -43, -143, -111, -129, -139, -64}
    };
    float T_2[2][10] = {
        { 0, 109, 116, 87, -15, -8, 22, -135, -143, -90},
        { 0, -203, -242, -172, -164, -247, -189, -220, -242, -155}
    };
    float T_3[2][10] = {
        { 0, 195, 206, 195, 105, 112, 127, -30, -45, 0},
        { 0, -86, -126, -77, -47, -139, -116, -103, -147, -90}
    };
    float T_4[2][10] = {
        { 0, 94, 116, 109, -23, -8, 11, -105, -150, -113},
        { 0, 18, -31, 0, 87, -31, -29, -21, -35, 9}
    };

    // Define the other used matrices or values
    float x_y_distance[2][10]; //Matrix for the x-y distances between the central points and the interpolation point
    float H[10]; //Vector for absolute point distances
    float W[10]; //Weighting vector
    float sum = 0; //Sum value for the sum of the inverted distances
    float correction_x = 0; // Correction value x
    float correction_y = 0; // Correction value y

    //  SELECTION OF THE TABLE TO BE APPLIED!!
    if (orientation > 315 || orientation <= 45) {
        for (int i = 0; i < 2; i++) { //copying the tables
            for (int j = 0; j < 10; j++) {
                T[i][j] = T_1[i][j];
            }
        }
    } else if (orientation > 45 && orientation <= 135) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 10; j++) {
                T[i][j] = T_2[i][j];
            }
        }
    } else if (orientation > 135 && orientation <= 225) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 10; j++) {
                T[i][j] = T_3[i][j];
            }
        }
    } else {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 10; j++) {
                T[i][j] = T_4[i][j];
            }
        }
    }
    // END OF SELECTION!!

    //CALCULATE THE X-Y DISTANCES
    for (int i = 1; i < 10; i++) {
        x_y_distance[0][i] = *tempX - P_mid[0][i];
        x_y_distance[1][i] = *tempY - P_mid[1][i];
    }
    //END OF X-Y CALCULATION

    //CALCULATE THE TOTAL POINT DISTANCES
    for (int i = 1; i < 10; i++) {
        H[i] = sqrt(x_y_distance[0][i] * x_y_distance[0][i] + x_y_distance[1][i] * x_y_distance[1][i]);
        if (H[i] == 0) { //except the 0 because it is impossible to divide through 0
            H[i] = 0.000001;
        }
    }
    //END OF CALCULATING THE DISTANCES

    //CALCULATE THE SUM W OF INVERTED DISTANCES
    for (int i = 1; i < 10; i++) {
        sum = sum + pow(H[i], -p);
    }
    //END OF CALCULATING SUM

    //CALCULATION OF WEIGHTING VECTOR
    for (int i = 1; i < 10; i++) {
        W[i] = pow(H[i], -p) / sum;
    }
    //END OF CALCULATING WEIGHTING VECTOR

    //WEIGHTING THE CORRECTION FACTORS
    for (int i = 1; i < 10; i++) {
        correction_x = W[i] * T[0][i] + correction_x;
        correction_y = W[i] * T[1][i] + correction_y;
    }
    //END OF WEIGHTING

    //Write correctet values into the CDA
    *tempX = *tempX + (int) correction_x;
    *tempY = *tempY + (int) correction_y;

    fprintf(stdout, "Interpolated Correction finished \n");

    //        //Output
    //        for (int i = 1; i<10; i++){
    //            cout<<W[i]<<endl;
    //        }
    //        cout<<correction_x<<endl;
    //        cout<<correction_y<<endl;
}
//*******************************************************************
// EXECUTIVE PROCESSOR: ExecutiveProcessor Class Implementation: END
//*******************************************************************


//-------------------------------------------------------------------
//-------------------------------------------------------------------

int main(int argc, char* argv[]) {
    fprintf(stdout, "\nIRMA2: Executive Process \n");
    int parametro = 1;
    ExecutiveProcessor exe;
    if (argc == 2) {
        parametro = atoi(argv[1]);
        exe.setManualMove(parametro == 1);
    }

    exe.execute();

    return (0);
}
