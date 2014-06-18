/***********************************************************************
* IRMA2: gisFart.cpp
************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo     <tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/
#include "gisFart.h"

//Inicia los datos numericos con valores predefinidos.
gisFart::gisFart()
{
    activeNeurons=0;
    int i,j;
    for (i=0;i<4;i++)
        for (j=0;j<nNeurons;j++)
        {
            w[j][i]=1;
        }
}

gisFart::~gisFart()
{
}

int gisFart::set(int m_W, int m_H, int m_act_neurons, float ** m_w)
{
    mapWidth = m_W;
    mapHeight = m_H;
    map_activeNeurons = m_act_neurons;
    map_w = m_w;

    return 0;
}
//Traduce una coordenada del mapa a una coordenada FART
float gisFart::map2fart(int i, int coord)
{
    float f = (float)i;

    if (coord ==1)
        f /= mapWidth;
    else
        f /= mapHeight;
    return f;
}

//Ejecuta el algoritmo FART para las nuevas coordenadas(panel) de ingreso (X,Y)
int gisFart::newPoint(int x, int y, bool mode)
{
    int i,j;
    bool found;
    double T[nNeurons];
    double min[4];
    double Input[4];
    double sum1, sum2;
    int selectedNeuron=0, maxj=0;

    //Se define la entrada FART en la forma:
    //(Input[1],Input[2],Input[3],Input[4])=(x,y,x^c,y^c)
    //a partir de las entradas (X,Y) en coordenadas de panel.
    Input[0]= map2fart(x,1);
    Input[1]= map2fart(y,2);
    Input[2]=1-Input[0];
    Input[3]=1-Input[1];
    found = false;

    //Se verifica si alguna neurona activa(si es que hay) resuena con la entrada.
    if (activeNeurons>0)
    {
        for (j=0;j<activeNeurons;j++)
        {
            for (i=0;i<4;i++)
            {
                if (w[j][i]<Input[i])
                    min[i]=w[j][i];
                else
                    min[i]=Input[i];
            }
            sum1=min[0]+min[1]+min[2]+min[3];
            sum2=w[j][0]+w[j][1]+w[j][2]+w[j][3];
            T[j]=sum1/(Alpha+sum2);
        }

        int c=0;

        //Se busca la neurona que m�s resuene y se verifica si cumple con la condici�n de vigilancia.
        //Esto hasta que se encuentre una que cumpla o se hayan revisado todas las neuronas activas.
        while (!found && activeNeurons-c>0)
        {
            for (j=0;j<activeNeurons;j++)
                if (T[j]>T[maxj])
                    maxj=j;

            if (mode == RETURN_NEAREST_NH)
                return maxj;
            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  VIGILANTE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


            for (i=0;i<4;i++)
            {
                if (w[maxj][i]<Input[i])
                    min[i]=w[maxj][i];
                else
                    min[i]=Input[i];
            }

            bool stop = false;

            if ((min[0]+min[2] < 0.5) || (min[1]+min[3] < 0.5))
                stop = true;
            else
            {

                bool ux_in, vx_in, uy_in, vy_in, uvx_out, uvy_out;
                bool c0, c1, c2, c3;

                for (i=0;i<map_activeNeurons && !stop;i++)
                {
                    c0 = (min[0] < map_w[i][0]);
                    ux_in =  c0 && (map_w[i][0] < 1-min[2]);

                    c1 = (min[1] < map_w[i][1]);
                    uy_in = c1 && (map_w[i][1] < 1-min[3]);

                    if(ux_in && uy_in)
                    {
                        stop = true;
                    }
                    else
                    {
                        c2 = (map_w[i][2] > min[2]);
                        vx_in = (min[0] < 1-map_w[i][2]) && c2;

                        c3 = (map_w[i][3] > min[3]);
                        vy_in = (min[1] < 1-map_w[i][3]) && c3;

                        if(vx_in && vy_in)
                        {
                            stop = true;
                        }
                        else
                        {
                            uvx_out = (!c0) && (!c2);
                            uvy_out = (!c1) && (!c3);

                            if ((( ux_in || vx_in ) && uvy_out) || (( uy_in || vy_in ) && uvx_out))
                            {
                                stop = true;
                            }
                        }
                    }
                }
            }

            if (!stop)
                found = true;//Se encontro una neurona ya activa que resuena y cumple con el vigilante.
            else
            {
                T[maxj]=-1;
                c++;
            }
            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  VIGILANTE END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        }
    }

    //Si se encontr� (e==1) o no (e==0)
    if (found)
    {
        selectedNeuron = maxj; //escogido=nro o id. de la neurona escogida
    }
    else
    {
        selectedNeuron = activeNeurons;//escogido=nro o id. de la primera neurona no activa
        //Nota:neuronas escogidas van de 0 a [activas-1].

        activeNeurons ++;//Indica que hay una neurona activa m�s.
    }

    //Se actualizan los pesos de la neurona escogida
    for (i=0;i<4;i++)
    {
        if (w[selectedNeuron][i]<Input[i])
            min[i]=w[selectedNeuron][i];
        else
            min[i]=Input[i];
        w[selectedNeuron][i]=Beta*min[i];
    }

    return selectedNeuron;
}

int gisFart::printw()
{
    int i;
    for (i=0;i<activeNeurons;i++)
        printf("%f, %f, %f, %f\n",w[i][0],w[i][1],w[i][2],w[i][3]);

    return 0;
}

