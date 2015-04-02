#ifndef TAXI_SEARCHING_H
#define TAXI_SEARCHING_H

#include "linked_list.h"

// NOTE(brendan): INPUT: source and destination vertices.
// OUTPUT: pointer to shortest path from that source to the destination vertex
List<int> *getShortestPath(int source, int dest);

#endif /* TAXI_SEARCHING_H */
