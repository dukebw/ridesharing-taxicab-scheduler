#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct List {
  int item;
  List *next;
};

// NOTE(brendan): add item T to the list
List *
addToList(List *list, int newItem);

// NOTE(brendan): free all the nodes of the list
// client must set list to 0
void emptyList(List *list);

#endif /* LINKED_LIST_H */
