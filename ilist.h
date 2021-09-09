/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  ilist.h
 *
 *
 *  Castecne prevzato z oficialnich stranek projektu
 *  xvacul30 - Jan Vaculik
 *
 *  xmensi13 - Jan Mensik
 *
 *
 */

#ifndef ilist_h
#define ilist_h

#include <stdio.h>
// typy jednotlivych instrukci
#define I_NOT       0
#define I_READ      1
#define I_WRITE     2
#define I_STRI2INT  3
#define I_INT2CHAR  4
#define I_LAB       7
#define I_IFGOTO    8
#define I_GOTO      9
#define I_DEFVAR    10
#define I_READ_I    11
#define I_READ_S    12
#define I_READ_F    13
#define I_CALL      14
#define I_CREATEF   15
#define I_MOVE      16
#define I_PUSHF     17
#define I_RET       18
#define I_POPF      19
#define I_VAL       20
#define I_STRLEN    21
#define I_ADD       22
#define I_SUB       23
#define I_MUL       24
#define I_DIV       25
#define I_IDIV      26
#define I_EQ        27
#define I_LT        28
#define I_GT        29
#define I_JMPNQ     30
#define I_JMP       31
#define I_I2F       32
#define I_JMPEQ     33
#define I_CONCAT    34
#define I_SUBSTR    35
#define I_GETCHAR   36



#define LF              1
#define TF              2
#define GF              3

typedef struct
{
    int instType;   // typ instrukce
    int frame;      // rámec
    int frame2;
    int frame3;
    void *addr1;    // adresa 1
    void *addr2;    // adresa 2
    void *addr3;    // adresa 3
} tInstr;

typedef struct listItem
{
    tInstr Instruction;
    struct listItem *nextItem;
} tListItem;

typedef struct
{
    struct listItem *first;  // ukazatel na prvni prvek
    struct listItem *last;   // ukazatel na posledni prvek
    struct listItem *active; // ukazatel na aktivni prvek
} tListOfInstr;

void listInit(tListOfInstr *L);
void listFree(tListOfInstr *L);
void listInsertLast(tListOfInstr *L, tInstr I);
void listFirst(tListOfInstr *L);
void listNext(tListOfInstr *L);
void listGoto(tListOfInstr *L, void *gotoInstr);
void *listGetPointerLast(tListOfInstr *L);
tInstr *listGetData(tListOfInstr *L);
void listGotoLast(tListOfInstr *L);


#endif /* ilist_h */
