/***********************************************************************
* IRMA2: sql.h
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
#ifndef _SQL_H
#define _SQL_H

#include "database.h"
#include "../../common/configFile/configFile.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//----------------------------------------------------

#define MAX_FEATURES 30
#define numberOfKeywords 14
#define keywordsLength 10
#define maxTokenLength 50
#define tokenListLength 50

#define EOS '\0'
#define quotationMark '\''
#define specialTokens "(),*=<>"
#define identifierSpecialChars "_-."
#define blankString " "


#define noMoreTokens tokenListIndex+1 == nStoredTokens
#define checkNextTokenInList()     if (noMoreTokens)return false
#define numberOfItems(array) sizeof(array)/sizeof(array[0])


#define DONT_WRITE_IN_FILE false
#define firstToken  true
#define nextToken   false
#define token tokenBuffer
// Tokens

#define t_keyword       0
#define t_identifier    1
#define t_integer       2
#define t_string        3
#define t_special       4


// Keywords

#define k_create    0
#define k_table     1
#define k_char      2
#define k_int       3
#define k_not       4
#define k_null      5
#define k_insert    6
#define k_into      7
#define k_values    8
#define k_select    9
#define k_from      10
#define k_where     11
#define k_and       12
#define k_or        13


// Special Tokens

#define s_leftParen     1 // '('
#define s_rightParen    2 // ')'
#define s_coma          3 // ','
#define s_star          4 // '*'
#define s_equal         5 // '='
#define s_lessThan      6 // '<'
#define s_greaterThan   7 // '>'
#define s_notEqual      8 // '<>'

//  Statments

#define st_none     0
#define st_create   1
#define st_insert   2
#define st_select   3

//OperationNode types

#define n_none             0
#define n_operator         1
#define n_logicOperator    2
#define n_column           3
#define n_value            4

//Operation Node int Values
#define n_equal        5
#define n_notEqual     8
#define n_and          12
#define n_or           13

// Character Constants

#define c_coma      ','
#define c_none      'EOS'
#define c_leftParen '('
#define c_rightParen ')'
//#define SpecialTokens "(),"



//#define CREATE_TABLE sql
//#define INSERT_INTO(table, table.insert(


typedef struct columnStructMold
{
    char* name;
    int type;
    bool notNull;
    struct columnStructMold *next;
}columnStruct, *pColumnStruct;

typedef struct createStructMold
{
    char* tableName;
    pColumnStruct columnList;
}create_struct;

typedef struct insertStructMold
{
    int tableIndex;
    //int* columnIndexArray;
    feature* featureToBeInsert;
}insert_struct;

class operationNode
{
    private:

    public:
        // Constructor
        operationNode();
        // Destructor
        ~operationNode();

        operationNode * left;
        operationNode * right;
        int type;
        valueStruct value;
};
/*
typedef struct operationStructMold
{
    struct operationStructMold *left;
    struct operationStructMold *right;
    int type;
    valueStruct value;
}operationStruct, *pOperationStruct;
*/

typedef struct selectStructMold
{
    char * tableName;
    int tableIndex;
    int nFeatures;
    bool all;
    int columns[numberOfColumns];
    int nColumns;
    //pOperationStruct operationTree;
    operationNode * operationTree;
    valueStruct ** answer;
    int answerF;
    int answerC;
}select_struct;

class sql
{
private:

    database gis;
    create_struct createStruct;
    insert_struct insertStruct;
    select_struct selectStruct;
    bool isNewCommand;
    char tokenBuffer[maxTokenLength];

    valueStruct tokenList[tokenListLength];
    int nStoredTokens;
    int statement;
    FILE * dbFile;
    string db_name;

    bool lexicalAnalyser (const char * command);
        bool getToken (const char *buffer, const char *delimiters, const char *specTokens);
        bool identifyToken (void);
            bool isTokenKeyword (void);
            bool isTokenIdentifier (void);
            bool isTokenInteger (void);
            bool isTokenString (void);
            bool isTokenSpecial (void);

        int  isOneOf(char c, const char *list);
        void convertToUpperCase (char *);
        char itoa (int);

    bool parser(void);
        bool parsCreateStmt(void);
            bool parsCreateInput(int *);
        bool parsInsertStmt(void);
        bool parsSelectStmt(void);
            bool parsWhereInput(int * ptokenListIndex, operationNode * pNode);
            bool parsLogicalInput(int * ptokenListIndex, operationNode * pNode);
            bool parsWhereCondition(int * ptokenListIndex, operationNode * pNode);

    void freeTokenList(void);

    bool behaviorGenerator(void);
        bool doCreateStmt(void);
        bool doInsertStmt(void);
        bool doSelectStmt(void);
            bool * getOutput(operationNode * pNode);

        pColumnStruct pointToLastColumn(void);


public:
        // Constructor
        sql();
        // Destructor
        ~sql();
        // Funciones miembro o metodos
        int command (const char *, bool = true);
        valueStruct ** getSelectAnswer(int *, int *);
        bool checkPreviousDB(void);
};

#endif
