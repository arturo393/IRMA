/************************************************************************************
 * FILE: sql.cpp, v.1.0.0, 09/14/2009
 * Author: Renato Agurto.
 *
 * IRMA-2: Feature Navigator Process
 *
 * Database management and communication with the CMUcam3
 * for IRMA-2 project
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
************************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo, Renato Agurto  <tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/
//TODO: Mejorar el display de tokens

#include "sql.h"

// Constructor
sql::sql()
{
    createStruct.columnList = NULL;
    insertStruct.featureToBeInsert = NULL;
    selectStruct.operationTree = NULL;
    selectStruct.answer = NULL;

    //for (int i=0;i<tokenListLength;i++)
      //  tokenList[i].sval = NULL;

}

// Destructor
sql::~sql()
{
    fclose (dbFile);
}

bool sql::checkPreviousDB()
{
    char ch;
    char * line;
    line = new char[255];

    /************************ START: Config File reading.**************************/

    ConfigFile config( "config/featureNavigator.conf" );
    int db_mode;
    string default_db_name = "irmaDB";

    config.readInto( db_mode, "db_mode", 1 );
    config.readInto( db_name, "db_name", default_db_name);

    if ( db_mode > 1 || db_mode < 0)
        db_mode = 1;

   /************************END: Config File reading.****************************/


    dbFile = fopen (db_name.c_str(),"r");
    if (dbFile!=NULL)
    {
         if (db_mode == 1)
         {
             fseek(dbFile, 0, SEEK_SET);
             size_t lsize;

             while (getline(&line, &lsize,dbFile) != -1)
             {
                 command(line, DONT_WRITE_IN_FILE);
             }
             fclose (dbFile);
         }
         else if (db_mode == 0)
         {
             fclose (dbFile);
             dbFile = fopen (db_name.c_str(),"w");
             fclose (dbFile);
             return false;
         }

    }
    else
    {
        return false;
    }
    return true;

}

int sql::command (const char * command, bool writeInFile)
{
    statement = st_none;
    lexicalAnalyser(command);

    //Display the stored Tokens
    /*for (int i=0;i<nStoredTokens;i++)
    {
        if (isOneOf(itoa(tokenList[i].type), "024"))
            printf( "%d: %d es %d\n", i, tokenList[i].ival, tokenList[i].type);

        if (isOneOf(itoa(tokenList[i].type), "13"))
            printf( "%d: %s es %d\n", i, tokenList[i].sval, tokenList[i].type);
    }*/


    if(parser())
    {
        //printf("Correcto:\n");
        printf("SQL      - %s\n",command);
    }
    else
    {
        printf("SQL      - Error in SQL query\n");
        printf("           -> %s\n",command);
        return -1;
    }

    freeTokenList();
    if(behaviorGenerator())
    {

        if ((statement == st_create || statement == st_insert) && writeInFile)
        {
            dbFile = fopen (db_name.c_str(),"a");
            fprintf(dbFile,"%s\n",command);
            //fprintf(stdout,"Guardado en irmaDB\n",command);
            fclose (dbFile);
        }
    }


    return 0;
}

valueStruct ** sql::getSelectAnswer(int * F, int *C)
{
    valueStruct ** aux;
    aux = selectStruct.answer;
    selectStruct.answer = NULL;
    *C = selectStruct.answerC;
    *F = selectStruct.answerF;

    return aux;
}


/*----------------STEP 1: LEXICAL ANALYSIS------------------ */
bool sql::lexicalAnalyser (const char * command)
{
    //printf( "Command=%s\n", command );

    isNewCommand = true;
    nStoredTokens = 0;

    while (getToken(command, blankString, specialTokens))
    {
        if (identifyToken())
        {
            //printf( "%s : %d\n", token, tokenList[nStoredTokens].type);
            nStoredTokens++;
        }
    }

    return true;
}

int sql::isOneOf(char c, const char *list)
{
    unsigned int p;
    for (p=0; p<strlen(list); p++)
        if (c == *(list+p))
            return p+1;

    return 0;
}

