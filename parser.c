/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  parser.c
 *
 *
 *  xvacul30 - Jan Vaculik
 *  xmensi13 - Jan Mensik
 *
 *  xtrejt00 - Martin Trejtnar
 *
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "symtable.h"
#include "ilist.h"
#include "scanner.h"
#include "parser.h"
#include "stack.h"


#define EXPR    200
#define COND    201

#define T_SIZE 7
#define RDC     44
#define SFT     42
#define ERR     45
#define EQL     43
#define ACC     46

#define DIVISION_BY_ZERO    9

int Precedence_table[T_SIZE][T_SIZE] =
{
    // MEDUNAS
    //      |  0  |  1  |  2  |  3  |  4  |  5  |  6
    //      |  +- |  */ |  ID |  (  |  )  |  R  |  $  |
    /*0*/ {RDC, SFT, SFT, SFT, RDC, RDC, RDC}, // +-
    /*1*/ {RDC, RDC, SFT, SFT, RDC, RDC, RDC}, // */
    /*2*/ {RDC, RDC, ERR, ERR, RDC, RDC, RDC}, // ID
    /*3*/ {SFT, SFT, SFT, SFT, EQL, SFT, ERR}, // (
    /*4*/ {RDC, RDC, ERR, ERR, RDC, RDC, RDC}, // )
    /*5*/ {SFT, SFT, SFT, SFT, RDC, RDC, RDC}, // R relační operátory (<, >, <=, >=, ==, !=)
    /*6*/ {SFT, SFT, SFT, SFT, ERR, SFT, ACC}  // $
};

int expr_type;
bool left_side = true;  //promenna pro rozhodnuti, zda jsme na leve nebo prave strane vyrazu

tTableList *TL;
tSymbolTable *table;        // globalni promenna uchovavajici tabulku symbolu
tListOfInstr *list;         // globalni promenna uchovavajici seznam instrukci
int token;                  // globalni promenna, ve ktere bude ulozen aktualni token
string attr;                // globalni promenna, ve ktere bude ulozen atribut tokenu
string var;                 // globalni promenna, kterou pouzivame pro tvorbu pomocnych promennych
tTableItem *varZero;        // pomocna promenna pro PSA

int counterVar = 1;         // pocitadlo pro tisknutelne pomocne promenne
int counterVar2 = 1;        // pocitadlo pro netisknutelne pomocne promenne
int whileCounter = 1;       // pocitadlo pro WHILE labels
int endCounter = 1;         // pocitadlo pro END labels
int elseCounter = 1;        // pocitadlo pro ELSE labels

int errorVar = SYNTAX_OK;   // promenna uchovavajici navratovou hodnotu parseru
int currentFrame = GF;      // promenna slouzici k poznani soucasneho ramce



void generateVariable(string *variable)
// generuje jedinecne nazvy identifikatoru se znakem $

{
    strClear(variable);
    strAddChar(variable, '$');
    int i;
    i = counterVar;
    while (i != 0)
    {
        strAddChar(variable, (char)(i % 10 + '0'));
        i = i / 10;
    }
    counterVar ++;
}

void generateVariable2(string *variable)
// generuje jedinecne nazvy identifikatoru se znakem *

{
    strClear(variable);
    strAddChar(variable, '*');
    int i;
    i = counterVar2;
    while (i != 0)
    {
        strAddChar(variable, (char)(i % 10 + '0'));
        i = i / 10;
    }
    counterVar2 ++;
}

void generateElseLabel(string *variable)
// generuje jedinecne nazvy pro else labels

{
    strClear(variable);
    strAddChar(variable, '%');
    strAddChar(variable, 'e');
    strAddChar(variable, 'l');
    strAddChar(variable, 's');
    strAddChar(variable, 'e');
    
    int i;
    i = elseCounter;
    while (i != 0)
    {
        strAddChar(variable, (char)(i % 10 + '0'));
        i = i / 10;
    }
    elseCounter ++;
}

void generateWhileLabel(string *variable)
// generuje jedinecne nazvy pro while labels

{
    strClear(variable);
    strAddChar(variable, '%');
    strAddChar(variable, 'w');
    strAddChar(variable, 'h');
    strAddChar(variable, 'i');
    strAddChar(variable, 'l');
    strAddChar(variable, 'e');
    
    int i;
    i = whileCounter;
    while (i != 0)
    {
        strAddChar(variable, (char)(i % 10 + '0'));
        i = i / 10;
    }
    whileCounter ++;
}

void generateEndLabel(string *variable)
// generuje jedinecne nazvy pro end labels

{
    strClear(variable);
    strAddChar(variable, '%');
    strAddChar(variable, 'e');
    strAddChar(variable, 'n');
    strAddChar(variable, 'd');
    
    int i;
    i = endCounter;
    while (i != 0)
    {
        strAddChar(variable, (char)(i % 10 + '0'));
        i = i / 10;
    }
    endCounter ++;
}


void generateInstruction(int instType, void *addr1, void *addr2, void *addr3)
// vlozi novou instrukci do seznamu instrukci
// nastavuje spravny ramec pro pracovni globalni promennou *0

{
    tInstr I;
    I.instType = instType;
    
    I.addr1 = addr1;
    I.addr2 = addr2;
    I.addr3 = addr3;
    
    I.frame = currentFrame;
    if (I.addr1 != NULL) {
        if ((strncmp(((tTableItem*)I.addr1)->key.str, "*0", 2)) == 0) {
            I.frame = GF;
        }
    }
    
    I.frame2 = currentFrame;
    if (I.addr2 != NULL) {
        if ((strncmp(((tTableItem*)I.addr2)->key.str, "*0", 2)) == 0) {
            I.frame2 = GF;
        }
    }
    if (I.addr3 != NULL) {
        I.frame3 = currentFrame;
        if ((strncmp(((tTableItem*)I.addr3)->key.str, "*0", 2)) == 0) {
            I.frame3 = GF;
        }
    }
    
    listInsertLast(list, I);
}

void generateEmpty()
// generuje prazdnou pomocnou promennou pro jednodussi volani funkce bez prirazeni

{
    tTableItem *variableInfo;
    strClear(&var);
    
    generateVariable(&var);
    tableInsert(table, table->root, &var);
    variableInfo = tableSearch(table, table->root, &var);
    generateInstruction(I_DEFVAR, (void *) variableInfo, NULL, NULL);
    void *currentInstruction;
    currentInstruction = listGetPointerLast(list);
    listGoto(list, currentInstruction);
}


bool statList()
// funkce pro neterminal <statList>

{
    bool result;
    token = getNextToken(&attr);

    if (token == NEW_LINE)
    {
        return statList();
    }
    // pravidlo <statList> -> ε
    else if (token == END_OF_FILE || token == END || token == ELSE)
    {
        return true;
    }
    else
    // pravidlo <statList> -> <statement> \n <statList>
    {
        result = statement();
        if (!result) return false;
        if (token == END_OF_FILE) return true;
        if (token != NEW_LINE) return false;
        return statList();
    }
}


bool statement()
// funkce pro neterminal <statement>

