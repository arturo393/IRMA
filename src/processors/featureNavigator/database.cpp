/***********************************************************************
* IRMA2: database.cpp
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
#include "database.h"


/*----------------DATABASE FUNCTIONS------------------ */
database::database()
{
    int i;
    for (i=0;i<numberOfTables;i++)
        tableArray[i] = NULL;

    usedTables = 0;
}

database::~database()
{
    int i;
    for (i=0;i<numberOfTables;i++)
        if (tableArray[i] != NULL)
        {
            delete tableArray[i];
            tableArray[i] = NULL;
        }

}

int database::createTable(char * tableName)
{
    if (usedTables < numberOfTables && searchTable(tableName) == notFound)
    {
        tableArray[usedTables] = new table(tableName);
        usedTables++;
        return (usedTables-1);
    }
    return -1;
}

bool database::deleteLastTable()
{
    if (usedTables > 0)
    {
        delete tableArray[usedTables];
        usedTables--;
        return true;
    }
    return false;
}

bool database::addColumn(int tableIndex, char * cName, int cType, bool cNotNull)
{
    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return notFound;
    if (tableArray[tableIndex] == NULL)
        return notFound;

    if(!tableArray[tableIndex]->createColumn(cName, cType, cNotNull))
        return false;

    return true;
}

bool database::addFeature(int tableIndex, feature * newFeature)
{
    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return notFound;
    if (tableArray[tableIndex] == NULL)
        return notFound;

    if(!tableArray[tableIndex]->createFeature(newFeature))
        return false;

    return true;
}

int database::getNumberOfColumns(int tableIndex)
{
    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return notFound;
    if (tableArray[tableIndex] == NULL)
        return notFound;

    return tableArray[tableIndex]->getUsedColumns();
}

int database::getNumberOfFeatures(int tableIndex)
{
    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return notFound;
    if (tableArray[tableIndex] == NULL)
        return notFound;

    return tableArray[tableIndex]->getUsedFeatures();
}

int database::searchTable(char * tableName)
{
    int i;
    for (i=0;i<numberOfTables;i++)
        if (tableArray[i] != NULL)
            if(strcmp(tableArray[i]->getName(),tableName) == 0)
                return i;
    return notFound;
}


int database::searchColumn (int tableIndex, char* columnName)
{
    if (tableIndex >= usedTables)
        return notFound;

    return tableArray[tableIndex]->getColumnIndex(columnName);
}

int database::getColumnType (int tableIndex, int columnIndex)
{
    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return notFound;
    if (tableArray[tableIndex] == NULL)
        return notFound;

    return tableArray[tableIndex]->getColumnType(columnIndex);
}

char * database::getColumnName (int tableIndex, int columnIndex)
{
    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return NULL;
    if (tableArray[tableIndex] == NULL)
        return NULL;

    return tableArray[tableIndex]->getColumnName(columnIndex);
}

feature database::getFeature(int tableIndex, int featureIndex)
{
    feature errorFeature;
    errorFeature.goodFeature = false;

    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return errorFeature;
    if (tableArray[tableIndex] == NULL)
        return errorFeature;

    return tableArray[tableIndex]->getFeature(featureIndex);
}

bool * database::simpleSelect(int tableIndex, int columnIndex, valueStruct value, int op)
{
    if (tableIndex < 0 || tableIndex > numberOfTables -1)
        return NULL;
    if (tableArray[tableIndex] == NULL)
        return NULL;

    return tableArray[tableIndex]->simpleSelect(columnIndex, op, value);
}

/*-------------------TABLE FUNCTIONS------------------ */
table::table(char * tableName)
{
    int i;

    name = new char[strlen(tableName)+1];
    strcpy(name, tableName);
    usedColumns = 0;
    usedFeatures = 0;

    for (i=0;i<numberOfColumns;i++)
        columnArray[i] = NULL;

    for (i=0;i<numberOfFeatures;i++)
        featureArray[i] = NULL;
}

table::~table()
{
    int i;

    for (i=0;i<numberOfColumns;i++)
        if (columnArray[i] != NULL)
        {
            delete columnArray[i];
            columnArray[i] = NULL;
        }

    for (i=0;i<numberOfFeatures;i++)
        if (featureArray[i] != NULL)
        {
            delete featureArray[i];
            featureArray[i] = NULL;
        }

}

char * table::getName(void)
{
    return name;
}

int table::getUsedColumns()
{
    return usedColumns;
}

int table::getUsedFeatures()
{
    return usedFeatures;
}