bool sql::getToken(const char *buffer, const char *delimiters, const char *specTokens)
{
    int tokenIndex;
    static int bufferIndex;
    if (isNewCommand)
    {
        bufferIndex = 0;
        isNewCommand = false;
    }
    while (isOneOf(buffer[bufferIndex], delimiters) && (buffer[bufferIndex] != EOS))
        bufferIndex++;

    if (buffer[bufferIndex] == EOS)
        return false;

    if (isOneOf(buffer[bufferIndex], specTokens))
    {
        tokenBuffer[0] = buffer[bufferIndex];
        tokenBuffer[1] = EOS;
        bufferIndex++;
        if ((isOneOf(buffer[bufferIndex-1], "<")) && (buffer[bufferIndex] != EOS))
        {
            if (isOneOf(buffer[bufferIndex], ">"))
            {
                tokenBuffer[1] = buffer[bufferIndex];
                tokenBuffer[2] = EOS;
                bufferIndex++;
            }
        }

        return true;
    }

    if (buffer[bufferIndex] == quotationMark)
    {
        tokenIndex = 0;
        tokenBuffer[tokenIndex++] = buffer[bufferIndex++];

        while ((buffer[bufferIndex] != quotationMark) &&  (buffer[bufferIndex] != EOS))
            tokenBuffer[tokenIndex++] = buffer[bufferIndex++];

        if (buffer[bufferIndex] == EOS)
            return false; //error in string: end quotation mark is missing
        else
        {
            tokenBuffer[tokenIndex++] = buffer[bufferIndex++];
            tokenBuffer[tokenIndex] = EOS;
            return true;
        }
    }

    tokenIndex = 0;

    do
    {
        tokenBuffer[tokenIndex++] = buffer[bufferIndex++];
    }
    while (!isOneOf(buffer[bufferIndex], delimiters) && !isOneOf(buffer[bufferIndex], specTokens) && (buffer[bufferIndex] != EOS));

    tokenBuffer[tokenIndex] = EOS;

    return true;

}

bool sql::identifyToken()
{
    if (isalpha(tokenBuffer[0]))
    {
        if (isTokenKeyword() || isTokenIdentifier())
            return true;
    }

    else if (isdigit(tokenBuffer[0]))
    {
        if (isTokenInteger())
            return true;
    }

    else if (tokenBuffer[0] == quotationMark)
    {
        if (isTokenString())
            return true;
    }

    else if (isTokenSpecial())
        return true;

    return false;
}


bool sql::isTokenKeyword (void)
{
    char keywordTable[numberOfKeywords][keywordsLength] = {"CREATE", "TABLE", "CHAR", "INT", "NOT", "NULL", "INSERT", "INTO", "VALUES", "SELECT", "FROM", "WHERE", "AND", "OR"};
    int i;
    char cToken[maxTokenLength];

    strcpy(cToken, tokenBuffer);
    convertToUpperCase(cToken);

    for (i=0; i<numberOfKeywords; i++)
    {
        if (strcmp(cToken, keywordTable[i]) == 0)
        {
            tokenList[nStoredTokens].type = t_keyword;
            tokenList[nStoredTokens].ival = i;
            return true;
        }
    }
    return false;
}


bool sql::isTokenIdentifier (void)
{
    unsigned int i;
    if (!isalpha(tokenBuffer[0]))
        return false;

    for (i=1; i<strlen(tokenBuffer); i++)
    {
        if (!(isalnum(tokenBuffer[i]) || isOneOf(tokenBuffer[i], identifierSpecialChars)))
            return false;
    }

    tokenList[nStoredTokens].type = t_identifier;
    tokenList[nStoredTokens].sval = new char[strlen(tokenBuffer)+1];
    strcpy(tokenList[nStoredTokens].sval,tokenBuffer);
    return true;
}

bool sql::isTokenInteger (void)
{
    unsigned int i;

    for (i=0; i<strlen(tokenBuffer); i++)
    {
        if (!isdigit(tokenBuffer[i]))
            return false;
    }

    tokenList[nStoredTokens].type = t_integer;
    tokenList[nStoredTokens].ival = atoi(tokenBuffer);
    return true;
}

