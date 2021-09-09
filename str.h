/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  str.h
 *
 *
 *  Prevzato z oficialnich stranek projektu
 *
 *
 *
 *
 *
 */

#ifndef str_h
#define str_h

//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

typedef struct
{
    char* str;        // misto pro dany retezec ukonceny znakem '\0'
    int length;        // skutecna delka retezce
    int allocSize;    // velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);

#endif /* str_h */