{
    tTableItem *variableInfo;
    int result;
    int found;
    int fn;
    strClear(&var);
    void *currentInstruction;
    
    if (token == NEW_LINE) return true;
    if (token == END_OF_FILE) return true;
    
    switch (token)
    {
        case ID:
        //pravidlo <statement> -> ID <IDexpr> <statement>
            
            found = tableInsert(table, table->root, &attr);

            if(found == 0)
            {
                variableInfo = tableSearch(table, table->root, &attr);
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if (token == ASSIGN)
                {
                    generateInstruction(I_DEFVAR, (void *) variableInfo, NULL, NULL);
                    generateInstruction(I_MOVE, (void *) variableInfo, NULL, NULL);
                    void *currentInstruction;
                    currentInstruction = listGetPointerLast(list);
                    listGoto(list, currentInstruction);
                }
                
                else
                {
                    generateInstruction(I_VAL, (void *) variableInfo, NULL, NULL);
                    void *currentInstruction;
                    currentInstruction = listGetPointerLast(list);
                    listGoto(list, currentInstruction);
                    result = callFunction();
                    if (!result) return false;
                }
            }
            
            else
            {
                variableInfo = tableSearch(table, table->root, &attr);

                generateInstruction(I_VAL, (void *) variableInfo, NULL, NULL);
                void *currentInstruction;
                currentInstruction = listGetPointerLast(list);
                listGoto(list, currentInstruction);
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                if (token != ASSIGN)
                {
                    if (variableInfo->function) {
                        result = callFunction();
                        if (!result) return false;
                    }
                }
            }
            
            return (idExpr() && statement());
            
            break;
            
        case WRITE:
        // pravidlo <stat> -> "WRITE" <writeExpr>
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            
            if (token == L_BRACKET)
            // pravidlo <writeExpr> -> '(' <term> <otherTerms> ')'
            {

                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                generateInstruction(I_WRITE, NULL, NULL, NULL);
                void *currentInstruction;
                currentInstruction = listGetPointerLast(list);
                listGoto(list, currentInstruction);

                if((result = term()) == false) return false;
                
                if((result = otherTerms()) == false) return false;
                
                if (token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if (token != NEW_LINE) {errorVar = SYNTAX_ERROR; return false;}
                
                return true;
            }
            
            else
            //else pravidlo <writeExpr> -> <term> <otherTerms>
            {
                
                generateInstruction(I_WRITE, NULL, NULL, NULL);
                void *currentInstruction;
                currentInstruction = listGetPointerLast(list);
                listGoto(list, currentInstruction);
               
                if((result = term()) == false) return false;
                if((result = otherTerms()) == false) return false;
                
                return true;
            }
            
            break;
            
        case WHILE:
        // pravidlo <stat> -> "WHILE" <expr> "DO" \n <statList>
            
            generateVariable2(&var);
            tableInsert(table, table->root, &var);
            tTableItem *toAssign;
            toAssign = tableSearch(table, table->root, &var);
            generateInstruction(I_DEFVAR, (void *) toAssign, NULL, NULL);
            
            generateWhileLabel(&var);
            tableInsert(table, table->root, &var);
            variableInfo = tableSearch(table, table->root, &var);
            generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
            
            tInstr *addrOfLab1;
            addrOfLab1 = listGetPointerLast(list);
            
            listGoto(list, addrOfLab1);
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            result = expr(NULL, table, toAssign, COND);
            if (result != SYNTAX_OK) return false;
            
            if (token != DO){errorVar = SYNTAX_ERROR; return false;}
           
            tInstr *endJump = listGetPointerLast(list);
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            if (token != NEW_LINE) {errorVar = SYNTAX_ERROR; return false;}
            result = statList();
            if (!result) return false;
            if (token != END){errorVar = SYNTAX_ERROR; return false;}
            
            generateInstruction(I_JMP, (void *) variableInfo, NULL, NULL);
            tInstr *whileJump = listGetPointerLast(list);
            
            generateEndLabel(&var);
            tableInsert(table, table->root, &var);
            variableInfo = tableSearch(table, table->root, &var);
            generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
            endJump->addr1 = (void *) variableInfo;
            tInstr *addrofEnd = listGetPointerLast(list);
            
            
            if ((token = getNextToken(&attr)) == LEX_ERROR) return false;
            if (token != NEW_LINE) {errorVar = SYNTAX_ERROR; return false;}
            
            tInstr *addrofLab2 = NULL;
            tInstr *whileJump2 = NULL;
            tInstr *endJump2 = NULL;
            tInstr *addrofEnd2 = NULL;
            
            listGoto(list, addrOfLab1);
            listNext(list);
            tInstr *tmp = listGetData(list);
            
            generateWhileLabel(&var);
            tableInsert(table, table->root, &var);
            variableInfo = tableSearch(table, table->root, &var);
            generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
            addrofLab2 = listGetPointerLast(list);
            
            generateEndLabel(&var);
            tableInsert(table, table->root, &var);
            variableInfo = tableSearch(table, table->root, &var);
            
            while (tmp != NULL) {
                if (tmp->instType == I_DEFVAR)
                {
                    listNext(list);
                    tmp = listGetData(list);
                    
                    if (tmp->instType == I_MOVE && ((tTableItem*)tmp->addr2)) {
                        listNext(list);
                        tmp = listGetData(list);
                    }
                }
                
                
                else if (tmp == endJump)
                {
                    generateInstruction(I_JMPNQ, NULL, ((tTableItem*)tmp->addr2) , ((tTableItem*)tmp->addr3));
                    endJump2 = listGetPointerLast(list);
                    listNext(list);
                    tmp = listGetData(list);
                    if (tmp->instType == I_MOVE && tmp->addr2 == NULL) {
                        listNext(list);
                        tmp = listGetData(list);
                    }
                }
                
                else if (tmp == whileJump)
                {
                    generateInstruction(I_JMP, NULL, NULL, NULL);
                    tmp->instType = I_VAL;
                    whileJump2 = listGetPointerLast(list);
                    listNext(list);
                    tmp = listGetData(list);
                }
                
                else if (tmp == addrofEnd)
                {
                    generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
                    addrofEnd2 = listGetPointerLast(list);
                    break;
                }
                
                else if (tmp->instType == I_LAB) {
                    
                    if ((strncmp(((tTableItem*)tmp->addr1)->key.str, "%end", 4)) == 0) {
                        
                        generateEndLabel(&var);
                        tableInsert(table, table->root, &var);
                        tTableItem *buffer = tableSearch(table, table->root, &var);
                        generateInstruction(I_LAB, (void *) buffer, NULL, NULL);
                        
                    }
                    if ((strncmp(((tTableItem*)tmp->addr1)->key.str, "%else", 5)) == 0) {
                    
                        generateElseLabel(&var);
                        tableInsert(table, table->root, &var);
                        tTableItem *buffer = tableSearch(table, table->root, &var);
                        generateInstruction(I_LAB, (void *) buffer, NULL, NULL);
                        
                    }
                    listNext(list);
                    tmp = listGetData(list);
                }
                
                else {
                    if (tmp->addr1 == NULL) generateInstruction(tmp->instType, NULL, NULL, NULL);
                    else if (tmp->addr2 == NULL) generateInstruction(tmp->instType, ((tTableItem*)tmp->addr1), NULL, NULL);
                    else if (tmp->addr3 == NULL) generateInstruction(tmp->instType, ((tTableItem*)tmp->addr1), ((tTableItem*)tmp->addr2), NULL);
                    else generateInstruction(tmp->instType, ((tTableItem*)tmp->addr1), ((tTableItem*)tmp->addr2), ((tTableItem*)tmp->addr3));
                    listNext(list);
                    tmp = listGetData(list);
                }
                
            }
            
            endJump2->addr1 = ((tTableItem*)addrofEnd2->addr1);
            whileJump2->addr1 = ((tTableItem*)addrofLab2->addr1);
            endJump->addr1 = ((tTableItem*)addrofEnd2->addr1);
            
            return true;
            break;
        
        case IF:
        // pravidlo <stat> -> "IF" <expr> "THEN" \n <statList> "ELSE" \n <statList> "END"
            
            generateVariable2(&var);
            tableInsert(table, table->root, &var);
            toAssign = tableSearch(table, table->root, &var);
            generateInstruction(I_DEFVAR, (void *) toAssign, NULL, NULL);
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            
            result = expr(NULL, table, toAssign, COND);
            if (result != SYNTAX_OK) return false;
            if (token != THEN){errorVar = SYNTAX_ERROR; return false;}
            
            tInstr *ifJump = listGetPointerLast(list);
            listGoto(list, ifJump);
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            if (token != NEW_LINE) {errorVar = SYNTAX_ERROR; return false;}

            result = statList();

            if (!result) return false;
            if (token != ELSE){errorVar = SYNTAX_ERROR; return false;}
            generateEndLabel(&var);
            tableInsert(table, table->root, &var);
            tTableItem *endLabel = tableSearch(table, table->root, &var);
            generateInstruction(I_JMP, (void *) endLabel, NULL, NULL);
            
            generateElseLabel(&var);
            tableInsert(table, table->root, &var);
            variableInfo = tableSearch(table, table->root, &var);
            generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
            tInstr *elseLab = listGetPointerLast(list);
            ifJump->addr1 = (void *) variableInfo;
            
            if ((token = getNextToken(&attr)) == LEX_ERROR) return false;
            if (token != NEW_LINE) {errorVar = SYNTAX_ERROR; return false;}
            
            listGoto(list, ifJump);
            tmp = listGetData(list);
            while (tmp != NULL) {
                if (tmp->instType == I_DEFVAR) {
                    generateInstruction(I_DEFVAR, ((tTableItem*)tmp->addr1), NULL, NULL);
                    listNext(list);
                    tmp = listGetData(list);
                    
                    if (tmp->instType == I_MOVE && ((tTableItem*)tmp->addr2) == NULL){
                        generateInstruction(I_MOVE, ((tTableItem*)tmp->addr1), NULL, NULL);
                        
                        listNext(list);
                        tmp = listGetData(list);
                    }
                }
                else if (tmp == elseLab) {
                    break;
                }
                else {
                    listNext(list);
                    tmp = listGetData(list);
                }
                
            }
            
            result = statList();
            
            if (!result) return false;
            if (token != END){errorVar = SYNTAX_ERROR; return false;}
            generateInstruction(I_LAB, (void *) endLabel, NULL, NULL);
            
            if ((token = getNextToken(&attr)) == LEX_ERROR) return false;
            if (token != NEW_LINE) {errorVar = SYNTAX_ERROR; return false;}
            
            return true;
            break;
            
        case DEF:
        //pravidlo "DEF" "ID" <params> \n <statlist> "END"
            
            ;
            int lastFrame = currentFrame;
            currentFrame = LF;
            int paramsNo = 0;
            bool brackets = false;
            tTableItem *tmpEndLabel = NULL;
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            if (token != ID) {errorVar = SYNTAX_ERROR; return false;}
            found = tableInsert(table, table->root, &attr);
            variableInfo = tableSearch(table, table->root, &attr);
            
            if (found != 0)
            {
                if (variableInfo->declared) {errorVar = SEM_ERROR_3; return false;}
            }
           
            else if (found == 0) {
                variableInfo->data.varValue.fn_value.numberOfParams = -1;
               
                generateEndLabel(&var);
                tableInsert(table, table->root, &var);
                tmpEndLabel = tableSearch(table, table->root, &var);
                generateInstruction(I_JMP, (void *) tmpEndLabel, NULL, NULL);
            }
            
            variableInfo->function = true;
            variableInfo->declared = true;
            tableTableInsert(TL, &attr);
            tSymbolTable *funcTable = tableReturn(TL, &attr);
            tableInit(funcTable);
            tSymbolTable *lastTable = table;
            table = funcTable;
            
            generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
            generateInstruction(I_PUSHF, NULL, NULL, NULL);
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            if (token == L_BRACKET)
            {
                brackets = true;
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            }
            if (token != ID) { errorVar = SYNTAX_ERROR; return false;}
            
            result = params(funcTable, &paramsNo);
            if (!result) { errorVar = SYNTAX_ERROR; return false;}
            
            if(variableInfo->data.varValue.fn_value.numberOfParams != -1 && variableInfo->data.varValue.fn_value.numberOfParams != paramsNo)
            {
                errorVar = SEM_ERROR_5;
                return false;
            }
            variableInfo->data.varValue.fn_value.numberOfParams = paramsNo;
            
            if (brackets)
            {
                if (token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            }
            
            result = statList();
            if (!result) return false;
            
            if (token != END) {errorVar = SYNTAX_ERROR; return false;}
            
            tInstr *I = listGetData(list);
            strClear(&var);
            
            strAddChar(&var, '%');
            strAddChar(&var, 'r');
            strAddChar(&var, 'e');
            strAddChar(&var, 't');
            strAddChar(&var, 'v');
            strAddChar(&var, 'a');
            strAddChar(&var, 'l');
            tableInsert(table, table->root, &var);
            variableInfo = tableSearch(table, table->root, &var);
            
            generateInstruction(I_MOVE, (void *) variableInfo,(void*) ((tTableItem*)I->addr1), NULL);
            variableInfo->data.varType = ((tTableItem*)I->addr1)->data.varType;
            generateInstruction(I_POPF, NULL, NULL, NULL);
            generateInstruction(I_RET, NULL, NULL, NULL);
            
            if (found == 0) {
                
                generateInstruction(I_LAB, (void *) tmpEndLabel, NULL, NULL);
                
            }
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            
            currentFrame = lastFrame;
            lastFrame = LF;
            
            table = lastTable;
            
            return true;
            break;
            
        case INPUTI: // pravidlo <input> -> INPUTI
        case INPUTS: // pravidlo <input> -> INPUTS
        case INPUTF: // pravidlo <input> -> INPUTF
            
            fn = token;
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            
            generateEmpty();
            I = listGetData(list);
            
            if (token == L_BRACKET) //pravidlo INPUTI -> INPUTI ()
            {
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if (token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                
                
                switch (fn)
                {
                    case INPUTI:
                        generateInstruction(I_READ_I,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                        ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
                        break;
                        
                    case INPUTS:
                        generateInstruction(I_READ_S,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                        ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                        break;
                        
                    case INPUTF:
                        generateInstruction(I_READ_F,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                        ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                        break;
                        
                    default:
                        break;
                }
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                return statement();
            }
            
            else
            {
                tInstr *I;
                I = listGetData(list);
                switch (fn) {
                    case INPUTI:
                        generateInstruction(I_READ_I,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                        ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
                        break;
                        
                    case INPUTS:
                        generateInstruction(I_READ_S,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                        ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                        break;
                        
                    case INPUTF:
                        ((tTableItem*)I->addr1)->data.varType = TYPE_FLOAT;
                        generateInstruction(I_READ_F,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                        break;
                        
                    default:
                        break;
                }
                return statement();
            }
            
            break;
            
        case STRI2INT:  //ORD
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            
            generateEmpty();
            I = listGetData(list);
            
            generateInstruction(I_STRI2INT,(void *) ((tTableItem*)I->addr1), NULL, NULL);
            ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
            
            currentInstruction = listGetPointerLast(list);
            listGoto(list, currentInstruction);
            
            if(token == L_BRACKET) // pravidlo <idExpr> -> = '(' <term> ',' <num> ')'
            {
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if((result = term()) == false) return false;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if(token != COMMA) {errorVar = SYNTAX_ERROR; return false;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if((result = num()) == false) return false;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if(token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                return statement();
                
            }
            
            else // pravidlo <idExpr> -> = <term> ',' <num>
            {
                if((result = term()) == false) return false;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if(token != COMMA) {errorVar = SYNTAX_ERROR; return false;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if((result = num()) == false) return false;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                return statement();
            }
            
            break;
            
        case INT2CHAR:  //CHR
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            
            generateEmpty();
            I = listGetData(list);
            
            generateInstruction(I_INT2CHAR,(void *) ((tTableItem*)I->addr1), NULL, NULL);
            ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
            
            currentInstruction = listGetPointerLast(list);
            listGoto(list, currentInstruction);
            
            if(token == L_BRACKET) // pravidlo <idExpr> -> = '(' <num> ')'
            {
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if((result = num()) == false) return false;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if(token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                return statement();
                
            }
            
            else // pravidlo <idExpr> -> = <num>
                
            {
                if((result = num()) == false) return false;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                return statement();
            }
            
            break;
            
        case STRLEN: //LENGTH
            
            if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
            
            generateEmpty();
            I = listGetData(list);
            
            generateInstruction(I_STRLEN,(void *) ((tTableItem*)I->addr1), NULL, NULL);
            ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
            
            currentInstruction = listGetPointerLast(list);
            listGoto(list, currentInstruction);
            
            if(token == L_BRACKET) //pravidlo <idExpr> -> = LENGTH ( <term> )
            {
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if((result = term()) == false) return false;
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if(token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                return statement();
            }
            
            else //pravidlo <idExpr> -> = LENGTH
            {
                if((result = term()) == false) return false;
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                return statement();
            }
            
            break;
            
        default:
            errorVar = SYNTAX_ERROR;
            return true;
            break;
            
            
    }
    
}

bool term()
{
    tInstr *data = listGetData(list);
    tTableItem *variableInfo;
    if (token == ID) // <term> -> ID
    {
        if ((variableInfo = tableSearch(table,table->root, &attr)) == NULL) {errorVar = SYNTAX_ERROR; return false;}
        switch (data->instType)
        {
            case I_WRITE:
                data->addr1 = (void*) variableInfo;
                break;
                
            case I_STRI2INT:
                data->addr2 = (void*) variableInfo;
                break;
                
            case I_STRLEN:
                data->addr2 = (void*) variableInfo;
                break;
            case I_SUBSTR:
                data->addr1 = (void *) variableInfo;
                break;
                
            default:
                break;
                
        }
        return true;
    }
    
    else if (token == STRING_LIT) // <term> -> STRING_LIT
    {
        strClear(&var);
        generateVariable(&var);
        tableInsert(table, table->root, &var);
        variableInfo = tableSearch(table, table->root, &var);
        variableInfo->data.varType = TYPE_STRING;
        strInit(&variableInfo->data.varValue.s_value);
        strCopyString(&variableInfo->data.varValue.s_value, &attr);
        
        switch (data->instType)
        {
            case I_WRITE:
                data->addr1 = (void*) variableInfo;
                break;
                
            case I_STRI2INT:
                data->addr2 = (void*) variableInfo;
                break;
                
            case I_STRLEN:
                data->addr2 = (void*) variableInfo;
                break;
            
            case I_SUBSTR:
                data->addr1 = (void *) variableInfo;
                break;
                
            default:
                break;
        }
        return true;
    }
    
    else {errorVar = SYNTAX_ERROR; return false;}
    
}

bool otherTerms()
{
    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
    
    if (token == R_BRACKET || token == NEW_LINE)
    {
        return true;
    }
    
    if (token == COMMA) // pravidlo <otherTerms> -> ',' <term> <otherTerms>
    {
        if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
        generateInstruction(I_WRITE, NULL, NULL, NULL);
        void *currentInstruction;
        currentInstruction = listGetPointerLast(list);
        listGoto(list, currentInstruction);
        return (term() && otherTerms());
    }
    
    else {errorVar = SYNTAX_ERROR; return false;}
    
}

bool idExpr()
{
    bool result;
    int found;
    
    if(token == ASSIGN)
    {
        if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
        int fn = token;
        tInstr *I;
        I = listGetData(list);
        strClear(&var);
        void *currentInstruction;
        
        switch(token)
        {
                // pravidlo <idExpr> -> <input>
            case INPUTI: // pravidlo <input> -> INPUTI
            case INPUTS: // pravidlo <input> -> INPUTS
            case INPUTF: // pravidlo <input> -> INPUTF
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                if (token == L_BRACKET) //pravidlo INPUTI -> INPUTI ()
                {
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if (token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                    
                    
                    switch (fn)
                    {
                        case INPUTI:
                            generateInstruction(I_READ_I,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                            ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
                            break;
                            
                        case INPUTS:
                            generateInstruction(I_READ_S,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                            ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                            break;
                            
                        case INPUTF:
                            generateInstruction(I_READ_F,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                            ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                            break;
                            
                        default:
                            break;
                    }
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    return true;
                }
                
                else
                {
                    tInstr *I;
                    I = listGetData(list);
                    switch (fn) {
                        case INPUTI:
                            generateInstruction(I_READ_I,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                            ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
                            break;
                            
                        case INPUTS:
                            generateInstruction(I_READ_S,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                            ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                            break;
                            
                        case INPUTF:
                            ((tTableItem*)I->addr1)->data.varType = TYPE_FLOAT;
                            generateInstruction(I_READ_F,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                            break;
                            
                        default:
                            break;
                    }
                    return true;
                }
                
                break;
                
            case STRI2INT:  //ORD
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                generateInstruction(I_STRI2INT,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
                
                currentInstruction = listGetPointerLast(list);
                listGoto(list, currentInstruction);
                
                if(token == L_BRACKET) // pravidlo <idExpr> -> = '(' <term> ',' <num> ')'
                {
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if((result = term()) == false) return false;
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if(token != COMMA) {errorVar = SYNTAX_ERROR; return false;}
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if((result = num()) == false) return false;
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if(token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    
                    return true;
                    
                }
                
                else // pravidlo <idExpr> -> = <term> ',' <num>
                {
                    if((result = term()) == false) return false;
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if(token != COMMA) {errorVar = SYNTAX_ERROR; return false;}
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if((result = num()) == false) return false;
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    
                    return true;
                }
                
                break;
                
            case INT2CHAR:  //CHR
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                generateInstruction(I_INT2CHAR,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                
                currentInstruction = listGetPointerLast(list);
                listGoto(list, currentInstruction);
                
                if(token == L_BRACKET) // pravidlo <idExpr> -> = '(' <num> ')'
                {
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if((result = num()) == false) return false;
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if(token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    
                    return true;
                    
                }
                
                else // pravidlo <idExpr> -> = <num>
                    
                {
                    if((result = num()) == false) return false;
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    
                    return true;
                }
                
                break;
                
            case STRLEN: //LENGTH
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                generateInstruction(I_STRLEN,(void *) ((tTableItem*)I->addr1), NULL, NULL);
                ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
                
                currentInstruction = listGetPointerLast(list);
                listGoto(list, currentInstruction);
                
                if(token == L_BRACKET) //pravidlo <idExpr> -> = LENGTH ( <term> )
                {
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if((result = term()) == false) return false;
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if(token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    return true;
                }
                
                else //pravidlo <idExpr> -> = LENGTH
                {
                    if((result = term()) == false) return false;
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    return true;
                }
                
                break;
                
            case SUBSTR:
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                bool brackets = false;
                
                tInstr *lastInstruction = listGetPointerLast(list);
                
                generateInstruction(I_SUBSTR, NULL, NULL, NULL);
                void *tmpInst = listGetPointerLast(list);
                listGoto(list, tmpInst);
                tInstr *currentInstruction = listGetData(list);
                
                if (token == L_BRACKET) {
                    brackets = true;
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                }
                
                if((result = term()) == false) return false;

                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if(token != COMMA) {errorVar = SYNTAX_ERROR; return false;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if((result = num()) == false) return false;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if(token != COMMA) {errorVar = SYNTAX_ERROR; return false;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if((result = num()) == false) return false;
                
                if (brackets) {
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if (token != R_BRACKET) {errorVar = SYNTAX_ERROR; return false;}
                }
            
                
                generateVariable2(&var);
                tableInsert(table, table->root, &var);
                tTableItem *lenVar = tableSearch(table, table->root, &var);
                generateInstruction(I_DEFVAR, (void *) lenVar, NULL, NULL);
                
                generateInstruction(I_STRLEN, (void *) lenVar, ((tTableItem*)currentInstruction->addr1), NULL);
                generateVariable2(&var);
                tableInsert(table, table->root, &var);
                tTableItem *notVar = tableSearch(table, table->root, &var);
                generateInstruction(I_DEFVAR, (void *) notVar, NULL, NULL);
                
                generateInstruction(I_LT, (void *) notVar, ((tTableItem*)currentInstruction->addr2), (void *) lenVar);
                generateInstruction(I_NOT, (void *) notVar, (void *) notVar, NULL);
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                tTableItem *boolTrue = tableSearch(table, table->root, &var);
                boolTrue->data.varType = TYPE_BOOL;
                boolTrue->data.varValue.b_value = true;
                
                generateInstruction(I_MOVE, ((tTableItem*)lastInstruction->addr1), NULL, NULL);
                generateInstruction(I_JMPEQ, NULL, (void *) boolTrue , (void *) notVar);
                tInstr *addrOf1stJump = listGetPointerLast(list);
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                tTableItem *intOne = tableSearch(table, table->root, &var);
                intOne->data.varType = TYPE_INT;
                intOne->data.varValue.i_value = 1;
                
                generateVariable2(&var);
                tableInsert(table, table->root, &var);
                tTableItem *concatRes = tableSearch(table, table->root, &var);
                generateInstruction(I_DEFVAR, (void *) concatRes, NULL, NULL);
                
                generateVariable2(&var);
                tableInsert(table, table->root, &var);
                tTableItem *loopCounter = tableSearch(table, table->root, &var);
                generateInstruction(I_DEFVAR, (void *) loopCounter, NULL, NULL);
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                tTableItem *intZero = tableSearch(table, table->root, &var);
                intZero->data.varType = TYPE_INT;
                intZero->data.varValue.i_value = 0;
                
                generateInstruction(I_MOVE, (void *) loopCounter, (void *) intZero, NULL);
                
                generateVariable2(&var);
                tableInsert(table, table->root, &var);
                tTableItem *secondOpCheck = tableSearch(table, table->root, &var);
                generateInstruction(I_DEFVAR, (void *) secondOpCheck, NULL, NULL);
                generateInstruction(I_MOVE, (void *) secondOpCheck, (void *) ((tTableItem*)currentInstruction->addr2), NULL);
                
                generateVariable2(&var);
                tableInsert(table, table->root, &var);
                tTableItem *thirdOpCheck = tableSearch(table, table->root, &var);
                generateInstruction(I_DEFVAR, (void *) thirdOpCheck, NULL, NULL);
                generateInstruction(I_MOVE, (void *) thirdOpCheck, (void *) ((tTableItem*)currentInstruction->addr3), NULL);
                
                generateInstruction(I_GETCHAR, (void *) concatRes, ((tTableItem*)currentInstruction->addr1), secondOpCheck);
                generateInstruction(I_ADD, (void *) secondOpCheck, (void *) secondOpCheck, (void *) intOne);
                generateInstruction(I_SUB, (void *) thirdOpCheck, (void *) thirdOpCheck, (void *) intOne);

                generateVariable2(&var);
                tableInsert(table, table->root, &var);
                tTableItem *res = tableSearch(table, table->root, &var);
                generateInstruction(I_DEFVAR, (void *) res, NULL, NULL);
                
                generateWhileLabel(&var);
                tableInsert(table, table->root, &var);
                tTableItem *variableInfo = tableSearch(table, table->root, &var);
                generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
//                tInstr *addrOfWhile;
//                addrOfWhile = listGetPointerLast(list);
                
              
                
                generateInstruction(I_GETCHAR, (void *) res, ((tTableItem*)currentInstruction->addr1), secondOpCheck);
                generateInstruction(I_ADD, (void *) secondOpCheck, (void *) secondOpCheck, (void *) intOne);
                generateInstruction(I_CONCAT, (void *) concatRes, (void *) concatRes,  (void *) res);
                generateInstruction(I_SUB, (void *) thirdOpCheck, (void *) thirdOpCheck, (void *) intOne);
                
                generateInstruction(I_JMPEQ, NULL, thirdOpCheck, loopCounter);
                tInstr *jump2 = listGetPointerLast(list);
                generateInstruction(I_JMPEQ, NULL, secondOpCheck, lenVar);
                tInstr *jump3 = listGetPointerLast(list);
                generateInstruction(I_JMP, (void *) variableInfo, NULL, NULL);
                
                generateEndLabel(&var);
                tableInsert(table, table->root, &var);
                variableInfo = tableSearch(table, table->root, &var);
                generateInstruction(I_LAB, (void *) variableInfo, NULL, NULL);
                addrOf1stJump->addr1 = variableInfo;
                jump2->addr1 = variableInfo;
                jump3->addr1 = variableInfo;
                
                generateInstruction(I_MOVE, ((tTableItem*)lastInstruction->addr1), (void *) concatRes, NULL);
                ((tTableItem*)lastInstruction->addr1)->data.varType = TYPE_INT;
               
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                return true;
                break;
                
            case INT_LIT:
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                tTableItem *buffer;
                tInstr *I = listGetData(list);
                buffer = tableSearch(table, table->root, &var);
                buffer->data.varType = TYPE_INT;
                buffer->data.varValue.i_value = atoi(attr.str);
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                if (token != PLUS && token != MINUS && token != DIV && token != MULT && token != EQUAL && token != N_EQUAL && token != L_EQ && token != G_EQ && token != LOWER && token != GREATER) {
                    generateInstruction(I_MOVE, (void*) ((tTableItem*)I->addr1) , (void *) buffer, NULL);
                    ((tTableItem*)I->addr1)->data.varType = TYPE_INT;
                    return true;
                }
                
                else {
                    tTableItem *helper = ((tTableItem*)I->addr1);
                    result = expr(buffer, table, helper, EXPR);
                    
                }
                
                return true;
                break;
                
            case FLOAT_LIT:
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                I = listGetData(list);
                buffer = tableSearch(table, table->root, &var);
                buffer->data.varType = TYPE_FLOAT;
                buffer->data.varValue.f_value = atof(attr.str);
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if (token != PLUS && token != MINUS && token != DIV && token != MULT && token != EQUAL && token != N_EQUAL && token != L_EQ && token != G_EQ && token != LOWER && token != GREATER) {
                    generateInstruction(I_MOVE, (void*) ((tTableItem*)I->addr1) , (void *) buffer, NULL);
                    ((tTableItem*)I->addr1)->data.varType = TYPE_FLOAT;
                    return true;
                }
                
                else {
                    tTableItem *helper = ((tTableItem*)I->addr1);
                    result = expr(buffer, table, helper, EXPR);
                   
                }
                
                return true;
                break;
                
            case STRING_LIT:
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                I = listGetData(list);
                buffer = tableSearch(table, table->root, &var);
                buffer->data.varType = TYPE_STRING;
                strInit(&buffer->data.varValue.s_value);
                strCopyString(&buffer->data.varValue.s_value, &attr);
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                if (token != PLUS && token != MINUS && token != DIV && token != MULT && token != EQUAL && token != N_EQUAL && token != L_EQ && token != G_EQ && token != LOWER && token != GREATER) {
                    generateInstruction(I_MOVE, (void*) ((tTableItem*)I->addr1) , (void *) buffer, NULL);
                    ((tTableItem*)I->addr1)->data.varType = TYPE_STRING;
                    return true;
                }
                
                else {
                    tTableItem *helper = ((tTableItem*)I->addr1);
                    result = expr(buffer, table, helper, EXPR);
                   
                }
                
                return true;
                break;
                
            case ID:
                
                found = tableInsert(table, table->root, &attr);
                variableInfo = tableSearch(table, table->root, &attr);
                
                if(found == 0)
                {
                    void *lastinstruction;
                    lastinstruction = listGetPointerLast(list);
                    listGoto(list, lastinstruction);
                    I = listGetData(list);
                    
                    generateInstruction(I_VAL, (void *) variableInfo, NULL, NULL);
                    currentInstruction = listGetPointerLast(list);
                    listGoto(list, currentInstruction);
                    
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    if (token != PLUS && token != MINUS && token != DIV && token != MULT && token != EQUAL && token != N_EQUAL && token != L_EQ && token != G_EQ && token != LOWER && token != GREATER) {
                        
                        result = callFunction();
                        if (!result) return false;
                        buffer = (void*) ((tTableItem*)I->addr1);
                        
                        strClear(&var);
                        strAddChar(&var, '%');
                        strAddChar(&var, 'r');
                        strAddChar(&var, 'e');
                        strAddChar(&var, 't');
                        strAddChar(&var, 'v');
                        strAddChar(&var, 'a');
                        strAddChar(&var, 'l');
                        tableInsert(table, table->root, &var);
                        variableInfo = tableSearch(table, table->root, &var);
                        
                        
                        generateInstruction(I_MOVE, (void*) ((tTableItem*)I->addr1), (void *) variableInfo, NULL);
                        ((tTableItem*)I->addr1)->data.varType = variableInfo->data.varType;
                        currentInstruction = listGetPointerLast(list);
                        listGoto(list, currentInstruction);
                        I = listGetData(list);
                        I->frame2 = TF;
                        
                        
                    }
                    else {
                        errorVar = SYNTAX_ERROR;
                        return false;
                    }
                }
                
                else {
                    void *lastinstruction;
                    lastinstruction = listGetPointerLast(list);
                    listGoto(list, lastinstruction);
                    tInstr *I;
                    I = listGetData(list);
                    generateInstruction(I_VAL, (void *) variableInfo, NULL, NULL);
                    void *currentInstruction;
                    currentInstruction = listGetPointerLast(list);
                    listGoto(list, currentInstruction);
                    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                    
                    if (token != PLUS && token != MINUS && token != DIV && token != MULT && token != EQUAL && token != N_EQUAL && token != L_EQ && token != G_EQ && token != LOWER && token != GREATER)  {
                        if (variableInfo->function) {
                            result = callFunction();
                            if (!result) return false;
                            buffer = (void*) ((tTableItem*)I->addr1);
                            
                            strClear(&var);
                            strAddChar(&var, '%');
                            strAddChar(&var, 'r');
                            strAddChar(&var, 'e');
                            strAddChar(&var, 't');
                            strAddChar(&var, 'v');
                            strAddChar(&var, 'a');
                            strAddChar(&var, 'l');
                            tableInsert(table, table->root, &var);
                            variableInfo = tableSearch(table, table->root, &var);
                            
                            
                            generateInstruction(I_MOVE, (void*) ((tTableItem*)I->addr1), (void *) variableInfo, NULL);
                            ((tTableItem*)I->addr1)->data.varType = variableInfo->data.varType;
                            currentInstruction = listGetPointerLast(list);
                            listGoto(list, currentInstruction);
                            I = listGetData(list);
                            I->frame2 = TF;
                        }
                        else {
                            generateInstruction(I_MOVE, (void*) ((tTableItem*)I->addr1), (void*) variableInfo, NULL);
                            ((tTableItem*)I->addr1)->data.varType = variableInfo->data.varType;
                        }
                        
                    }
                    else {
                        tTableItem *helper = ((tTableItem*)I->addr1);
                        result = expr(variableInfo, table, helper, EXPR);
                    
                    }
                    
                }
                
                return true;
                break;
                
            case L_BRACKET:
                
                I = listGetData(list);
                tTableItem bracket;
                bracket.data.varType = L_BRACKET;
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                
                if (token == PLUS || token == MINUS || token == DIV || token == MULT || token == EQUAL || token == N_EQUAL || token == L_EQ || token == G_EQ || token == LOWER || token == GREATER) {
                    errorVar = SYNTAX_ERROR;
                    return false;
                }
                
                else {
                    tTableItem *helper = ((tTableItem*)I->addr1);
                    result = expr(&bracket, table, helper, EXPR);
                    
                }
                
                return true;
                break;
            
            case NIL:
                I = listGetData(list);
                generateInstruction(I_MOVE, ((tTableItem*)I->addr1), NULL, NULL);
                if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
                ((tTableItem*)I->addr1)->data.varType = TYPE_NIL;
                return true;
                break;
                
                
            default:
                errorVar = SYNTAX_ERROR;
                return false;
                break;
        }
        
        
    }
    
    else if (token == NEW_LINE || token == END_OF_FILE) return true;
    
    else {
        
        errorVar = SYNTAX_ERROR;
        return false;
    }
    
}

bool num()
{
    tTableItem *variableInfo;
    tInstr *data = listGetData(list);
    
    if (token == ID) {
        if ((variableInfo = tableSearch(table,table->root, &attr)) == NULL) {errorVar = INTERNAL_ERROR; return false;}
        
        switch (data->instType) {
                
            case I_STRI2INT:
                data->addr3 = (void*) variableInfo;
                break;
                
            case I_INT2CHAR:
                data->addr2 = (void*) variableInfo;
                break;
            case I_SUBSTR:
                if (data->addr2 == NULL) {
                    data->addr2 = (void*) variableInfo;
                }
                else {
                    data->addr3 = (void*) variableInfo;
                }
                break;
                
            default:
                break;
        }
        
        return true;
    }
    
    else if (token == INT_LIT) {
        strClear(&var);
        
        generateVariable(&var);
        tableInsert(table, table->root, &var);
        variableInfo = tableSearch(table, table->root, &var);
        variableInfo->data.varType = TYPE_INT;
        variableInfo->data.varValue.i_value = atoi(attr.str);
        
        switch (data->instType) {
                
            case I_STRI2INT:
                data->addr3 = (void*) variableInfo;
                break;
                
            case I_INT2CHAR:
                data->addr2 = (void*) variableInfo;
                
            case I_SUBSTR:
                if (data->addr2 == NULL) {
                    data->addr2 = (void*) variableInfo;
                }
                else {
                    data->addr3 = (void*) variableInfo;
                }
                break;
                
            default:
                break;
        }
        
        return true;
    }
    
    else {errorVar = SYNTAX_ERROR; return false;}
}

bool params(tSymbolTable *fnTable, int *paramsNo)
{
    int param = 0;
    strClear(&var);
    
    strAddChar(&var, '%');
    strAddChar(&var, 'r');
    strAddChar(&var, 'e');
    strAddChar(&var, 't');
    strAddChar(&var, 'v');
    strAddChar(&var, 'a');
    strAddChar(&var, 'l');
    
    tableInsert(fnTable, fnTable->root, &var);
    tTableItem *variableInfo = tableSearch(fnTable, fnTable->root, &var);
    tTableItem *currentParam;
    generateInstruction(I_DEFVAR, (void *) variableInfo, NULL, NULL);
    generateInstruction(I_MOVE, (void *) variableInfo, NULL, NULL);
    
    while (token == ID) {
        
        (*paramsNo)++;
        param++;
        tableInsert(fnTable, fnTable->root, &attr);
        currentParam = tableSearch(fnTable, fnTable->root, &attr);
        generateInstruction(I_DEFVAR, (void *) currentParam, NULL, NULL);
        
        strClear(&var);
        strAddChar(&var, '%');
        
        while (param != 0)
        {
            strAddChar(&var, (char)(param % 10 + '0'));
            param = param / 10;
        }
        
        tableInsert(fnTable, fnTable->root, &var);
        variableInfo = tableSearch(fnTable, fnTable->root, &var);
        generateInstruction(I_MOVE, (void *) currentParam, (void *) variableInfo, NULL);
        currentParam->data.varType = variableInfo->data.varType;
        
        if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
        if (token != COMMA) break;
        if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
    }
    return true;
}

bool program()
{
    bool result;
    //prvni pravidlo <prog> -> <statList> <EOF>
    result = statList();
    if (!result) return false;
    if (token != END_OF_FILE) return false;
    return true;
}

bool callFunction()
{
    tInstr *I = listGetData(list);
    tTableItem *variableInfo = (void *) ((tTableItem*)I->addr1);
    tTableItem *buffer = NULL;
    tTableItem *currentParam;
    strClear(&var);
    
    int param = 0;
    int params = 0;
    int lastFrame = currentFrame;
    currentFrame = TF;
    generateInstruction(I_CREATEF,NULL, NULL, NULL);
    variableInfo->function = true;
    
    while (token == ID || token == INT_LIT || token == STRING_LIT || token == FLOAT_LIT) {
        
        param++;
        params++;
        strClear(&var);
        strAddChar(&var, '%');
        
        while (param != 0)
        {
            strAddChar(&var, (char)(param % 10 + '0'));
            param = param / 10;
        }
        tableInsert(table, table->root, &var);
        currentParam = tableSearch(table, table->root, &var);
        
        switch (token) {
            case ID:
                if ((buffer = tableSearch(table,table->root, &attr)) == NULL) {errorVar = INTERNAL_ERROR; return false;}
                
                break;
            case INT_LIT:
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                buffer = tableSearch(table, table->root, &var);
                buffer->data.varType = TYPE_INT;
                buffer->data.varValue.i_value = atoi(attr.str);
                
                break;
            case STRING_LIT:
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                buffer = tableSearch(table, table->root, &var);
                buffer->data.varType = TYPE_STRING;
                strInit(&buffer->data.varValue.s_value);
                strCopyString(&buffer->data.varValue.s_value, &attr);
                
                break;
            case FLOAT_LIT:
                
                generateVariable(&var);
                tableInsert(table, table->root, &var);
                buffer = tableSearch(table, table->root, &var);
                buffer->data.varType = TYPE_FLOAT;
                buffer->data.varValue.f_value = atof(attr.str);
                
                break;
                
            default:
                break;
        }
        generateInstruction(I_DEFVAR, (void *) currentParam, NULL, NULL);
        generateInstruction(I_MOVE, (void *) currentParam, (void *) buffer, NULL);
        currentParam->data.varType = buffer->data.varType;
        
        tInstr *I = listGetPointerLast(list);
        I->frame2 = lastFrame;
        if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
        if (token != COMMA) break;
        if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
    }
    if (!variableInfo->declared) {
        variableInfo->data.varValue.fn_value.numberOfParams = params;
    }
    else if (variableInfo->data.varValue.fn_value.numberOfParams != params) {errorVar = SEM_ERROR_5; return false;}
    generateInstruction(I_CALL, (void *) variableInfo, NULL, NULL);
    
    if((token = getNextToken(&attr)) == LEX_ERROR) {errorVar = LEX_ERROR; return false;}
    currentFrame = lastFrame;
    lastFrame = TF;
    return true;
    
}

int parse(tTableList *tableList, tListOfInstr *instrList)
{
    int result;
    TL = tableList;
    table = tableList->first;
    list = instrList;
    strInit(&attr);
    strInit(&var);
    strClear(&var);
    strAddChar(&var, '*');
    strAddChar(&var, '0');
    tableInsert(table, table->root, &var);
    varZero = tableSearch(table, table->root, &var);
    generateInstruction(I_DEFVAR, (void *) varZero, NULL, NULL);
    strClear(&var);
    
    result = program();
    if (!result) return errorVar;
    result = functionsDeclared(table, table->root);
    if (!result) errorVar = SEM_ERROR_3;
    
    strFree(&attr);
    strFree(&var);
    return errorVar;
}

//---------------------------PSA----------------------------
// TODO
// - deleni s pretypovanim
// - operace s booly
//


int getIndex(item * item){
    switch(item->table_item->data.varType){
        case ID:
        case TYPE_INT:
        case TYPE_FLOAT:
        case TYPE_STRING:
        case TYPE_NIL:
        case INT_LIT:
        case FLOAT_LIT:
            return 2;
        case PLUS:
        case MINUS:
            return 0;
        case MULT:
        case DIV:
            return 1;
        case DOLLAR:
            return 6;
        case LOWER:
        case GREATER:
        case L_EQ:
        case G_EQ:
        case EQUAL:
        case N_EQUAL:
            return 5;
        case L_BRACKET:
            return 3;
        case R_BRACKET:
            return 4;
        default:
            return -1;
            break;
    }
}

int getIndexOfToken(int token){
    
    switch(token){
        case ID:
        case TYPE_INT:
        case TYPE_FLOAT:
        case TYPE_STRING:
        case TYPE_NIL:
        case NIL:
        case INT_LIT:
        case FLOAT_LIT:
        case STRING_LIT:
            return 2;
        case PLUS:
        case MINUS:
            return 0;
        case MULT:
        case DIV:
            return 1;
        case DOLLAR:
            return 6;
        case LOWER:
        case GREATER:
        case L_EQ:
        case G_EQ:
        case EQUAL:
        case N_EQUAL:
            return 5;
        case L_BRACKET:
            return 3;
        case R_BRACKET:
            return 4;
        default:
            return -1;
            break;
    }
    
}


item * create_item(tTableItem *table_item, int type, int i_value, float f_value, string *s_value){
    
    item * new_item;
    new_item = malloc(sizeof(item));
    new_item->next = NULL;
    new_item->constant = false;
    new_item->reduced = false;
    
    if(table_item != NULL){
        new_item->table_item = table_item;
        new_item->inTable = true;
        if ((strncmp(table_item->key.str, "$", 1)) == 0)
            new_item->constant = true;
        else
            new_item->constant = false;
    }
    else{
        new_item->inTable = false;
        new_item->table_item = malloc(sizeof(tTableItem));
        new_item->constant = true;
        new_item->table_item->data.varType = type;      //ulozeni typu hodnoty
        
        switch (type) {
            case TYPE_INT:
                new_item->table_item->data.varValue.i_value = i_value;
                break;
            case TYPE_FLOAT:
                new_item->table_item->data.varValue.f_value = f_value;
                break;
            case TYPE_STRING:
                strInit(&new_item->table_item->data.varValue.s_value);
                strCopyString(&(new_item->table_item->data.varValue.s_value), s_value);
                break;
            default:
                break;
        }
    }
    
    return new_item;
}

item * getFirstTerm(tStack * stack){
    item * itemPtr;
    itemPtr = sTop(stack);
    
    while(itemPtr->reduced == true){
        itemPtr = itemPtr->next;
    }
    
    return itemPtr;
}

int howManyTermsToPop(tStack * stack){
    
    item * tmp;
    tmp = sTop(stack);
    
    if(tmp->reduced) return 3;
    else return 1;
    
}

bool getNewVar(tStack * stack){
    item * tmp;
    tmp = sTop(stack);
    int counter = 1;
    
    while(tmp->next != NULL){
        tmp = tmp->next;
        counter++;
    }
    
    return counter > 4 ? true : false;
    
}



int expr(tTableItem * first_token, tSymbolTable * symTable, tTableItem * to_assign, int mod){
    
    switch(mod){
        case EXPR:
            
            return processExpr(first_token, symTable, to_assign);
            
        case COND:
            
            return processExpr(NULL, symTable, to_assign);
            
        default:
            errorVar = INTERNAL_ERROR;
            return INTERNAL_ERROR;
            break;
    }
}

int processExpr(tTableItem * first_item, tSymbolTable * symTable, tTableItem * to_assign){
    
    tTableItem * left = to_assign;   //pomocna promenna pro ukladani vysledku vyrazu
    tStack stack;
    sInit(&stack);
    // PUSH DOLLAR
    item * dollar;
    dollar = create_item(NULL, DOLLAR, 0, 0.0, NULL);
    sPush(&stack, dollar);
    
    
    if(first_item != NULL){
        item * first;
        first = create_item(first_item, 0, 0, 0.0, NULL);
        first->inTable = true;
        sPush(&stack, first);
    }
    
    while(true){
        
        int rule;
        if(token == NEW_LINE || token == DO || token == THEN){      //Pripad, kdy nam dojde konec vyrazu, musime ho cist jako DOLLAR
            rule = Precedence_table[getIndex(getFirstTerm(&stack))][getIndexOfToken(DOLLAR)];
        }
        else{
            rule = Precedence_table[getIndex(getFirstTerm(&stack))][getIndexOfToken(token)];
        }
        tTableItem * to_search;
        item * to_push;
        
        switch (rule){
            case SFT:   //SHIFT
                if(token == ID){
                    to_search = tableSearch(symTable, symTable->root, &attr);
                    if(to_search == NULL) {delete_stack(&stack); return SEM_ERROR_4;}
                    to_push = create_item(to_search, 0, 0, 0.0, NULL);
                    sPush(&stack, to_push);
                }
                else if(token == INT_LIT){
                    to_push = create_item(NULL, TYPE_INT, atoi(attr.str), 0.0, NULL);
                    sPush(&stack, to_push);
                }
                else if(token == FLOAT_LIT){
                    to_push = create_item(NULL, TYPE_FLOAT, 0, atof(attr.str), NULL);
                    sPush(&stack, to_push);
                }
                else if(token == NIL){
                    to_push = create_item(NULL, TYPE_NIL, 0, 0.0, NULL);
                    sPush(&stack, to_push);
                }
                else if(token == PLUS || token == MINUS || token == MULT || token == DIV || token == EQUAL || token == N_EQUAL || token == LOWER || token == GREATER || token == L_EQ || token == G_EQ || token == L_BRACKET || token == R_BRACKET){
                    to_push = create_item(NULL, token, 0, 0.0, NULL);
                    sPush(&stack, to_push);
                }
                else if(token == STRING_LIT){
                    to_push = create_item(NULL, TYPE_STRING, 0, 0.0, &attr);
                    sPush(&stack, to_push);
                }
                else {delete_stack(&stack); errorVar = SEM_ERROR_4; return SEM_ERROR_4;}
                
                if((token = getNextToken(&attr)) == LEX_ERROR) {delete_stack(&stack); errorVar = LEX_ERROR; return LEX_ERROR;}
                
                break;
                
                
            case RDC:   //REDUCE
                ;
                int index = howManyTermsToPop(&stack);
                switch(index){
                        
                    case 1:     // E -> var
                        (sTop(&stack))->reduced = true;
                        break;
                    case 3:     // E -> E op E
                        //Druhy operand
                        ;
                        item * operator;
                        operator = (sTop(&stack))->next;
                        item * second_operand;
                        second_operand = sTop(&stack);
                        item * first_operand;
                        first_operand = operator->next;
                        
                        tTableItem * left_operand = NULL;
                        tTableItem * right_operand = NULL;
                        left = to_assign;
                        
                        
                        
                        //Oba operandy jsou konstanta
                        if(first_operand->constant && second_operand->constant){
                            //Konverze konstant
                            if(first_operand->table_item->data.varType == TYPE_INT && second_operand->table_item->data.varType == TYPE_FLOAT){
                                //Preved 1. operand na FLOAT_LIT
                                first_operand->table_item->data.varType = TYPE_FLOAT;
                                first_operand->table_item->data.varValue.f_value = (int)first_operand->table_item->data.varValue.i_value;
                            }
                            else if(first_operand->table_item->data.varType == TYPE_FLOAT && second_operand->table_item->data.varType == TYPE_INT){
                                //Preved 2. operand na FLOAT_LIT
                                second_operand->table_item->data.varType = TYPE_FLOAT;
                                second_operand->table_item->data.varValue.f_value = (int)second_operand->table_item->data.varValue.i_value;
                            }
                        }
                        
                        
                        /*
                         *  KONVERZE typů a hodnot, generování pomocných proměnných, aktualizování hodnot na zásobníku
                         *
                         *
                         */
                        if(first_operand->table_item->data.varType == TYPE_INT){
                            // INT + FLOAT
                            if(first_operand->table_item->data.varType != second_operand->table_item->data.varType){
                                
                                left->data.varType = TYPE_FLOAT;
                                
                                if(first_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    left_operand = tableSearch(symTable, symTable->root, &var);
                                    left_operand->data.varType = TYPE_FLOAT;
                                    left_operand->data.varValue.f_value = first_operand->table_item->data.varValue.i_value;
                                }
                                // INTvar + FLOAT
                                if(first_operand->constant == false){
                                    if(second_operand->table_item->data.varType == TYPE_FLOAT){
                                        left_operand = varZero;
                                        //generateInstruction(I_DEFVAR, (void *)left_operand, NULL, NULL);
                                        generateInstruction(I_I2F, (void *)left_operand, (void *)first_operand->table_item, NULL);
                                    }
                                    else if(second_operand->table_item->data.varType == TYPE_NIL){
                                        left_operand = first_operand->table_item;
                                    }
                                }
                                if(second_operand->table_item->data.varType == TYPE_FLOAT){
                                    // INT(var) + FLOAT
                                    if(second_operand->constant == true){
                                        generateVariable(&var);
                                        if(tableInsert(symTable, symTable->root, &var) != 0){
                                            delete_stack(&stack);
                                            errorVar = INTERNAL_ERROR;
                                            return INTERNAL_ERROR;
                                        }
                                        right_operand = tableSearch(symTable, symTable->root, &var);
                                        right_operand->data.varType = TYPE_FLOAT;
                                        right_operand->data.varValue.f_value = second_operand->table_item->data.varValue.f_value;
                                    }
                                    // INT(var) + FLOATvar
                                    else{   //Constant == false
                                        right_operand = second_operand->table_item;
                                    }
                                }
                                else if(second_operand->table_item->data.varType == TYPE_NIL){
                                    if(second_operand->constant == true){
                                        generateVariable(&var);
                                        if(tableInsert(symTable, symTable->root, &var) != 0){
                                            delete_stack(&stack);
                                            errorVar = INTERNAL_ERROR;
                                            return INTERNAL_ERROR;
                                        }
                                        right_operand = tableSearch(symTable, symTable->root, &var);
                                        right_operand->data.varType = TYPE_NIL;
                                    }
                                    else{       //Constant == true
                                        right_operand = second_operand->table_item;
                                        right_operand->data.varType = left_operand->data.varType;
                                    }
                                }
                                else{       //Second operand je TYPE_STRING nebo TYPE_BOOL
                                    delete_stack(&stack);
                                    errorVar = SEM_ERROR_4;
                                    return SEM_ERROR_4;
                                }
                            }
                            else{   //Typy jsou stejne -> INT
                                
                                left->data.varType = TYPE_INT;
                                
                                if(first_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    left_operand = tableSearch(symTable, symTable->root, &var);
                                    left_operand->data.varType = TYPE_INT;
                                    left_operand->data.varValue.i_value = first_operand->table_item->data.varValue.i_value;
                                }
                                else{   //Constant == false
                                    left_operand = first_operand->table_item;
                                }
                                
                                if(second_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    right_operand = tableSearch(symTable, symTable->root, &var);
                                    right_operand->data.varType = TYPE_INT;
                                    right_operand->data.varValue.i_value = second_operand->table_item->data.varValue.i_value;
                                }
                                else{   //Constant == false
                                    right_operand = second_operand->table_item;
                                }
                            }
                        }
                        else if(first_operand->table_item->data.varType == TYPE_FLOAT){   //First operand = TYPE_FLOAT
                            
                            if(first_operand->table_item->data.varType != second_operand->table_item->data.varType){
                                
                                left->data.varType = TYPE_FLOAT;
                                
                                if(first_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    left_operand = tableSearch(symTable, symTable->root, &var);
                                    left_operand->data.varType = TYPE_FLOAT;
                                    left_operand->data.varValue.f_value = first_operand->table_item->data.varValue.f_value;
                                }
                                else{
                                    left_operand = first_operand->table_item;
                                }
                                if(second_operand->table_item->data.varType == TYPE_INT){
                                    if(second_operand->constant == true){
                                        generateVariable(&var);
                                        if(tableInsert(symTable, symTable->root, &var) != 0){
                                            delete_stack(&stack);
                                            errorVar = INTERNAL_ERROR;
                                            return INTERNAL_ERROR;
                                        }
                                        right_operand = tableSearch(symTable, symTable->root, &var);
                                        right_operand->data.varType = TYPE_FLOAT;
                                        right_operand->data.varValue.f_value = second_operand->table_item->data.varValue.i_value;
                                    }
                                    else{
                                        right_operand = varZero;
                                        generateInstruction(I_I2F, (void *)right_operand, (void *)second_operand->table_item, NULL);
                                    }
                                }
                                else{       //Second operand je TYPE_STRING nebo TYPE_BOOL
                                    delete_stack(&stack);
                                    errorVar = SEM_ERROR_4;
                                    return SEM_ERROR_4;
                                }
                            }
                            
                            else{       //Typy jsou stejne -> TYPE_FLOAT
                                
                                left->data.varType = TYPE_FLOAT;
                                
                                if(first_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    left_operand = tableSearch(symTable, symTable->root, &var);
                                    left_operand->data.varType = TYPE_FLOAT;
                                    left_operand->data.varValue.f_value = first_operand->table_item->data.varValue.f_value;
                                }
                                else{   //Constant == false
                                    left_operand = first_operand->table_item;
                                }
                                
                                if(second_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    right_operand = tableSearch(symTable, symTable->root, &var);
                                    right_operand->data.varType = TYPE_FLOAT;
                                    right_operand->data.varValue.f_value = second_operand->table_item->data.varValue.f_value;
                                }
                                else{   //Constant == false
                                    right_operand = second_operand->table_item;
                                }
                            }
                        }
                        else if(first_operand->table_item->data.varType == TYPE_STRING){
                            if(second_operand->table_item->data.varType == TYPE_STRING){
                                //Dva stringy
                                
                                //Prvni operand
                                if(first_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    left_operand = tableSearch(symTable, symTable->root, &var);
                                    left_operand->data.varType = TYPE_STRING;
                                    strInit(&left_operand->data.varValue.s_value);
                                    strCopyString(&left_operand->data.varValue.s_value, &first_operand->table_item->data.varValue.s_value);
                                }
                                else{
                                    left_operand = first_operand->table_item;
                                }
                                
                                //Druhy operand
                                if(second_operand->constant == true){
                                    generateVariable(&var);
                                    if(tableInsert(symTable, symTable->root, &var) != 0){
                                        delete_stack(&stack);
                                        errorVar = INTERNAL_ERROR;
                                        return INTERNAL_ERROR;
                                    }
                                    right_operand = tableSearch(symTable, symTable->root, &var);
                                    right_operand->data.varType = TYPE_STRING;
                                    strInit(&right_operand->data.varValue.s_value);
                                    strCopyString(&right_operand->data.varValue.s_value, &second_operand->table_item->data.varValue.s_value);
                                }
                                else{
                                    right_operand = second_operand->table_item;
                                }
                                
                                //Predejdeme chybnym operatorum u operace s textovymi retezci
                                //Se stringy muzeme pojit operatory "+", "==" a "!="
                                if(!(operator->table_item->data.varType == PLUS || operator->table_item->data.varType == EQUAL || operator->table_item->data.varType == N_EQUAL)){
                                    //Neprijde ani jeden z vyse zminenych -> SEM_ERROR_4
                                    delete_stack(&stack);
                                    errorVar = SEM_ERROR_4;
                                    return SEM_ERROR_4;
                                }
                                
                            }
                            else{
                                delete_stack(&stack);
                                errorVar = SEM_ERROR_4;
                                return SEM_ERROR_4;
                            }
                        }
                        else if(first_operand->table_item->data.varType == TYPE_BOOL){
                            delete_stack(&stack);
                            errorVar = SEM_ERROR_4;
                            return SEM_ERROR_4;
                        }
                        else if(first_operand->table_item->data.varType == TYPE_NIL){
                            left_operand = first_operand->table_item;
                            first_operand->table_item->data.varType = second_operand->table_item->data.varType;
                            
                            if(second_operand->constant == true){
                                generateVariable(&var);
                                if(tableInsert(symTable, symTable->root, &var) != 0){
                                    delete_stack(&stack);
                                    errorVar = INTERNAL_ERROR;
                                    return INTERNAL_ERROR;
                                }
                                right_operand = tableSearch(symTable, symTable->root, &var);
                                right_operand->data.varType = second_operand->table_item->data.varType;
                                right_operand->data.varValue.i_value = second_operand->table_item->data.varValue.i_value;
                                left_operand->data.varType = right_operand->data.varType;
                            }
                            else{
                                right_operand = second_operand->table_item;
                                left_operand->data.varType = right_operand->data.varType;
                                
                            }
                        }
                        //Kontrola, zda-li musime na zasobniku vytvorit pomocnou promennou, do ktere se ulozi vysledek nasledujici operace
                        if(getNewVar(&stack)){
                            generateVariable2(&var);
                            if(tableInsert(symTable, symTable->root, &var) != 0){
                                delete_stack(&stack);
                                errorVar = INTERNAL_ERROR;
                                return INTERNAL_ERROR;
                            }
                            left = tableSearch(table, table->root, &var);
                            left->data.varType = left_operand->data.varType;
                            generateInstruction(I_DEFVAR, (void *)left, NULL, NULL);
                        }
                        
                        
                        //Provedeni operace a ulozeni na stack
                        switch(operator->table_item->data.varType){
                                
                                
                            case PLUS:
                                if(left_operand->data.varType == TYPE_STRING){
                                    generateInstruction(I_CONCAT, (void *)left, (void *)left_operand, (void *)right_operand);
                                    first_operand->table_item = left;
                                    first_operand->constant = false;
                                    first_operand->inTable = true;
                                    first_operand->table_item->data.varType = TYPE_STRING;
                                }
                                else{   //TYPE_INT nebo TYPE_FLOAT
                                    generateInstruction(I_ADD, (void *)left, (void *)left_operand, (void *)right_operand);
                                    first_operand->table_item = left;
                                    first_operand->constant = false;
                                    first_operand->inTable = true;
                                }
                                break;
                                
                            case MINUS:
                                
                                generateInstruction(I_SUB, (void *)left, (void *)left_operand, (void *)right_operand);
                                first_operand->table_item = left;
                                first_operand->constant = false;
                                first_operand->inTable = true;
                                
                                break;
                                
                            case MULT:
                                
                                generateInstruction(I_MUL, (void *)left, (void *)left_operand, (void *)right_operand);
                                first_operand->table_item = left;
                                first_operand->constant = false;
                                first_operand->inTable = true;
                                
                                break;
                                
                                break;
                            case DIV:
                                
                                if(left_operand->data.varType == right_operand->data.varType){
                                    if(left_operand->data.varType == TYPE_INT){
                                        generateInstruction(I_IDIV, (void *)left, (void *)left_operand, (void *)right_operand);
                                        first_operand->table_item = left;
                                        first_operand->constant = false;
                                        first_operand->inTable = true;
                                        
                                        break;
                                    }
                                    else if(left_operand->data.varType == TYPE_FLOAT){
                                        generateInstruction(I_DIV, (void *)left, (void *)left_operand, (void *)right_operand);
                                        first_operand->table_item = left;
                                        first_operand->constant = false;
                                        first_operand->inTable = true;
                                        
                                        break;
                                    }
                                    else{
                                        //ERROR
                                    }
                                }
                                break;
                                
                                
                            case EQUAL:
                                
                                generateInstruction(I_EQ, (void *)left, (void *)left_operand, (void *)right_operand);
                                first_operand->table_item = left;
                                first_operand->inTable = true;
                                first_operand->constant = false;
                                first_operand->table_item->data.varType = TYPE_BOOL;
                                
                                break;
                                
                            case N_EQUAL:
                                
                                generateInstruction(I_EQ, (void *)left, (void *)left_operand, (void *)right_operand);
                                generateInstruction(I_NOT, (void *)left, (void *)left, NULL);
                                first_operand->table_item = left;
                                first_operand->constant = false;
                                first_operand->inTable = true;
                                first_operand->table_item->data.varType = TYPE_BOOL;
                                
                                break;
                                
                            case LOWER:
                                
                                generateInstruction(I_LT, (void *)left, (void *)left_operand, (void *)right_operand);
                                first_operand->table_item = left;
                                first_operand->constant = false;
                                first_operand->inTable = true;
                                first_operand->table_item->data.varType = TYPE_BOOL;
                                
                                break;
                                
                            case L_EQ:
                                
                                generateInstruction(I_GT, (void *)left, (void *)left_operand, (void *)right_operand);
                                generateInstruction(I_NOT, (void *)left, (void *)left, NULL);
                                first_operand->table_item = left;
                                first_operand->constant = false;
                                first_operand->inTable = true;
                                first_operand->table_item->data.varType = TYPE_BOOL;
                                
                                break;
                                
                            case GREATER:
                                
                                generateInstruction(I_GT, (void *)left, (void *)left_operand, (void *)right_operand);
                                first_operand->table_item = left;
                                first_operand->constant = false;
                                first_operand->inTable = true;
                                first_operand->table_item->data.varType = TYPE_BOOL;
                                
                                break;
                                
                            case G_EQ:
                                
                                generateInstruction(I_LT, (void *)left, (void *)left_operand, (void *)right_operand);
                                generateInstruction(I_NOT, (void *)left, (void *)left, NULL);
                                first_operand->table_item = left;
                                first_operand->constant = false;
                                first_operand->inTable = true;
                                first_operand->table_item->data.varType = TYPE_BOOL;
                                
                                break;
                                
                            default:
                                delete_stack(&stack);
                                errorVar = SYNTAX_ERROR;
                                return SYNTAX_ERROR;
                                break;
                        }
                        sPop(&stack);
                        sPop(&stack);
                        break;
                }
                break;
                
                
                
            case EQL:   // EQUAL : E -> (E)
                if (sEmpty(&stack)) {errorVar = INTERNAL_ERROR; return INTERNAL_ERROR;}
                item * to_be_deleted = stack.first->next;   // '('
                if(to_be_deleted == NULL) {errorVar = INTERNAL_ERROR; return INTERNAL_ERROR;}
                item * after;
                if(to_be_deleted->next == NULL){after = NULL;}
                else{after = to_be_deleted->next;}
                stack.first->next = after;
                free(to_be_deleted);
                to_be_deleted = NULL;
                if((token = getNextToken(&attr)) == LEX_ERROR) {delete_stack(&stack); errorVar = LEX_ERROR; return LEX_ERROR;}

                break;
            case ERR:   // ERROR
                delete_stack(&stack);
                errorVar = SYNTAX_ERROR;
                return SYNTAX_ERROR;
            case ACC: // ACCEPT : $C$
                if(first_item != NULL){
                    //Assignment, nothing extra to generate
                }
                else{
                    generateVariable(&var);
                    if(tableInsert(symTable, symTable->root, &var) != 0){
                        delete_stack(&stack);
                        errorVar = INTERNAL_ERROR;
                        return INTERNAL_ERROR;
                    }
                    tTableItem * table_item = tableSearch(symTable, symTable->root, &var);
                    table_item->data.varType = TYPE_BOOL;
                    table_item->data.varValue.b_value = true;
                    generateInstruction(I_JMPNQ, NULL, (void *)left, (void *)table_item);
                    
                }
                delete_stack(&stack);
                return SYNTAX_OK;
            default:
                delete_stack(&stack);
                errorVar = INTERNAL_ERROR;
                return INTERNAL_ERROR;
        }
    }
    
    //generateInstruction(I_VAL, (void *) left, NULL, NULL);
    return SYNTAX_OK;
}