bool sql::isTokenString (void)
{
    unsigned int i;
    unsigned int len = strlen(tokenBuffer);
    if (tokenBuffer[0] != quotationMark)
        return false;

    if (!isalpha(tokenBuffer[1]))
        return false;

    for (i=2; i<len-1; i++)
    {
        if (!(isalnum(tokenBuffer[i]) || isOneOf(tokenBuffer[i], identifierSpecialChars)))
            return false;
    }

    if (tokenBuffer[len-1] != quotationMark)
        return false;

    tokenList[nStoredTokens].type = t_string;
    tokenList[nStoredTokens].sval = new char[len+1];
    strcpy(tokenList[nStoredTokens].sval,tokenBuffer);
    return true;
}

bool sql::isTokenSpecial (void)
{
    int r = isOneOf(tokenBuffer[0], specialTokens);

    if (r == 0)
        return false;

    tokenList[nStoredTokens].type = t_special;
    if (tokenBuffer[1] == EOS)
        tokenList[nStoredTokens].ival = r;
    else
    {
        tokenList[nStoredTokens].ival = s_notEqual;
    }//Change if there are more than one 2-char special token
    return true;


}

void sql::convertToUpperCase (char * str)
{
    unsigned int p;

    for (p=0; p<strlen(str); p++)
        str[p] = toupper(str[p]);
}

char sql::itoa (int x)
{
    char c[5];
    sprintf(c, "%d", x);
    return c[0];
}

/* ------------------------ STEP 2: PARSING ---------------------- */

bool sql::parser()
{
    if (tokenList[0].type != t_keyword)
        return false;

    switch (tokenList[0].ival)
    {
    case k_create:
        if(!parsCreateStmt())
            return false;
        statement = st_create;
        break;

    case k_insert:
        if(!parsInsertStmt())
            return false;
        statement = st_insert;
        break;

    case k_select:
        if(!parsSelectStmt())
            return false;
        statement = st_select;
        break;

    default:
        return false;
    }
    return true;
}

/* ------------------------ CREATE PARSING ---------------------- */
bool sql::parsCreateStmt(void)
{
    int tokenListIndex = 0;

    //State 2->3
    checkNextTokenInList();
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_keyword)
        return false;
    if (tokenList[tokenListIndex].ival != k_table)
        return false;

    //State 3->4
    checkNextTokenInList();
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_identifier)
        return false;

    //Set the createStruct table name
    createStruct.tableName = new char[strlen(tokenList[tokenListIndex].sval)+1];
    strcpy(createStruct.tableName,tokenList[tokenListIndex].sval);

    //State 4->5
    checkNextTokenInList();
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_special)
        return false;
    if (tokenList[tokenListIndex].ival != s_leftParen)
        return false;

    //State 5->6
    do
    {
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_identifier)
            return false;

        if (!parsCreateInput(&tokenListIndex))
            return false;

        //State 6 -> 5 OR 7
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_special)
            return false;
        if ((tokenList[tokenListIndex].ival != s_rightParen) && (tokenList[tokenListIndex].ival != s_coma))
            return false;
    }
    while (tokenList[tokenListIndex].ival == s_coma);//State 6->5

    //State 7

    if (noMoreTokens)
        return true;
    else
        return false;

}


