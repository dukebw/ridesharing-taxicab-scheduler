#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <stdio.h>

template<typename T>
struct List {
        T item;
        List<T> *next;

        // NOTE(brendan): private constructor so that clients can't make a List
        // object except by using addToList(item, null);
        List() {}

public:
        // NOTE(Zach): empty/destroy the entire list
        static void emptyList(List<T> **list);

        // NOTE(brendan): add item T to the list
        static List<T> *
        addToList(T newitem, List<T> *list);

        // NOTE(brendan): INPUT: list, item index. OUTPUT: pointer to updated
        // list. UPDATE: list; item is inserted such that it is now node 0
        static List<T> *
        insertAt(List<T> *list, T newitem, int index);

        // NOTE(brendan): INPUT: list, index. OUTPUT: item at index
        static bool
        itemAt(T *outputItem, List<T> *list, int index);

        // NOTE(brendan): INPUT: list. OUTPUT: new list. removes head of list
        static List<T> *
        removeHead(List<T> *list);

        // NOTE(brendan): delete the first occurrence of item T; returns the
        // list
        static List<T> *
        deleteFromList(T toDeleteItem, List<T> *list);

        // NOTE(brendan): iterate over list, executing function f() on each node
        static void 
        traverseList(void (*f)(T), List<T> *list);

        // NOTE(brendan): iterate over list, executing function f() on each node
        // and the argument
        static void 
        traverseList(void (*f)(void *, int, T), void *argOne, int argTwo,
                     List<T> *list);

        // NOTE(brendan): iterate over list, executing function f() on each node
        static void 
        traverseList(void (*f)(void *, void *, T), void *argOne, void *argTwo,
                     List<T> *list);

        // NOTE(brendan): write all the items in list contiguously to fp
        static List<T> *
        readListFromFile(List<T> *list, FILE *fp);

        // NOTE(brendan): write all the items in list contiguously to fp
        static void
        writeListToFile(List<T> *list, FILE *fp);

        // NOTE(brendan): iterate over list, returning the item of the first
        // node that satisfies function f
        static T *
        reduceList(bool (*f)(T listItem, T item), T newest, List<T> *list);
};

// NOTE(brendan): add item T to the list
template<typename T> List<T> *
List<T>::addToList(T newItem, List<T> *list) 
{
    List<T> *resultList = (List<T> *)malloc(sizeof(List<T>));
    resultList->item = newItem;
    resultList->next = list;
    return resultList;
}

// NOTE(brendan): INPUT: list, item index. OUTPUT: pointer to updated
// list. UPDATE: list; item is inserted such that it is now node 0
// If index is > the end of the list just inserts at the end
// if index is < 0 treats it as if index == 0
template<typename T> List<T> *
List<T>::insertAt(List<T> *list, T newItem, int index) 
{
    List<T> *resultList = list;
    List<T> *prev = 0;
    for (int i = 0; list && (i < index); list = list->next, ++i) {
        prev = list;
    }
    if (prev) {
        prev->next = List<T>::addToList(newItem, list);
    }
    else {
        resultList = List<T>::addToList(newItem, list);
    }
    return resultList;
}

// NOTE(brendan): INPUT: outputItem, list, index. OUTPUT: false if not found
// true if found. UPDATE: outputItem
template<typename T> bool
List<T>::itemAt(T *outputItem, List<T> *list, int index)
{
    // TODO(brendan): assert outputItem != 0
    if (index >= 0) {
        for (int i = 0; list; list = list->next, ++i) {
            if (i == index) {
                *outputItem = list->item;
                return true;
            }
        }
    }
    return false;
}

// NOTE(brendan): INPUT: list. OUTPUT: new list. removes head of list
template<typename T> List<T> *
List<T>::removeHead(List<T> *list) 
{
    if (list) {
        List<T> *toFreeList = list;
        list = list->next;
        free(toFreeList);
    }
    return list;
}

template<typename T> void 
List<T>::emptyList(List<T> **list) 
{
    List<T> *current = *list;
    List<T> *previous = NULL;
    while (current != NULL) {
        previous = current;
        current = current->next;
        free(previous);
    }
    *list = NULL;
}

// NOTE(brendan): delete the first occurence of item T; returns the list
template<typename T> List<T> *
List<T>::deleteFromList(T toDeleteItem, List<T> *list) 
{
    if (list) {
        List<T> *current;
        List<T> *previous;
        for (current = list, previous = 0; 
             (current != 0) && (current->item != toDeleteItem); 
             previous = current, current = current->next);
        if (current) {
            if (previous) {
                previous->next = current->next;
            }
            else {
                list = list->next;
            }
            free(current);
        }
        return list;
    }
    else {
        return 0;
    }
}

// NOTE(brendan): iterate over list, executing function f() on each node
template<typename T> void 
List<T>::traverseList(void (*f)(T), List<T> *list) 
{
    for(List<T> *current = list;
        current != NULL;
        current = current->next) {
        (*f)(current->item);
    }
}

// NOTE(brendan): iterate over list, executing function f() on each node
// and the argument
template<typename T> void 
List<T>::traverseList(void (*f)(void *, int, T), void *argOne, int argTwo,
                      List<T> *list) 
{
    for(; list; list = list->next) {
        (*f)(argOne, argTwo, list->item);
    }
}

// NOTE(brendan): iterate over list, executing function f() on each node
template<typename T> void 
List<T>::traverseList(void (*f)(void *, void *, T), void *argOne, 
                      void *argTwo, List<T> *list)
{
    for (; list != 0; list = list->next) {
        (*f)(argOne, argTwo, list->item);
    }
}

// TODO(brendan): testing? changed from pointer member item to primitive
// NOTE(brendan): write all the items in list contiguously to fp
template<typename T> List<T> *
List<T>::readListFromFile(List<T> *list, FILE *fp) 
{
    List<T>::emptyList(&list);
    int count;
    fread(&count, sizeof(int), 1, fp);
    printf("tokens read: %d\n", count);
    for(int i = 0; i < count; ++i) {
        T item;
        fread(&item, sizeof(T), 1, fp);
        list = List<T>::addToList(item, list);
    }
    return list;
}

// NOTE(brendan): write all the items in list contiguously to fp
template<typename T> void
List<T>::writeListToFile(List<T> *list, FILE *fp) 
{
    fseek(fp, sizeof(int), SEEK_CUR);
    int count = 0;
    for(; list != NULL; list = list->next) {
        fwrite(&list->item, sizeof(T), 1, fp);
        ++count;
    }
    printf("tokens written: %d\n", count);
    fseek(fp, -(count*sizeof(T) + sizeof(int)), SEEK_CUR);
    fwrite(&count, sizeof(int), 1, fp);
    fseek(fp, count*sizeof(T), SEEK_CUR);
}

// NOTE(brendan): iterate over list, returning the item of the first node
// that satisfies function f
template<typename T> T *
List<T>::reduceList(bool (*f)(T listItem, T item), T newest, List<T> *list) 
{
    for(; list != NULL; list = list->next) {
        if((*f)(list->item, newest) == true ) {
            return list->item;	
        }
    }
    return NULL;
}

#endif
