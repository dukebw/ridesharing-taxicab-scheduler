#ifndef TAXICAB_H
#define TAXICAB_H

#include "edge_weighted_digraph.h"
#include "taxi_searching.h"
#include "helper.h"
#include <SDL2/SDL.h>

#define NUMBER_OF_IMAGES 5
#define NUMBER_OF_TAXIS 6
#define MAX_NODES 8192
#define MAX_WAYS 128
#define MAX_WAY_NODES 1024

enum {BACKGROUND_TEXTURE, TAXI_TEXTURE, TAXI_FULL_TEXTURE, PICKUP_TEXTURE,
      DROPOFF_TEXTURE};

// NOTE(brendan): for keeping track of pickup/dropoff queries
struct Query {
    bool32 pickup;
    int32 vertex;
};

// NOTE(brendan): for storing data parsed from OSM files
struct Node {
    // NOTE(brendan): x, y displacement from top-left point
    Vector dis;
    uint64 id;
    uint64 wayId;
    // NOTE(brendan): store vertex in graph that the node corresponds to
    int32 vertex;
};

// NOTE(brendan): contains data that each taxi object needs to draw it
// and calculate its schedule
struct Taxi {
    int32 passengerCount;
    int32 queryCount;
    bool32 changePath;
    // NOTE(brendan): rendering
    Vector position;
    Vector velocity;
    // NOTE(brendan): for schedule-updating
    // NOTE(brendan): allocate and free schedules; shortestPaths live
    // statically in taxi_searching
    List<Query> *schedule;
    // NOTE(brendan): pointer to shortest path that the taxi is on
    List<DirectedEdge *> *shortestPath;
    int32 currentVertex;
};

// NOTE(brendan): contains data for insertion points in the query-insertion
// alg.
struct InsertionPoint {
    int32 start;
    int32 end;
    int32 index;
    real32 weight;
};

// NOTE(brendan): struct for storing the current min taxi query to be updated
struct TaxiQuery {
    Taxi *taxi;
    real32 weight;
    int32 pickupIndex;
    int32 dropoffIndex;
};

struct Image {
    int32 width;
    int32 height;
    SDL_Texture *texture;
};

// NOTE(brendan): struct containing all the state for the application
struct TaxiState {
    bool32 graphInitialized;
    int32 screenWidth;
    int32 screenHeight;
    int32 maxPassengerCount;
    int32 maxQueryCount;
    int32 timeSinceLastQuery;
    int32 queryInterval;
    SDL_Renderer *renderer;
    Image images[NUMBER_OF_IMAGES];
    EdgeWeightedDigraph roadNetwork;
    Vector nodeCoords[MAX_WAY_NODES];
    Taxi taxis[NUMBER_OF_TAXIS];
    List<int32> *drawPickups;
    List<int32> *drawDropoffs;
    Node nodes[MAX_NODES];
    int32 nodesCount;
    Vector mapCorners;
    Node wayNodes[MAX_WAY_NODES];
    int32 wayNodesCount;
    int32 uniqueWayNodes;
};

// TODO(brendan): put in update_and_render to avoid cycles in uses diagram
// NOTE(brendan): does updating and rendering
void updateAndRender(TaxiState *taxiState, int32 dt);

#endif /* TAXICAB_H */
