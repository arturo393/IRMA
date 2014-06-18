/***********************************************************************
* IRMA2: database.h
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
#ifndef _DATABASE_H
#define _DATABASE_H

#include <stdio.h>
#include <string.h>
//----------------------------------------------------

#define numberOfTables      3
#define numberOfColumns     7
#define numberOfFeatures    20

#define maxTableNameLength 30

#define notFound -1

//Value types
#define v_notDefined 0
#define v_int        1
#define v_char       2

//Select operation Values
#define op_equal       5
#define op_notEqual     8

//#define CREATE_TABLE sql
//#define INSERT_INTO(table, table.insert(

typedef struct valueMold
{
    int type;
    union
    {
        char* sval;
        int ival;
    };
}valueStruct, *pValueStruct;

class feature
{
private:


public:
        // Constructor
        feature();
        // Destructor
        ~feature();
        // Funciones miembro o metodos
        valueStruct valueArray[numberOfColumns];
        bool goodFeature;
};

class column
{
private:

    char* name;
    int type;
    bool notNull;

public:
        // Constructor
        column(char *, int, bool);
        // Destructor
        ~column();
        // Funciones miembro o metodos
        char * getName();
        int getType();
};

class table
{
private:

    char* name;
    column* columnArray[numberOfColumns];
    feature* featureArray[numberOfFeatures];
    int usedColumns;
    int usedFeatures;

public:
        // Constructor
        table(char *);
        // Destructor
        ~table();
        // Funciones miembro o metodos
        char * getName();
        int getUsedColumns();
        int getUsedFeatures();
        bool createColumn(char *, int, bool);
        bool createFeature(feature * newFeature);
        int getColumnIndex(char*);
        int getColumnType (int columnIndex);
        char * getColumnName (int columnIndex);
        feature getFeature (int featureIndex);
        bool * simpleSelect(int columnIndex, int op, valueStruct value);

};

class database
{
private:

    table* tableArray [numberOfTables];
    int usedTables;

public:
    // Constructor
    database();
    // Destructor
    ~database();

    // Funciones miembro o metodos
    int createTable(char *);
    bool deleteLastTable(); //Eventually this function can be replaced by a deleteTable(char *) function
    bool addColumn(int tableIndex, char *, int, bool);
    bool addFeature(int tableIndex, feature * newFeature);
    int getNumberOfColumns(int tableIndex);
    int getNumberOfFeatures(int tableIndex);
    int searchTable (char *);
    int searchColumn (int tableIndex, char* columnName);
    int getColumnType (int tableIndex, int columnIndex);
    char * getColumnName (int tableIndex, int columnIndex);
    feature getFeature (int tableIndex, int featureIndex);
    bool * simpleSelect(int tableIndex, int columnIndex, valueStruct value, int op);
};


#endif
