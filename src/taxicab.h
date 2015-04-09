#ifndef TAXICAB_H
#define TAXICAB_H

#include "edge_weighted_digraph.h"
#include "taxi_searching.h"
#include "helper.h"
#include <SDL2/SDL.h>

#define NUMBER_OF_IMAGES 5
#define NUMBER_OF_TAXIS 6
#define MAX_NODES 8192

enum {BACKGROUND_TEXTURE, TAXI_TEXTURE, TAXI_FULL_TEXTURE, PICKUP_TEXTURE,
      DROPOFF_TEXTURE};

// NOTE(brendan): for keeping track of pickup/dropoff queries
struct Query {
    bool pickup;
    int vertex;
};

// NOTE(brendan): for storing data parsed from OSM files
struct Node {
    // NOTE(brendan): x, y displacement from top-left point
    VecDouble dis;
    long id;
};

// NOTE(brendan): contains data that each taxi object needs to draw it
// and calculate its schedule
struct Taxi {
    int passengerCount;
    int queryCount;
    bool changePath;
    // NOTE(brendan): rendering
    Point position;
    Vector velocity;
    // NOTE(brendan): for schedule-updating
    // NOTE(brendan): allocate and free schedules; shortestPaths live
    // statically in taxi_searching
    List<Query> *schedule;
    // NOTE(brendan): pointer to shortest path that the taxi is on
    List<DirectedEdge *> *shortestPath;
};

// NOTE(brendan): contains data for insertion points in the query-insertion
// alg.
struct InsertionPoint {
    int start;
    int end;
    int index;
    float weight;
};

// NOTE(brendan): struct for storing the current min taxi query to be updated
struct TaxiQuery {
    Taxi *taxi;
    float weight;
    int pickupIndex;
    int dropoffIndex;
};

struct Image {
    int width;
    int height;
    SDL_Texture *texture;
};

// NOTE(brendan): struct containing all the state for the application
struct TaxiState {
    bool graphInitialized;
    int screenWidth;
    int screenHeight;
    int maxPassengerCount;
    int maxQueryCount;
    int timeSinceLastQuery;
    int queryInterval;
    SDL_Renderer *renderer;
    Image images[NUMBER_OF_IMAGES];
    EdgeWeightedDigraph roadNetwork;
    Point intersectionCoords[INTERSECTIONS];
    Taxi taxis[NUMBER_OF_TAXIS];
    List<int> *drawPickups;
    List<int> *drawDropoffs;
    Node nodes[MAX_NODES];
    int nodesCount;
    Node wayNodes[MAX_NODES];
    int wayNodesCount;
    VecDouble mapCorners;
};

// TODO(brendan): put in update_and_render to avoid cycles in uses diagram
// NOTE(brendan): does updating and rendering
void updateAndRender(TaxiState *taxiState, int dt);

#endif /* TAXICAB_H */
