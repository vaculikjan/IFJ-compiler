/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  stack.c
 *
 *
 *  xvacul13 - Jan Mensik
 *
 *  xtrejt00 - Martin Trejtnar
 *
 *
 *
 */
#include <stdlib.h>
#include "stack.h"


void sInit(tStack *stack){
    stack->first = NULL;
}

void sPush(tStack *stack, item * to_push){
    if(sEmpty(stack)){
        stack->first = to_push;
    }
    else{
        item * tmp;
        tmp = stack->first;
        stack->first = to_push;
        to_push->next = tmp;
    }
}

void sPop(tStack *stack){
    if(sEmpty(stack)) return;
    item * to_pop;
    to_pop = stack->first;
    if(to_pop->next == NULL){
        stack->first = NULL;
    }

    stack->first = to_pop->next;

    if(to_pop->inTable == false){

        if(to_pop->table_item->data.varType == TYPE_STRING){
            strFree(&to_pop->table_item->data.varValue.s_value);
        }
        free(to_pop->table_item);
    }
    free(to_pop);
    return;
}

item * sTop(tStack *stack){
    if(sEmpty(stack)) return NULL;
    return stack->first;
}

item sCopyFirst(tStack * stack){
    return *(stack->first);
}


bool sEmpty(tStack *stack){
    return stack->first == NULL ? true : false;
}

void delete_stack(tStack *stack){
    while(!(sEmpty(stack))){
        item * old_first;
        item * new_first;
        
        old_first = stack->first;
        new_first = old_first->next;
        
        if(new_first == NULL){
            stack->first = NULL;
        }
        
        stack->first = new_first;
        if(old_first->inTable == false){
            if(old_first->table_item->data.varType == TYPE_STRING){
                strFree(&old_first->table_item->data.varValue.s_value);
            }
            free(old_first->table_item);
        }
        free(old_first);
    }
    return;
}