bool sql::parsCreateInput(int * ptokenListIndex)
{
    int tokenListIndex = *ptokenListIndex;

    pColumnStruct temp;

    temp = pointToLastColumn();
    if (temp != NULL)
    {
        temp->next = new columnStruct;
        temp = temp->next;
    }
    else
    {
        temp =new columnStruct;
        createStruct.columnList = temp;
    }
    temp->next = NULL;
    temp->name = new char[strlen(tokenList[tokenListIndex].sval)+1];
    strcpy(temp->name,tokenList[tokenListIndex].sval);

    //State 2->(3 OR 6)
    checkNextTokenInList();
    tokenListIndex++;
    if (tokenList[tokenListIndex].type != t_keyword)
        return false;

    //State 2->3
    if (tokenList[tokenListIndex].ival == k_char)
    {
        temp->type = v_char;
        //State 3->4
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_special)
            return false;
        if (tokenList[tokenListIndex].ival != s_leftParen)
            return false;

        //State 4->5
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_integer)
            return false;


        //State 5->6
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_special)
            return false;
        if (tokenList[tokenListIndex].ival != s_rightParen)
            return false;
    }

    //State 2->6
    else if (tokenList[tokenListIndex].ival == k_int)
        temp->type = v_int;
    else
        return false;

    checkNextTokenInList();

    if (tokenList[tokenListIndex+1].type == t_keyword)
    {
        //State 6->7
        tokenListIndex++;

        if (tokenList[tokenListIndex].ival != k_not)
            return false;

        //State 7->8
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_keyword)
            return false;
        if (tokenList[tokenListIndex].ival != k_null)
            return false;

        temp->notNull = true;

    }
    else
        temp->notNull = false;

    //Final State (6 OR 8) reached
    *ptokenListIndex = tokenListIndex;
    return true;
}

/****************************INSERT PARSING ******************************************/
bool sql::parsInsertStmt(void)
{
    int tokenListIndex = 0;

    int maxNumberOfColumns;
    int columnCounter = 0;
    int valueCounter = 0;

    insertStruct.featureToBeInsert = new feature;

    //State 2->3
    checkNextTokenInList();
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_keyword)
        return false;
    if (tokenList[tokenListIndex].ival != k_into)
        return false;

    //State 3->4
    checkNextTokenInList();
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_identifier)
        return false;

    //Set the insertStruct table index
    insertStruct.tableIndex = gis.searchTable(tokenList[tokenListIndex].sval);
    if (insertStruct.tableIndex == notFound) //No table with the given name was found
        return false;

    //get the number of columns of the given table
    maxNumberOfColumns = gis.getNumberOfColumns(insertStruct.tableIndex);
    int columnIndexArray[maxNumberOfColumns];
    for (int i=0; i<maxNumberOfColumns; i++)
    {
        columnIndexArray[i] = -1;
        //insertStruct.featureToBeInsert->valueArray[i].type = v_notDefined; <-------------------------------????????????????????????????
    }


    //State 4-> ?
    checkNextTokenInList();

    if (tokenList[tokenListIndex+1].type == t_special)
    {
        //State 4->5
        tokenListIndex++;

        if (tokenList[tokenListIndex].ival != s_leftParen)
        return false;

        //State 5->6
        do
        {
            checkNextTokenInList();
            tokenListIndex++;

            if (tokenList[tokenListIndex].type != t_identifier)
                return false;

            if(columnCounter >= maxNumberOfColumns)
                return false;

            columnIndexArray[columnCounter] = gis.searchColumn(insertStruct.tableIndex, tokenList[tokenListIndex].sval);
            if (columnIndexArray[columnCounter] == notFound)
                return false;

            if (insertStruct.featureToBeInsert->valueArray[columnIndexArray[columnCounter]].type != v_notDefined)
                return false;

            insertStruct.featureToBeInsert->valueArray[columnIndexArray[columnCounter]].type = gis.getColumnType(insertStruct.tableIndex, columnIndexArray[columnCounter]);
            columnCounter++;



            //State 6 -> 5 OR 7
            checkNextTokenInList();
            tokenListIndex++;

            if (tokenList[tokenListIndex].type != t_special)
                return false;
            if ((tokenList[tokenListIndex].ival != s_rightParen) && (tokenList[tokenListIndex].ival != s_coma))
                return false;
        }
        while (tokenList[tokenListIndex].ival == s_coma);//State 6->5

        //State 7
        checkNextTokenInList();
    }
    else
    {
        columnCounter = maxNumberOfColumns;

        for (int i=0; i<maxNumberOfColumns; i++)
        {
            columnIndexArray[i] = i;
            insertStruct.featureToBeInsert->valueArray[i].type = gis.getColumnType(insertStruct.tableIndex, i);
        }
    }

    //State (7 OR 4) -> 8
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_keyword)
        return false;
    if (tokenList[tokenListIndex].ival != k_values)
        return false;

    //State 8->9
    checkNextTokenInList();
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_special)
        return false;
    if (tokenList[tokenListIndex].ival != s_leftParen)
        return false;

    do
    {
        //State 9->10
        checkNextTokenInList();
        tokenListIndex++;

        if(valueCounter >= columnCounter)
                return false;

        if (tokenList[tokenListIndex].type == t_integer && insertStruct.featureToBeInsert->valueArray[columnIndexArray[valueCounter]].type == v_int)
        {
            insertStruct.featureToBeInsert->valueArray[columnIndexArray[valueCounter]].ival = tokenList[tokenListIndex].ival;
        }
        else if (tokenList[tokenListIndex].type == t_string && insertStruct.featureToBeInsert->valueArray[columnIndexArray[valueCounter]].type == v_char)
        {
            insertStruct.featureToBeInsert->valueArray[columnIndexArray[valueCounter]].sval = new char[strlen(tokenList[tokenListIndex].sval)+1];
            strcpy(insertStruct.featureToBeInsert->valueArray[columnIndexArray[valueCounter]].sval,tokenList[tokenListIndex].sval);
        }
        else
        {
            return false;
        }

        valueCounter++;

        //State 10 -> 9 OR 11
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_special)
            return false;
        if ((tokenList[tokenListIndex].ival != s_rightParen) && (tokenList[tokenListIndex].ival != s_coma))
            return false;
    }
    while (tokenList[tokenListIndex].ival == s_coma);//State 10->9

    if (valueCounter != columnCounter)
        return false;

    //Check if state 11 is the final state
    if (noMoreTokens)
        return true;
    else
        return false;

}

