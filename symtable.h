/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  symtable.h
 *
 *
 *  xvacul30 - Jan Vaculik
 *
 *  xmensi13 - Jan Mensik
 *
 *
 *
 */

#ifndef symtable_h
#define symtable_h

#include <stdio.h>
#include <stdbool.h>
#include "str.h"

// definice jednotlivych typu
#define TYPE_INT    1
#define TYPE_BOOL   2
#define TYPE_FLOAT  3
#define TYPE_STRING 4
#define TYPE_NIL    5

typedef struct tFunction
{
    string * params;
    struct tFunction *nextParam;
    int numberOfParams;
    
} tFunction;

typedef struct
{
    int i_value;
    bool b_value;
    float f_value;
    string s_value;
    tFunction fn_value;
    
} tValueType;

typedef struct
{
    int varType;  // typ dane promenne, v nasem pripade vzdy int
    tValueType varValue; // pro ruzne typy nutnost resit pomoci unie
} tData;

typedef struct tableItem
{
    string key;                  // klic, podle ktereho se bude vyhledavat = nazev identifikatoru
    tData data;                  // data, ktera jsou ke klici pridruzena
    bool function;
    bool declared;
    struct tableItem *lptr;
    struct tableItem *rptr;// ukazatel na dalsi prvek tabulky symbolu
    int ifElse;
} tTableItem;

typedef struct tSymbolTable
{
    struct tableItem *root;
    struct tSymbolTable *next;
    string key;
} tSymbolTable;

typedef struct tTableList
{
    tSymbolTable *first;
} tTableList;



void tableInit(tSymbolTable *T);
int tableInsert(tSymbolTable *T, tTableItem *active, string *key);
tTableItem *tableSearch(tSymbolTable *T, tTableItem *active, string *key);
void tableFree(tSymbolTable *T, tTableItem *active);
bool functionsDeclared(tSymbolTable *T, tTableItem *active);
tSymbolTable *tableReturn(tTableList *tableList, string *key);
int tableTableInsert (tTableList *tableList, string *key);
void tableTableFree (tTableList *tableList);

#endif /* symtable_h */
