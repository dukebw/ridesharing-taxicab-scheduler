#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <stdio.h>

template<typename T>
class List {
// TODO(brendan): change back to private
public:
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

  // NOTE(brendan): delete the first occurrence of item T; returns the list
  static List<T> *
  deleteFromList(T toDeleteItem, List<T> *list);

  // NOTE(brendan): iterate over list, executing function f() on each node
  static void 
  traverseList(void (*f)(T), List<T> *list);

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

  // NOTE(brendan): iterate over list, returning the item of the first node
  // that satisfies function f
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
  if(list != NULL) {
    List<T> *current;
    List<T> *previous;
    for(current = list, previous = NULL; 
        (current != NULL) && (current->item != toDeleteItem); 
        previous = current, current = current->next);
    if(current != NULL) {
      if(previous != NULL) {
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
    return NULL;
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