/* ------------------------ SELECT PARSING ---------------------- */
bool sql::parsSelectStmt(void)
{
    int tokenListIndex = 0;
    char * columnNames[numberOfColumns];
    int columnNameIndex = 0; //columnNames index
    int maxNumberOfColumns;

    //State 2->(3 OR 4)
    checkNextTokenInList();

    if (tokenList[tokenListIndex+1].type == t_identifier)
    {

        selectStruct.all= false;
        for (int i=0; i<numberOfColumns; i++)
            selectStruct.columns[i]=-1;

        do
        {
            //2->3
            checkNextTokenInList();
            tokenListIndex++;

            if (tokenList[tokenListIndex].type != t_identifier)
                return false;

            //Save the column name
            if (columnNameIndex == numberOfItems(columnNames)) //If columnNames Array is full
                return false;
            columnNames[columnNameIndex] = new char[strlen(tokenList[tokenListIndex].sval)+1];
            strcpy(columnNames[columnNameIndex],tokenList[tokenListIndex].sval);
            columnNameIndex++;


            //State 3 -> 2 OR 5
            checkNextTokenInList();
            tokenListIndex++;

            if (tokenList[tokenListIndex].type == t_keyword)
            {
                if (tokenList[tokenListIndex].ival != k_from)
                    return false;
            }
            else if (tokenList[tokenListIndex].type == t_special)
            {
                if (tokenList[tokenListIndex].ival != s_coma)
                    return false;
            }
            else
                return false;
        }
        while (tokenList[tokenListIndex].ival == s_coma);//State 3->2

        //State 5
    }
    else if (tokenList[tokenListIndex+1].type == t_special)
    {
        //2->4
        tokenListIndex++;
        if (tokenList[tokenListIndex].ival != s_star)
            return false;

        selectStruct.all= true;

        //State 4->5
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_keyword)
            return false;
        if (tokenList[tokenListIndex].ival != k_from)
            return false;
    }
    else
        return false;

    //State 5->6
    checkNextTokenInList();
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_identifier)
        return false;

    //Set the selectStruct table name
    selectStruct.tableName = new char[strlen(tokenList[tokenListIndex].sval)+1];
    strcpy(selectStruct.tableName,tokenList[tokenListIndex].sval);

    //Set the selectStruct table index
    selectStruct.tableIndex = gis.searchTable(selectStruct.tableName);
    if (selectStruct.tableIndex == notFound) //No table with the given name was found
        return false;

    if (selectStruct.all == true)
    {
        //Select will show all columns in order

        //get the number of columns of the given table
        maxNumberOfColumns = gis.getNumberOfColumns(selectStruct.tableIndex);
        for (int i=0; i<maxNumberOfColumns; i++)
            selectStruct.columns[i] = i;

        selectStruct.nColumns = maxNumberOfColumns;
    }
    else
    {
        //Select will show the given columns in the given order
        for (int i=0; i<columnNameIndex; i++)
        {
            selectStruct.columns[i] = gis.searchColumn(selectStruct.tableIndex, columnNames[i]);
            if (selectStruct.columns[i] == notFound) //No column with the given name was found
                return false;
        }

        selectStruct.nColumns = columnNameIndex;
    }

    //Check if state 6 is the final state
    if (noMoreTokens)
        return true;

    //else
    //State 6->7
    tokenListIndex++;

    if (tokenList[tokenListIndex].type != t_keyword)
        return false;
    if (tokenList[tokenListIndex].ival != k_where)
        return false;

    //State 7->8
    checkNextTokenInList();
    tokenListIndex++;

    selectStruct.operationTree = new operationNode;

    if (!parsWhereInput(&tokenListIndex, selectStruct.operationTree))
        return false;

    //Check if state 8 is the final state
    if (noMoreTokens)
        return true;
    else
        return false;
}

