/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  symtable.c
 *
 *
 *  xvacul30 - Jan Vaculik
 *
 *  xmensi13 - Jan Mensik
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "symtable.h"

void tableInit(tSymbolTable *T)
// funkce inicializuje tabulku symbolu
{
    T->root = NULL;
    T->next = NULL;
}

int tableInsert(tSymbolTable *T, tTableItem *active, string *key)
// funkce vlozi do tabulky symbolu novy identifikator
{

    if(active == NULL){           //pridani uzlu na root
        tTableItem *new_item;
        new_item = malloc(sizeof(tTableItem));
        
        strInit(&(new_item->key));
        strCopyString(&(new_item->key), key);
        new_item->data.varValue.fn_value.numberOfParams = -1;
        new_item->lptr = NULL;
        new_item->rptr = NULL;
        new_item->data.varType = TYPE_NIL;
        new_item->function = false;
        new_item->declared = false;
        new_item->data.varValue.fn_value.numberOfParams = -1;
        new_item->ifElse = 0;
        
        T->root = new_item;
        
        return 0;
    }
    else
    {
        if(strCmpString(key, &active->key) < 0)
        {
            if(active->lptr == NULL)
            {
                //vkladame na root->lptr
                tTableItem *new_item;
                new_item = malloc(sizeof(tTableItem));
                
                strInit(&(new_item->key));
                strCopyString(&(new_item->key), key);
                new_item->lptr = NULL;
                new_item->rptr = NULL;
                new_item->data.varType = TYPE_NIL;
                new_item->function = false;
                new_item->declared = false;
                new_item->data.varValue.fn_value.numberOfParams = -1;
                new_item->ifElse = 0;
                
                
                active->lptr = new_item;
                
                return 0;
            }
            else
            {
                //rekurze nad lptr
                return tableInsert(T, active->lptr, key);
            }
        }
        else if(strCmpString(key, &active->key) > 0)
        {
            if(active->rptr == NULL)
            {
                //vkladame na root->rptr
                tTableItem *new_item;
                new_item = malloc(sizeof(tTableItem));
                
                strInit(&(new_item->key));
                strCopyString(&(new_item->key), key);
                new_item->lptr = NULL;
                new_item->rptr = NULL;
                new_item->data.varType = TYPE_NIL;
                new_item->function = false;
                new_item->declared = false;
                new_item->data.varValue.fn_value.numberOfParams = -1;
                new_item->ifElse = 0;
                
                
                active->rptr = new_item;
                
                return 0;
            }
            else
            {
                //rekurze nad rptr
                return tableInsert(T, active->rptr, key);
            }
        }
        else    // strcmp vraci 0, nasel item
        {
            return 1;
        }
    }
}

tTableItem *tableSearch(tSymbolTable *T, tTableItem *active, string *key)
// pokud se dana polozka s klicem key v tabulce symbolu nachazi,
// funkce vrati ukazatel na data teto polozky, jinak vrati NULL
{
    
    if(active == NULL)
    {
        return NULL;
    }
    
    else
    {
        if(strCmpString(key, &active->key) < 0)
        {
            
            //rekurze nad lptr
            return tableSearch(T, active->lptr, key);
        }
        else if(strCmpString(key, &active->key) > 0)
        {
            //rekurze nad rptr
            return tableSearch(T, active->rptr, key);
        }
        else    // strcmp vraci 0, nasel item se spravnym klicem a funkce vraci jeho data
        {
            return active;
        }
    }
}

void tableFree(tSymbolTable *T, tTableItem *active)
// funkce dealokuje tabulku symbolu
{
    //    tTableItem *ptr;
    if (T == NULL) return;
    if (active == NULL) return;
    
    while (active != NULL)
    {
        if (active->lptr != NULL)
        {
            tableFree(T, active->lptr);
            active->lptr = NULL;
        }
        else if (active->rptr != NULL)
        {
            tableFree(T, active->rptr);
            active->rptr = NULL;
        }
        else
        {
            if (active->data.varType == TYPE_STRING) {
                if ((strncmp(active->key.str, "$", 1)) == 0) {
                    strFree(&active->data.varValue.s_value);
                }
            }
            strFree(&active->key);
            
            if (T->root->lptr == NULL && T->root->rptr == NULL){
                T->root = NULL;
                free (active);
                return;
            }
            
            free (active);
            return;
        }
    }
}

bool functionsDeclared(tSymbolTable *T, tTableItem *active)
{
    bool result;
    
    if(active->lptr != NULL)
    {
        result = functionsDeclared(T, active->lptr);
        if (!result) {
            return false;
        }
    }
    
    if(active->rptr != NULL)
    {
        result = functionsDeclared(T, active->rptr);
        if (!result) {
            return false;
        }
    }
    
    if(active->function)
    {
        if(!active->declared) return false;
    }
    return true;
    
}

tSymbolTable *tableReturn(tTableList *tableList, string *key)
{
    
    tSymbolTable *buffer = tableList->first;
    while (buffer != NULL)
    {
        if ((strCmpString(&buffer->key, key)) == 0)
        {
            return buffer;
        }
        buffer = buffer->next;
        
    }
    return buffer;
}

int tableTableInsert (tTableList *tableList, string *key) {
   
    int found = 0;
    tSymbolTable *buffer;
    buffer = tableReturn(tableList, key);
    if (buffer != NULL)
    {
        found = 1;
    }
    else
    {
        buffer = malloc(sizeof(tSymbolTable));
        tableInit(buffer);
        strInit(&buffer->key);
        strCopyString(&buffer->key, key);
        tSymbolTable *helper = tableList->first;
        while (helper->next != NULL) {
            helper = helper->next;
        }
        helper->next = buffer;
    }
    
    return found;
}

void tableTableFree (tTableList *tableList)
{
    while(tableList->first != NULL) {
        
        tSymbolTable *buffer;
        buffer = tableList->first;
        tableList->first = buffer->next;
        tableFree(buffer, buffer->root);
        strFree(&buffer->key);
        free(buffer);
    }
    
    tableList->first = NULL;
    
}
