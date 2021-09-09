/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  parser.h
 *
 *
 *  xvacul30 - Jan Vaculik
 *  xmensi13 - Jan Mensik
 *
 *  xtrejt00 - Martin Trejtnar
 *
 *
 */

#ifndef parser_h
#define parser_h

#include "ilist.h"
#include "stack.h"
#define SYNTAX_OK       1
#define SYNTAX_ERROR    2
#define SEM_ERROR_3     3
#define SEM_ERROR_4     4
#define SEM_ERROR_5     5
#define SEM_ERROR_6     6
#define DIV_ZERO        9
#define INTERNAL_ERROR 99




// hlavicka pro syntakticky analyzator
int parse(tTableList *tableList, tListOfInstr *instrList);
bool program(void);
bool statList(void);
bool statement(void);
bool term(void);
bool otherTerms(void);
bool idExpr(void);
bool num(void);
bool params(tSymbolTable *fnTable, int *params);
bool callFunction(void);
void generateVariable(string *variable);
void generateInstruction(int instType, void *addr1, void *addr2, void *addr3);
void generateEmpty(void);

//---------------PSA-------------------
item * create_item(tTableItem *table_item, int type, int i_value, float f_value, string *s_value);

int getIndex(item * top);
int getIndexOfToken(int token);

item * getFirstTerm(tStack * stack);
int howManyTermsToPop(tStack * stack);
bool getNewVar(tStack * stack);

int expr(tTableItem * first_token, tSymbolTable * table, tTableItem * to_assign, int mod);
int processExpr(tTableItem * first_item, tSymbolTable * table, tTableItem * to_assign);


#endif /* parser_h */
