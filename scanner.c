/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  scanner.c
 *
 *
 *  xmensi13 - Jan Mensik
 *
 *  xvacul30 - Jan Vaculik
 *  xtrejt00 - Martin Trejtnar
 *  xlesko06 - Patrik Lesko
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "str.h"
#include "scanner.h"


int getNextToken(string *attr)
// hlavni funkce lexikalniho analyzatoru
{
    typedef enum{
        START,
        IDENTIFIER,
        OPERATOR,
        STRING,
        LINE_COMMENT,
        INT,
        INT_EXP_SIGN,
        INT_EXP,
        FLOAT,
        FLOAT_EXP_SIGN,
        FLOAT_EXP,
        BLOCK_COMMENT_CHECK,
        BLOCK_COMMENT,
        BLOCK_COMMENT_END
        
    } t_state;
    
    t_state state = START;
    char c;
    // vymazeme obsah atributu a v pripade identifikatoru
    // budeme postupne do nej vkladat jeho nazev
    strClear(attr);
    while (1)
    {
        // nacteni dalsiho znaku
        c = fgetc(stdin);
        
        switch (state)
        {
            case START:
                // zakladni stav automatu
                if (c == EOF)
                {
                    return END_OF_FILE;
                }
                
                else if(c == '\n' || '\0')
                {
                    // stav symbolu konce radku
                    
                    return NEW_LINE;
                    
                }
                
                else if (isspace(c))
                    // bila mista - ignorovat
                {
                    state = START;
                }
                else if (c == '#')
                    // komentar
                {
                    state = LINE_COMMENT;
                }
                else if (islower(c) || c == '_')
                    // identifikator nebo klicove slovo
                {
                    strAddChar(attr, c);
                    state = IDENTIFIER;
                }
                else if (c == '+')
                {
                    return PLUS;
                }
                else if (c == '-')
                {
                    return MINUS;
                }
                else if(c == '*')
                {
                    return MULT;
                }
                else if(c == '/')
                {
                    return DIV;
                }
                else if(c == '<' || c == '>' || c == '!' || c == '=')
                {
                    strAddChar(attr, c);
                    state = OPERATOR;
                }
                else if (isdigit(c))
                {
                    strAddChar(attr, c);
                    state = INT;
                }
                else if (c == '"')
                {
                    state = STRING;
                }
                else if (c == ',')
                {
                    return COMMA;
                }
                else if (c == '(')
                {
                    strAddChar(attr, c);
                    return L_BRACKET;
                }
                else if (c == ')')
                {
                    return R_BRACKET;
                }
                else
                {
                    return LEX_ERROR;
                }
                
                break;
                
                
            case LINE_COMMENT:
                // komentar
                if (c == '\n' || c == EOF)
                {
                    ungetc(c, stdin);
                    state = START;
                }
                
                break;
                
                
            case IDENTIFIER:
                // identifikator nebo klicove slovo
                if (isalnum(c) || c == '_')
                {
                    // identifikator pokracuje
                    strAddChar(attr, c);
                }
                else if (c == '!' || c == '?')
                {
                    strAddChar(attr, c);
                    return ID;
                }
                // konec identifikatoru
                else if (isspace(c) || c == '(' || c == ',' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>')
                    // kontrola, zda se nejedna o klicove slovo
                {
                    ungetc(c, stdin);
                    //Keywords
                    if (strCmpConstStr(attr, "def") == 0) return DEF;
                    else if (strCmpConstStr(attr, "do") == 0) return DO;
                    else if (strCmpConstStr(attr, "else") == 0) return ELSE;
                    else if (strCmpConstStr(attr, "end") == 0) return END;
                    else if (strCmpConstStr(attr, "if") == 0) return IF;
                    else if (strCmpConstStr(attr, "not") == 0) return NOT;
                    else if (strCmpConstStr(attr, "nil") == 0) return NIL;
                    else if (strCmpConstStr(attr, "then") == 0) return THEN;
                    else if (strCmpConstStr(attr, "while") == 0) return WHILE;
                    //Built-in functions
                    else if (strCmpConstStr(attr, "inputs") == 0) return INPUTS;
                    else if (strCmpConstStr(attr, "inputi") == 0) return INPUTI;
                    else if (strCmpConstStr(attr, "inputf") == 0) return INPUTF;
                    else if (strCmpConstStr(attr, "print") == 0) return WRITE;
                    else if (strCmpConstStr(attr, "length") == 0) return STRLEN;
                    else if (strCmpConstStr(attr, "substr") == 0) return SUBSTR;
                    else if (strCmpConstStr(attr, "ord") == 0) return STRI2INT;
                    else if (strCmpConstStr(attr, "chr") == 0) return INT2CHAR;
                    else return ID;
                }
                else return LEX_ERROR;
                
                break;
                
                
            case OPERATOR:
                
                if (c == '=')
                {
                    if (strCmpConstStr(attr, "<") == 0) {strClear(attr); return L_EQ;}
                    else if (strCmpConstStr(attr, ">") == 0) {strClear(attr); return G_EQ;}
                    else if (strCmpConstStr(attr, "=") == 0) {strClear(attr); return EQUAL;}
                    else if (strCmpConstStr(attr, "!") == 0) {strClear(attr); return N_EQUAL;}
                    else return LEX_ERROR;
                }
                else if(isspace(c) || isalnum(c) || c == '_')
                {
                    ungetc(c, stdin);
                    if (strCmpConstStr(attr, "<") == 0) {strClear(attr); return LOWER;}
                    else if (strCmpConstStr(attr, ">") == 0) {strClear(attr); return GREATER;}
                    else if (strCmpConstStr(attr, "=") == 0) {strClear(attr); return ASSIGN;}
                    else return LEX_ERROR;
                }
                else if(c == 'b')
                {
                    if (strCmpConstStr(attr, "=") == 0) {
                        strAddChar(attr, c);
                        state = BLOCK_COMMENT_CHECK;
                    }
                    else return LEX_ERROR;
                }
                
                else return LEX_ERROR;
                
                break;
                
            case INT:
                
                if(isdigit(c))
                {
                    strAddChar(attr, c);
                }
                else if(c == '.')
                {
                    strAddChar(attr, c);
                    state = FLOAT;
                }
                else if(c == 'E' || c == 'e')
                {
                    strAddChar(attr, c);
                    state = INT_EXP_SIGN;
                }
                else if(isspace(c) || c == ')' || c == ',' || c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>' )
                {
                    ungetc(c, stdin);
                    return INT_LIT;
                }
                else return LEX_ERROR;
                
                break;
                
                
            case INT_EXP_SIGN:
                
                if(c == '+' || c == '-' || isdigit(c))
                {
                    strAddChar(attr, c);
                    state = INT_EXP;
                }
                else return LEX_ERROR;
                
                break;
                
            case INT_EXP:
                
                if(isdigit(c))
                {
                    strAddChar(attr, c);
                }
                else if(c == ')' || c == ',' || c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '!' || c == '<' || c == '>' || isspace(c))
                {
                    ungetc(c, stdin);
                    return INT_LIT;
                }
                else return LEX_ERROR;
                
                break;
                
                
            case FLOAT:
                
                if(isdigit(c))
                {
                    strAddChar(attr, c);
                }
                else if(c == 'E' || c == 'e')
                {
                    strAddChar(attr, c);
                    state = FLOAT_EXP_SIGN;
                }
                else if(isspace(c) || c == ')' || c == ',' || c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>')
                {
                    ungetc(c, stdin);
                    return FLOAT_LIT;
                }
                else return LEX_ERROR;
                
                break;
                
                
            case FLOAT_EXP_SIGN:
                
                if(c == '+' || c == '-' || isdigit(c))
                {
                    strAddChar(attr, c);
                    state = FLOAT_EXP;
                }
                else return LEX_ERROR;
                
                break;
                
                
            case FLOAT_EXP:
                
                if(isdigit(c))
                {
                    strAddChar(attr, c);
                }
                else if(c == ')' || c == ',' || c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '!' || c == '<' || c == '>' || isspace(c)){
                    ungetc(c, stdin);
                    return FLOAT_LIT;
                }
                else return LEX_ERROR;
                
                break;
                
                
            case STRING:
                
                if(c == '"')
                {
                    return STRING_LIT;
                }
                else if (c == EOF) return LEX_ERROR;
                else if (c == 92) {
                    //strAddChar(attr, c);
                    c = fgetc(stdin);
                    
                    switch (c) {
                        case 'n':
                            strAddChar(attr, 92);
                            strAddChar(attr, '0');
                            strAddChar(attr, '1');
                            strAddChar(attr, '0');
                            break;
                            
                        case 't':
                            strAddChar(attr, 92);
                            strAddChar(attr, '0');
                            strAddChar(attr, '0');
                            strAddChar(attr, '9');
                            break;
                            
                        case 's':
                            strAddChar(attr, 92);
                            strAddChar(attr, '0');
                            strAddChar(attr, '3');
                            strAddChar(attr, '2');
                            break;
                            
                        case '\\':
                            strAddChar(attr, 92);
                            strAddChar(attr, '0');
                            strAddChar(attr, '9');
                            strAddChar(attr, '2');
                            break;
                            
                        case '"':
                            strAddChar(attr, 92);
                            strAddChar(attr, '0');
                            strAddChar(attr, '3');
                            strAddChar(attr, '2');
                            break;
                            
                            
                        case 'x':
                            c = fgetc(stdin);
                            if(!(isdigit(c) || (c >= 'A' && c <= 'F'))) return LEX_ERROR;
                            string tmp;
                            strInit(&tmp);
                            strAddChar(&tmp, c);
                            
                            c = fgetc(stdin);
                            if(!(isdigit(c) || (c >= 'A' && c <= 'F'))){
                                ungetc(c, stdin);
                            }
                            else strAddChar(&tmp, c);
                            
                            long int decimal = strtol(tmp.str, NULL, 16);
                            printf("Decimal = %ld\n",decimal);
                            sprintf(tmp.str,"%ld",decimal);
                            strAddChar(attr, 92);
                            strcat(attr->str, tmp.str);
                            printf("Attr = %s\n",attr->str);
                            
                            break;
                            
                        default:
                            return LEX_ERROR;
                            break;
                    }
                    
                    
                }
                else if(c == ' ')
                {
                    strAddChar(attr, 92);
                    strAddChar(attr, '0');
                    strAddChar(attr, '3');
                    strAddChar(attr, '2');
                }
                else {
                    strAddChar(attr, c);
                }
                
                break;
                
                
            case BLOCK_COMMENT_CHECK:
                
                if((c == 'e') && (strCmpConstStr(attr, "=b") == 0))
                {
                    strAddChar(attr, c);
                }
                else if ((c == 'g') && (strCmpConstStr(attr, "=be") == 0))
                {
                    strAddChar(attr, c);
                }
                else if ((c == 'i') && (strCmpConstStr(attr, "=beg") == 0))
                {
                    strAddChar(attr, c);
                }
                else if ((c == 'n') && (strCmpConstStr(attr, "=begi") == 0))
                {
                    strAddChar(attr, c);
                }
                else if ((isspace(c)) && (strCmpConstStr(attr, "=begin") == 0))
                {
                    state = BLOCK_COMMENT;
                }
                else
                {
                    return LEX_ERROR;
                }
                
                break;
                
                
            case BLOCK_COMMENT:
                
                if(c == '\n')
                {
                    strClear(attr);
                    strAddChar(attr, c);
                }
                else if ((c == '=') && (strCmpConstStr(attr, "\n") == 0))
                {
                    strAddChar(attr, c);
                }
                else if ((c == 'e') && (strCmpConstStr(attr, "\n=") == 0))
                {
                    strAddChar(attr, c);
                }
                else if ((c == 'n') && (strCmpConstStr(attr, "\n=e") == 0))
                {
                    strAddChar(attr, c);
                }
                else if ((c == 'd') && (strCmpConstStr(attr, "\n=en") == 0))
                {
                    strClear(attr);
                    state = BLOCK_COMMENT_END;
                }
                else if (c == EOF) return LEX_ERROR;
                else
                {
                    strAddChar(attr, c);
                }
                
                break;
                
                
            case BLOCK_COMMENT_END:
                
                if(c == '\n' || c == EOF)
                {
                    state = START;
                }
                
                break;
                
        }
    }
}

