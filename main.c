/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  main.c
 *
 *
 *  xmensi13 - Jan Mensik
 *  xvacul30 - Jan Vaculik
 *  xtrejt00 - Martin Trejtnar
 *  xlesko06 - Patrik Lesko
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "symtable.h"
#include "ilist.h"
#include "scanner.h"
#include "parser.h"
#include "interpret.h"


int main(int argc, char** argv)
{
    if (argc != 1)
    {
        return INTERNAL_ERROR;
    }
    
    tTableList *tableList;
    tableList = malloc(sizeof(tTableList));
    tSymbolTable *ST;
    ST = malloc(sizeof(tSymbolTable));
    tableInit(ST); // inicializace tabulky symbolu
    string var;
    strInit(&var);
    
    strAddChar(&var, 'm');
    strAddChar(&var, 'a');
    strAddChar(&var, 'i');
    strAddChar(&var, 'n');

    ST->key = var;
    tableList->first = ST;
    
    tListOfInstr instrList;
    listInit(&instrList); // inicializace seznamu instrukci
    
    int result;
    
    result = parse(tableList, &instrList); // provedeme syntaktickou analyzu

    switch (result)
    {
        case LEX_ERROR:
        case SYNTAX_ERROR:
        case SEM_ERROR_3:
        case SEM_ERROR_4:
        case SEM_ERROR_5:
        case SEM_ERROR_6:
        case DIV_ZERO:
        case INTERNAL_ERROR:
        
            tableTableFree(tableList);
            listFree(&instrList);
            free (tableList);
            return result;
            break;
        
        default:
            break;
    }
    
    
    // provedeme interpretaci
    if (result == SYNTAX_OK) {
        inter(ST, &instrList);
    }
    
    tableTableFree(tableList);
    listFree(&instrList);
    free (tableList);
    return result;
}