bool table::createColumn(char * cName, int cType, bool cNotNull)
{
    if (usedColumns < numberOfColumns && getColumnIndex(cName) == notFound)
    {
        columnArray[usedColumns] = new column(cName, cType, cNotNull);
        usedColumns++;
        return true;
    }
    return false;
}

bool table::createFeature(feature * newFeature)
{
    if (usedFeatures < numberOfFeatures)
    {
        featureArray[usedFeatures] = newFeature;
        usedFeatures++;
        return true;
    }
    return false;
}

int table::getColumnIndex(char * columnName)
{
    int i;
    for (i=0;i<numberOfColumns;i++)
        if (columnArray[i] != NULL)
            if(strcmp(columnArray[i]->getName(),columnName) == 0)
                return i;
    return notFound;
}

int table::getColumnType (int columnIndex)
{
    if (columnIndex < 0 || columnIndex > numberOfColumns -1)
        return notFound;
    if (columnArray[columnIndex] == NULL)
        return notFound;

    return columnArray[columnIndex]->getType();
}

char * table::getColumnName (int columnIndex)
{
    if (columnIndex < 0 || columnIndex > numberOfColumns -1)
        return NULL;
    if (columnArray[columnIndex] == NULL)
        return NULL;

    return columnArray[columnIndex]->getName();
}

feature table::getFeature (int featureIndex)
{
    feature errorFeature;
    errorFeature.goodFeature = false;

    if (featureIndex < 0 || featureIndex > numberOfFeatures -1)
        return errorFeature;

    return *featureArray[featureIndex];
}

feature getFeature (int featureIndex);

bool * table::simpleSelect(int columnIndex, int op, valueStruct value)
{

    if (columnIndex < 0 || columnIndex > numberOfColumns -1)
        return NULL;
    if (columnArray[columnIndex] == NULL)
        return NULL;

    int columnType = columnArray[columnIndex]->getType();
    if (value.type != columnType)
        return NULL;

    bool * output = new bool[usedFeatures];
    switch(op)
    {
        case op_equal:
        if (columnType == v_int)
        {
            for (int i=0; i<usedFeatures; i++)
            {
                if (featureArray[i]->valueArray[columnIndex].type == v_int)
                    output[i] = (featureArray[i]->valueArray[columnIndex].ival == value.ival);
                else if (featureArray[i]->valueArray[columnIndex].type == v_notDefined)
                    output[i] = 0;
                else
                    return NULL;
            }
        }
        else if (columnType == v_char)
        {
            for (int i=0; i<usedFeatures; i++)
            {
                if (featureArray[i]->valueArray[columnIndex].type == v_char)
                    output[i] = (strcmp(featureArray[i]->valueArray[columnIndex].sval, value.sval) == 0);
                else if (featureArray[i]->valueArray[columnIndex].type == v_notDefined)
                    output[i] = 0;
                else
                    return NULL;
            }
        }
        else
            return NULL;

        break;

        case op_notEqual:
        if (columnType == v_int)
        {
            for (int i=0; i<usedFeatures; i++)
            {
                if (featureArray[i]->valueArray[columnIndex].type == v_int)
                    output[i] = (featureArray[i]->valueArray[columnIndex].ival != value.ival);
                else if (featureArray[i]->valueArray[columnIndex].type == v_notDefined)
                    output[i] = 0;
                else
                    return NULL;
            }
        }
        else if (columnType == v_char)
        {
            for (int i=0; i<usedFeatures; i++)
            {
                if (featureArray[i]->valueArray[columnIndex].type == v_char)
                    output[i] = (strcmp(featureArray[i]->valueArray[columnIndex].sval, value.sval) != 0);
                else if (featureArray[i]->valueArray[columnIndex].type == v_notDefined)
                    output[i] = 0;
                else
                    return NULL;
            }
        }
        else
            return NULL;

        break;
    }

    return output;
}
/*----------------COLUMN FUNCTIONS------------------ */
column::column(char * cName, int cType, bool cNotNull)
{
    name = new char[strlen(cName)+1];
    strcpy(name, cName);
    type = cType;
    notNull = cNotNull;
}

column::~column()
{
}

char * column::getName(void)
{
    return name;
}

int column::getType(void)
{
    return type;
}


/*----------------FEATURE FUNCTIONS------------------ */
feature::feature()
{
    int i;

    goodFeature = true;

    for (i=0;i<numberOfColumns;i++)
    {
        valueArray[i].type = v_notDefined;
        //valueArray[i].sval = NULL;
    }
}

feature::~feature()
{/*
    int i;

    for (i=0;i<numberOfColumns;i++)
    {

        if (valueArray[i].type == v_char)
        {
            delete valueArray[i].sval;
            valueArray[i].ival = NULL;
        }
    }*/
}