bool sql::parsWhereInput(int * ptokenListIndex, operationNode * pNode)
{
    int tokenListIndex = *ptokenListIndex;

    //State 1->2

    pNode->left = new operationNode;

    if (!parsLogicalInput(&tokenListIndex, pNode->left))
        return false;

    //Check if state 2 is the final state
    if ((noMoreTokens)||(tokenList[tokenListIndex+1].type != t_keyword))
    {
        *ptokenListIndex = tokenListIndex;
        return true;
    }

    //State 2->3
    tokenListIndex++;

    pNode->type = n_logicOperator;
    pNode->value.type = v_int;

    if (tokenList[tokenListIndex].ival == k_and)
        pNode->value.ival = n_and;
    else if (tokenList[tokenListIndex].ival == k_or)
        pNode->value.ival = n_or;
    else
        return false;

    //State 3->4
    checkNextTokenInList();
    tokenListIndex++;

    pNode->right = new operationNode;

    if (!parsLogicalInput(&tokenListIndex, pNode->right))
        return false;

    //Final State 4 reached
    *ptokenListIndex = tokenListIndex;
    return true;
}

bool sql::parsLogicalInput(int * ptokenListIndex, operationNode * pNode)
{
    int tokenListIndex = *ptokenListIndex;

    if (tokenList[tokenListIndex].type == t_identifier) //State 1->4
    {
        if (!parsWhereCondition(&tokenListIndex, pNode))
            return false;
    }

    else if (tokenList[tokenListIndex].type == t_special)//State 1->2
    {
        if (tokenList[tokenListIndex].ival != s_leftParen)
            return false;

        //State 2->3
        checkNextTokenInList();
        tokenListIndex++;

        if (!parsWhereInput(&tokenListIndex, pNode))
            return false;

        //State 3->4
        checkNextTokenInList();
        tokenListIndex++;

        if (tokenList[tokenListIndex].type != t_special)
            return false;
        if (tokenList[tokenListIndex].ival != s_rightParen)
            return false;

    }

    else
        return false;

    //Final State 4 reached
    *ptokenListIndex = tokenListIndex;
    return true;
}

