/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  stack.h
 *
 *
 *  xvacul13 - Jan Mensik
 *
 *  xtrejt00 - Martin Trejtnar
 *
 *
 *
 */

#ifndef stack_h
#define stack_h

#include <stdio.h>
#include "symtable.h"

typedef struct item{
    tTableItem * table_item;
    struct item * next;
    bool constant;
    bool reduced;
    bool inTable;
} item;

typedef struct tStack{
    item * first;
} tStack;

void sInit(tStack *stack);
void sPush(tStack *stack, item * to_push);
void sPop(tStack *stack);
item * sTop(tStack *stack);
item sCopyFirst(tStack * stack);
bool sEmpty(tStack *stack);
void delete_stack(tStack *stack);

#endif /* stack_h */
