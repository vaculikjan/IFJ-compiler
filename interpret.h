/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  interpret.h
 *
 *
 *  Prevzato z oficialnich stranek projektu
 *
 *
 *  
 *
 *
 */

#ifndef interpret_h
#define interpret_h

#include <stdio.h>
#include "ilist.h"
#include "symtable.h"

// hlavicka pro interpret
void inter(tSymbolTable *ST, tListOfInstr *instrList);


#endif /* interpret_h */