bool sql::parsWhereCondition(int * ptokenListIndex, operationNode * pNode)
{
    int tokenListIndex = *ptokenListIndex;

    pNode->left = new operationNode;
    pNode->right = new operationNode;

    //State 1
    int columIndex = gis.searchColumn(selectStruct.tableIndex, tokenList[tokenListIndex].sval);
    if (columIndex == notFound)
        return false;

    pNode->left->type = n_column;
    pNode->left->value.type = v_int;
    pNode->left->value.ival = columIndex;

    //State 2->3
    checkNextTokenInList();
    tokenListIndex++;

    pNode->type = n_operator;
    pNode->value.type = v_int;

    if (tokenList[tokenListIndex].type != t_special)
        return false;

    if (tokenList[tokenListIndex].ival == s_equal)
        pNode->value.ival = n_equal;
    else if (tokenList[tokenListIndex].ival == s_notEqual)
        pNode->value.ival = n_notEqual;
    else
        return false;


    //State 3->4
    checkNextTokenInList();
    tokenListIndex++;
    pNode->right->type = n_value;

    if (tokenList[tokenListIndex].type == t_integer)
    {
        pNode->right->value.type = v_int;
        pNode->right->value.ival = tokenList[tokenListIndex].ival;
    }
    else if (tokenList[tokenListIndex].type == t_string)
    {
        pNode->right->value.type = v_char;
        pNode->right->value.sval = new char[strlen(tokenList[tokenListIndex].sval)+1];
        strcpy(pNode->right->value.sval,tokenList[tokenListIndex].sval);
    }
    else
        return false;

    //Final State 4 reached
    *ptokenListIndex = tokenListIndex;
    return true;
}

/* ------------------------ STEP 3: BEHAVIOR GENERATION(?) ---------------------- */
bool sql::behaviorGenerator(void)
{
    switch (statement)
    {
    case st_create:
        if(!doCreateStmt())
            return false;
        break;

    case st_insert:
        if(!doInsertStmt())
            return false;
        break;

    case st_select:
        if(!doSelectStmt())
            return false;
        break;

    default:
        return false;
    }
    return true;
}

bool sql::doCreateStmt(void)
{
    pColumnStruct p,t;
    p = createStruct.columnList;

    int tableIndex;
    tableIndex = gis.createTable(createStruct.tableName);

    if (tableIndex != -1)
    {
        //printf("Table %s created\n",createStruct.tableName);
    }
    else
        printf("Error when trying to create Table %s \n",createStruct.tableName);

    delete [] createStruct.tableName;
    createStruct.tableName = NULL;

    while (p != NULL)
    {
        if (tableIndex != -1)// only if the table was successfully created, a column is added
        {
            if (gis.addColumn(tableIndex, p->name, p->type, p->notNull))
                ;//printf("Column %s added\n",p->name);
            else
                printf("Error when trying to add column %s \n",p->name);
        }
        t = p;
        p = p->next;
        delete [] t->name;
        delete t;
    }
    createStruct.columnList = NULL;

    if (tableIndex != -1)
        return true;
    else
        return false;
}

bool sql::doInsertStmt(void)
{
    if (!gis.addFeature(insertStruct.tableIndex, insertStruct.featureToBeInsert))
    {
        delete insertStruct.featureToBeInsert;
        insertStruct.featureToBeInsert = NULL;
        return false;
    }
    insertStruct.featureToBeInsert = NULL;
    return true;
}

