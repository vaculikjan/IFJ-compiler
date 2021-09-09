/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  interpret.c
 *
 *
 *  xvacul30 - Jan Vaculik
 *
 *  xmensi13 - Jan Mensik
 *
 *
 *
 */

#include "interpret.h"
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "symtable.h"
#include "ilist.h"


void inter(tSymbolTable *ST, tListOfInstr *instrList)
{
    listFirst(instrList);
    tInstr *I;
    I = listGetData(instrList);
    printf(".IFJcode18\n");
    while (instrList->active != NULL)
    {
        I = listGetData(instrList);
        
        switch (I->instType)
        {
                
            case I_WRITE:
                
                if ((strncmp(((tTableItem*)I->addr1)->key.str, "$", 1)) == 0) {
                    
                    printf("WRITE string@%s\n", ((tTableItem*)I->addr1)->data.varValue.s_value.str);
                }
                else {
                    
                    switch (I->frame) {
                        case TF:
                            printf("WRITE TF@%s\n", ((tTableItem*)I->addr1)->key.str);
                            break;
                            
                        case LF:
                            printf("WRITE LF@%s\n", ((tTableItem*)I->addr1)->key.str);
                            break;
                            
                        case GF:
                            printf("WRITE GF@%s\n", ((tTableItem*)I->addr1)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                break;
                
            case I_DEFVAR:
                
                switch (I->frame) {
                    case TF:
                        printf("DEFVAR TF@%s\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("DEFVAR LF@%s\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("DEFVAR GF@%s\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                break;
                
            case I_READ_I:
                
                switch (I->frame) {
                    case TF:
                        printf("READ TF@%s int\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("READ TF@%s int\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("READ GF@%s int\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                break;
                
            case I_READ_S:
                
                switch (I->frame) {
                    case TF:
                        printf("READ TF@%s string\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("READ LF@%s string\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("READ GF@%s string\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                break;
                
            case I_READ_F:
                
                switch (I->frame) {
                    case TF:
                        printf("READ TF@%s float\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("READ LF@%s float\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("READ GF@%s float\n", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                break;
                
                
            case I_STRI2INT:
                
                switch (I->frame) {
                    case TF:
                        printf("STRI2INT TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("STRI2INT LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("STRI2INT GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    printf("string@%s ", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                }
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                }
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
                
            case I_INT2CHAR:
                
                switch (I->frame2) {
                    case TF:
                        printf("INT2CHAR TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("INT2CHAR LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("INT2CHAR GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    printf("int@%d\n", ((tTableItem*)I->addr2)->data.varValue.i_value);
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_MOVE:
                
                switch (I->frame) {
                    case TF:
                        printf("MOVE TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("MOVE LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("MOVE GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if (I->addr2 == NULL) {
                    printf("nil@nil\n");
                    break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    switch (((tTableItem*)I->addr2)->data.varType) {
                        case TYPE_INT:
                            printf("int@%d\n", ((tTableItem*)I->addr2)->data.varValue.i_value);
                            break;
                        case TYPE_FLOAT:
                            printf("float@%a\n", ((tTableItem*)I->addr2)->data.varValue.f_value);
                            break;
                        case TYPE_STRING:
                            printf("string@%s\n", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s\n",((tTableItem*)I->addr2)->key.str);
                            break;
                        case LF:
                            printf("LF@%s\n",((tTableItem*)I->addr2)->key.str);
                            break;
                        case GF:
                            printf("GF@%s\n",((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_STRLEN:
                
                switch (I->frame) {
                    case TF:
                        printf("STRLEN TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("STRLEN LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("STRLEN GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    printf("string@%s\n", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_ADD:
                
                switch (I->frame) {
                    case TF:
                        printf("ADD TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("ADD LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("ADD GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_SUB:
                
                switch (I->frame) {
                    case TF:
                        printf("SUB TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("SUB LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("SUB GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
                
            case I_EQ:
                
                switch (I->frame) {
                    case TF:
                        printf("EQ TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("EQ LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("EQ GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_STRING) {
                        printf("string@%s ", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr2)->data.varValue.b_value) {
                            printf("bool@true ");
                        }
                        else {
                            printf("bool@false ");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_STRING) {
                        printf("string@%s\n", ((tTableItem*)I->addr3)->data.varValue.s_value.str);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_NIL) {
                        printf("nil@nil\n");
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr3)->data.varValue.b_value) {
                            printf("bool@true\n");
                        }
                        else {
                            printf("bool@false\n");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                break;
                
            case I_MUL:
                
                switch (I->frame) {
                    case TF:
                        printf("MUL TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("MUL LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("MUL GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_DIV:
                
                switch (I->frame) {
                    case TF:
                        printf("DIV TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("DIV LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("DIV GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_IDIV:
                
                switch (I->frame) {
                    case TF:
                        printf("IDIV TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("IDIV LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("IDIV GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_NOT:
                
                switch (I->frame) {
                    case TF:
                        printf("NOT TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("NOT LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("NOT GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr2)->data.varValue.b_value) {
                            printf("bool@true\n");
                        }
                        else {
                            printf("bool@false\n");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_LT:
                
                switch (I->frame) {
                    case TF:
                        printf("LT TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("LT LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("LT GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr2)->data.varValue.b_value) {
                            printf("bool@true ");
                        }
                        else {
                            printf("bool@false ");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr3)->data.varValue.b_value) {
                            printf("bool@true ");
                        }
                        else {
                            printf("bool@false ");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                break;
                
            case I_GT:
                
                switch (I->frame) {
                    case TF:
                        printf("GT TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("GT LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("GT GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr2)->data.varValue.b_value) {
                            printf("bool@true ");
                        }
                        else {
                            printf("bool@false ");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr3)->data.varValue.b_value) {
                            printf("bool@true ");
                        }
                        else {
                            printf("bool@false ");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                break;
                
            case I_JMPNQ:
                printf("JUMPIFNEQ %s ",((tTableItem*)I->addr1)->key.str);
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_STRING) {
                        printf("string@%s ", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr2)->data.varValue.b_value) {
                            printf("bool@true ");
                        }
                        else {
                            printf("bool@false ");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_STRING) {
                        printf("string@%s\n", ((tTableItem*)I->addr3)->data.varValue.s_value.str);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr3)->data.varValue.b_value == true) {
                            printf("bool@true\n");
                        }
                        else {
                            printf("bool@false\n");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                break;
                
            case I_JMPEQ:
                printf("JUMPIFEQ %s ",((tTableItem*)I->addr1)->key.str);
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_FLOAT) {
                        printf("float@%a ", ((tTableItem*)I->addr2)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_STRING) {
                        printf("string@%s ", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                    }
                    else if (((tTableItem*)I->addr2)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr2)->data.varValue.b_value) {
                            printf("bool@true ");
                        }
                        else {
                            printf("bool@false ");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr3)->data.varType == TYPE_INT) {
                        printf("int@%d\n", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_FLOAT) {
                        printf("float@%a\n", ((tTableItem*)I->addr3)->data.varValue.f_value);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_STRING) {
                        printf("string@%s\n", ((tTableItem*)I->addr3)->data.varValue.s_value.str);
                    }
                    else if (((tTableItem*)I->addr3)->data.varType == TYPE_BOOL) {
                        if (((tTableItem*)I->addr3)->data.varValue.b_value == true) {
                            printf("bool@true\n");
                        }
                        else {
                            printf("bool@false\n");
                        }
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                break;
                
            case I_I2F:
                switch (I->frame) {
                    case TF:
                        printf("INT2FLOAT TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("INT2FLOAT LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("INT2FLOAT GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr2)->data.varValue.i_value);
                    }
                    
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                break;
                
            case I_CONCAT:
                
                switch (I->frame) {
                    case TF:
                        printf("CONCAT TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("CONCAT LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("CONCAT GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_STRING) {
                        printf("string@%s ", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_STRING) {
                        printf("string@%s\n", ((tTableItem*)I->addr3)->data.varValue.s_value.str);
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_GETCHAR:
                
                switch (I->frame) {
                    case TF:
                        printf("GETCHAR TF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case LF:
                        printf("GETCHAR LF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    case GF:
                        printf("GETCHAR GF@%s ", ((tTableItem*)I->addr1)->key.str);
                        break;
                        
                    default:
                        break;
                }
                
                if ((strncmp(((tTableItem*)I->addr2)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_STRING) {
                        printf("string@%s ", ((tTableItem*)I->addr2)->data.varValue.s_value.str);
                    }
                }
                
                else {
                    
                    switch (I->frame2) {
                        case TF:
                            printf("TF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s ", ((tTableItem*)I->addr2)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                if ((strncmp(((tTableItem*)I->addr3)->key.str, "$", 1)) == 0) {
                    
                    if (((tTableItem*)I->addr2)->data.varType == TYPE_INT) {
                        printf("int@%d ", ((tTableItem*)I->addr3)->data.varValue.i_value);
                    }
                }
                
                else {
                    
                    switch (I->frame3) {
                        case TF:
                            printf("TF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case LF:
                            printf("LF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        case GF:
                            printf("GF@%s\n", ((tTableItem*)I->addr3)->key.str);
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                
                break;
                
            case I_LAB:
                printf("LABEL %s\n", ((tTableItem*)I->addr1)->key.str);
                break;
            case I_JMP:
                printf("JUMP %s\n", ((tTableItem*)I->addr1)->key.str);
                break;
            case I_PUSHF:
                printf("PUSHFRAME\n");
                break;
            case I_POPF:
                printf("POPFRAME\n");
                break;
            case I_CALL:
                printf("CALL %s\n", ((tTableItem*)I->addr1)->key.str);
                break;
            case I_RET:
                printf("RETURN\n");
                break;
            case I_CREATEF:
                printf("CREATEFRAME\n");
                break;
                
            default:
                break;
        }
        
        // prejdeme na dalsi instrukci
        // POZOR! Pokud byl proveden skok, nemelo by se posouvat na dalsi instrukci!
        // Protoze ale vime, ze skok mohl byt proveden pouze na navesti a v ramci
        // navesti se nic neprovadi, muzeme si dovolit prejit na dalsi instrukci i
        // v tomto pripade, pouze tim urychlime beh programu
        listNext(instrList);
        
    }
    
}