bool sql::doSelectStmt(void)
{
    selectStruct.nFeatures = gis.getNumberOfFeatures(selectStruct.tableIndex);
    //printf("nFeatures = %d", selectStruct.nFeatures);

    if (selectStruct.nFeatures == 0)
    {
        selectStruct.answerC = selectStruct.nColumns;
        selectStruct.answerF = 0;
        return true;
    }

    bool * output = NULL;
    output = new bool[selectStruct.nFeatures];

    if (selectStruct.operationTree == NULL)
    {
        for (int i=0;i<selectStruct.nFeatures;i++)
            output[i] = 1;
    }
    else
    {
        output = getOutput(selectStruct.operationTree);
        delete selectStruct.operationTree;
    }

    feature feature_aux;
    if (output == NULL)
    {
        delete [] output;
        return false;
    }

    int c,f;
    int fCounter = 0;

    for (f=0;f<selectStruct.nFeatures;f++)
        if (output[f] == 1)
            fCounter++;

    selectStruct.answer = new valueStruct * [fCounter];

    for (f=0;f<fCounter;f++)
        selectStruct.answer[f] = new valueStruct[selectStruct.nColumns];
    //selectStruct.answer = new valueStruct[fCounter][selectStruct.nColumns];



    for (c=0;c<selectStruct.nColumns;c++)
    {
        printf("%s\t", gis.getColumnName(selectStruct.tableIndex,selectStruct.columns[c]));
    }
    printf("\n");

    fCounter = 0;
    for (f=0;f<selectStruct.nFeatures;f++)
        if (output[f] == 1)
        {
            feature_aux = gis.getFeature(selectStruct.tableIndex,f);
            for (c=0;c<selectStruct.nColumns;c++)
            {
                if (feature_aux.valueArray[selectStruct.columns[c]].type == v_int)
                    printf("%d\t", feature_aux.valueArray[selectStruct.columns[c]].ival);
                else if (feature_aux.valueArray[selectStruct.columns[c]].type == v_char)
                    printf("%s\t", feature_aux.valueArray[selectStruct.columns[c]].sval);
                else
                    printf("\t");

                selectStruct.answer[fCounter][c] = feature_aux.valueArray[selectStruct.columns[c]];
            }

            printf("\n");

            fCounter++;
        }

    selectStruct.answerC = selectStruct.nColumns;
    selectStruct.answerF = fCounter;

    delete [] output;
    return true;
}

bool * sql::getOutput(operationNode * pNode)
{
    switch (pNode->type)
    {
        case n_none:
            //printf("none\n");
            return getOutput(pNode->left);
            break;

        case n_operator:
            //printf("op\n");
            return gis.simpleSelect(selectStruct.tableIndex, pNode->left->value.ival, pNode->right->value, pNode->value.ival);
            break;

        case n_logicOperator:
        {
            //printf("logOp\n");
            bool *output = NULL;
            bool *leftOutput = NULL;
            bool *rightOutput = NULL;
            output = new bool[selectStruct.nFeatures];
            leftOutput = getOutput(pNode->left);
            rightOutput = getOutput(pNode->right);
            if ((leftOutput == NULL) || (rightOutput == NULL))
            {
                //if (leftOutput == NULL)
                  //  printf("left NULO\n");
                //if (rightOutput == NULL)
                  //  printf("right NULO\n");
                delete [] leftOutput;
                delete [] rightOutput;
                return NULL;
            }

            if (pNode->value.ival == n_and)
            {
                for (int i=0; i<selectStruct.nFeatures; i++)
                {
                    output[i] = (leftOutput[i] && rightOutput[i]);
                }
            }
            else if (pNode->value.ival == n_or)
            {
                for (int i=0; i<selectStruct.nFeatures; i++)
                {
                    output[i] = (leftOutput[i] || rightOutput[i]);
                }
            }
            else
            {
                delete [] leftOutput;
                delete [] rightOutput;
                return NULL;
            }

            delete [] leftOutput;
            delete [] rightOutput;
            return output;
            break;
        }

        default:
            return NULL;
    }
}

pColumnStruct sql::pointToLastColumn(void)
{
    pColumnStruct p;
    p = createStruct.columnList;
    if (p == NULL)
        return (NULL);
    else
        while (p->next != NULL)
            p = p ->next;

    return (p);
}

void sql::freeTokenList(void)
{
    for(int i=0;i<nStoredTokens;i++)
    if (tokenList[i].type == t_identifier || tokenList[i].type == t_string)
    {
        delete [] tokenList[i].sval;
    }
}



/* ---------------------------------------- operationNode Functions ---------------------------------------------- */
// Constructor
operationNode::operationNode()
{
    left  = NULL;
    right = NULL;
    type  = n_none;
}

// Destructor
operationNode::~operationNode()
{
    delete left;
    delete right;
    if (type == n_value)
        if(value.type == t_string)
            delete [] value.sval;
}
