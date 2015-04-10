/* ========================================================================
   File: taxicab.cpp
   Date: Apr. 5/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   ======================================================================== */

#include "taxicab.h"
#include "dijkstra_sp.h"
#include "stopif.h"
#include <libxml2/libxml/xpath.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SPEED_FACTOR 0.0016f
#define TAXI_QUERY_INTERVAL 1000
#define NO_PATH -1

// TODO(brendan): go through and convert int -> int32 etc.
// -------------------------------------------------------------------------
// Function prototypes (forward declarations)
// ------------------------------------------------------------------------

#if 0
internal void debugPrintList(List<Query> *list);
#endif

inline Vector
mapUnitsToPixels(TaxiState *taxiState, Vector unitsVec);

inline real32
findDistance(Vector vectorOne, Vector vectorTwo);

internal void
updateInsertionPoint(InsertionPoint *insertPoint, List<Query> *schedule,
                     int32 midPoint);

internal void
setMinTaxiQuery(TaxiQuery *minTaxiQuery, Taxi *currentTaxi, 
                InsertionPoint *pickup, InsertionPoint *dropoff);

internal void
initInsertionPoint(InsertionPoint *insertionPoint, int32 start, int32 end,
                   int32 index, real32 weight);

inline int32 randomVertex(EdgeWeightedDigraph *graph);

internal real32
netPathWeight(int32 startPoint32, int32 midPoint, int32 endPoint);

internal int32
getTaxiCurrentVertex(Taxi *taxi, TaxiState *taxiState);

inline void removeTaxiQuery(Taxi *taxi);

internal void
setTaxiVelocity(Taxi *taxi, TaxiState *taxiState);

internal void
initTaxiCab(Taxi *taxi, TaxiState *taxiState, int32 passengerCount,
            bool32 changePath, int32 queryCount, int32 vertex);

inline void
placeImage(SDL_Renderer *renderer, Image *image, Vector point);

internal void
drawListImages(void *pTaxiState, int32 imageIndex, int32 coordIndex);

internal int32 parse(TaxiState *taxiState, char const *infile);

internal int32 compNode(const void *pNodeOne, const void *pNodeTwo);

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------

// NOTE(brendan): does updating and rendering for applications
void updateAndRender(TaxiState *taxiState, int32 dt) 
{
    // NOTE(brendan): Clear screen
    // TODO(brendan): need to set draw colour here?
    SDL_SetRenderDrawColor(taxiState->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(taxiState->renderer);
    SDL_RenderCopy(taxiState->renderer, 
                   taxiState->images[BACKGROUND_TEXTURE].texture, 0, 0);

    // NOTE(brendan): updating
    if (taxiState->graphInitialized) {
        // NOTE(brendan): deal with queries; make a new query 
        // every queryInterval
        taxiState->timeSinceLastQuery += dt;
        if (taxiState->timeSinceLastQuery > taxiState->queryInterval) {
            taxiState->timeSinceLastQuery = 0;
            int32 pickupVertex = randomVertex(&taxiState->roadNetwork);
            int32 dropoffVertex = randomVertex(&taxiState->roadNetwork);

            // NOTE(brendan): we iterate through the taxi's schedule and 
            // try to find the cheapest of all combinations for insert points 
            // for the pickup and dropoff of the query
            Taxi *currentTaxi = 0;
            TaxiQuery minTaxiQuery = {};
            minTaxiQuery.weight = INFINITY;
            InsertionPoint pickup = {};
            InsertionPoint dropoff = {};
            for (int32 taxiIndex = 0; 
                 taxiIndex < NUMBER_OF_TAXIS; 
                 ++taxiIndex) {
                currentTaxi = &taxiState->taxis[taxiIndex];
                if ((currentTaxi->passengerCount < 
                     taxiState->maxPassengerCount) &&
                    (currentTaxi->queryCount < taxiState->maxQueryCount)) {
                    int32 currentTaxiVertex = getTaxiCurrentVertex(currentTaxi, 
                                                                   taxiState);
                    int32 firstPickupEnd = NO_PATH;
                    if (currentTaxi->schedule) {
                        firstPickupEnd = currentTaxi->schedule->item.vertex;
                    }
                    real32 firstPickupWeight = 
                        netPathWeight(currentTaxiVertex, pickupVertex, 
                                      firstPickupEnd);
                    initInsertionPoint(&pickup, currentTaxiVertex, 
                                       firstPickupEnd, 0, firstPickupWeight);
                    for (List<Query> *pPickup = currentTaxi->schedule;
                         pPickup;
                         pPickup = pPickup->next) {
                        Query firstDropoffEnd;
                        if (!List<Query>::itemAt(&firstDropoffEnd, 
                                                 currentTaxi->schedule,
                                                 pickup.index)) {
                            firstDropoffEnd.vertex = NO_PATH;
                        }
                        real32 firstDropoffWeight = 
                            netPathWeight(pickupVertex, dropoffVertex, 
                                          firstDropoffEnd.vertex);
                        initInsertionPoint(&dropoff, pickupVertex, 
                                           firstDropoffEnd.vertex,
                                           pickup.index, firstDropoffWeight);
                        setMinTaxiQuery(&minTaxiQuery, currentTaxi, &pickup, 
                                        &dropoff);
                        for (List<Query> *pDropoff = pPickup;
                             pDropoff;
                             pDropoff = pDropoff->next) {
                            updateInsertionPoint(&dropoff, pDropoff, 
                                                 dropoffVertex);
                            setMinTaxiQuery(&minTaxiQuery, currentTaxi, 
                                            &pickup, &dropoff);
                        }
                        updateInsertionPoint(&pickup, pPickup, pickupVertex);
                    }
                    // NOTE(brendan): dropoff will go after pickup if both are
                    // at the end of the current schedule (before insertion)
                    dropoff.weight = 
                        getShortestPath(pickupVertex, 
                                        dropoffVertex)->totalWeight;
                    dropoff.start = pickupVertex;
                    setMinTaxiQuery(&minTaxiQuery, currentTaxi, &pickup, 
                                    &dropoff);
                }
            }
            if (minTaxiQuery.taxi) {
                ++minTaxiQuery.taxi->queryCount;
                minTaxiQuery.taxi->schedule = 
                    List<Query>::insertAt(minTaxiQuery.taxi->schedule, 
                                          {true, pickupVertex}, 
                                          minTaxiQuery.pickupIndex);
                minTaxiQuery.taxi->schedule = 
                    List<Query>::insertAt(minTaxiQuery.taxi->schedule, 
                                          {false, dropoffVertex}, 
                                          minTaxiQuery.dropoffIndex + 1);
                // NOTE(brendan): reset shortestPath based on schedule
                minTaxiQuery.taxi->changePath = true;
                // NOTE(brendan): add images to to-be-drawn lists
                taxiState->drawPickups = 
                    List<int32>::addToList(pickupVertex, 
                                           taxiState->drawPickups);
                taxiState->drawDropoffs =
                    List<int32>::addToList(dropoffVertex, 
                                           taxiState->drawDropoffs);
            }
        }

        // NOTE(brendan): update taxi positions, velocities, schedules and paths
        for (int32 taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
            Taxi *currentTaxi = &taxiState->taxis[taxiIndex];
            if (currentTaxi->shortestPath) {
                DirectedEdge *currentEdge = currentTaxi->shortestPath->item;
                Vector destination = taxiState->nodeCoords[currentEdge->to];
                real32 deltaY = destination.y - currentTaxi->position.y;
                real32 deltaX = destination.x - currentTaxi->position.x;

                // NOTE(brendan): taxi reaches the destination on this physics
                // update
                real32 vy = currentTaxi->velocity.y;
                real32 vx = currentTaxi->velocity.x;
                real32 timeToDestX = (vx != 0.0f) ? deltaX/vx : 0.0f;
                real32 timeToDestY = (vy != 0.0f) ? deltaY/vy : 0.0f;
                if ((timeToDestX < dt) && (timeToDestY < dt)) {
                    // NOTE(brendan): set taxi's position to its destination
                    currentTaxi->position.x = destination.x;
                    currentTaxi->position.y = destination.y;
                    currentTaxi->currentVertex = currentEdge->to;
                    // NOTE(brendan): reached a scheduled vertex
                    if (currentTaxi->changePath) {
                        currentTaxi->changePath = false;
                        currentTaxi->shortestPath = 0;
                    }
                    else {
                        currentTaxi->shortestPath = 
                            currentTaxi->shortestPath->next;
                    }
                    setTaxiVelocity(currentTaxi, taxiState);
                }

                currentTaxi->position.x += currentTaxi->velocity.x*dt;
                currentTaxi->position.y += currentTaxi->velocity.y*dt;
            }
            // NOTE(brendan): get the next dropoff or pickup spot in the
            // schedule and discard last one
            else {
                initTaxiCab(currentTaxi, taxiState, 
                            currentTaxi->passengerCount, false, 
                            currentTaxi->queryCount, 
                            currentTaxi->currentVertex);
            }
            // NOTE(brendan): do drawing of all images
            Image *taxiImage;
            if (currentTaxi->passengerCount == taxiState->maxPassengerCount) {
                taxiImage = &taxiState->images[TAXI_FULL_TEXTURE];
            }
            else {
                taxiImage = &taxiState->images[TAXI_TEXTURE];
            }
            Vector offsetPosition = currentTaxi->position;
            // TODO(brendan): x offset adjusts for bad map data; fix long-lat to
            // distance calculation using haversince function?
            offsetPosition.x -= 0.5f*taxiImage->width;
            offsetPosition.y -= taxiImage->height;
            placeImage(taxiState->renderer, taxiImage, offsetPosition);
        }
        // NOTE(brendan): draw the pickup and dropoff query bitmaps
        List<int32>::traverseList(drawListImages, taxiState, PICKUP_TEXTURE,
                                  taxiState->drawPickups);
        List<int32>::traverseList(drawListImages, taxiState, DROPOFF_TEXTURE,
                                  taxiState->drawDropoffs);

#if 1
        // TODO(brendan): remove; testing
        // NOTE(brendan): draw red lines on all the edges of the graph
        // (can see road network)
        SDL_SetRenderDrawColor(taxiState->renderer, 0xFF, 0, 0, 0xFF);
        Vector prevPixels = {};
        for (int32 nodeIndex = 0;
             nodeIndex < taxiState->wayNodesCount; 
             ++nodeIndex) {
            // NOTE(brendan): draw line between nodes on the "way"
            Vector nodePixels = 
                mapUnitsToPixels(taxiState, taxiState->wayNodes[nodeIndex].dis);
            if ((nodeIndex > 0) &&
                (taxiState->wayNodes[nodeIndex].wayId == 
                 taxiState->wayNodes[nodeIndex - 1].wayId)) {
                SDL_RenderDrawLine(taxiState->renderer, (int32)nodePixels.x, 
                                   (int32)nodePixels.y, (int32)prevPixels.x, 
                                   (int32)prevPixels.y);
            }
            prevPixels = nodePixels;
        }
#endif
    }
    else {
        // TODO(brendan): rename this
        parse(taxiState, "../src/yonge_sheppard_map.osm");

        // NOTE(brendan): do initialization of graph and taxiState
        taxiState->graphInitialized = true;
        taxiState->queryInterval = TAXI_QUERY_INTERVAL;
        taxiState->timeSinceLastQuery = 0;
        taxiState->maxPassengerCount = 3;
        taxiState->maxQueryCount = 3;
        makeEdgeWeightedDigraph(&taxiState->roadNetwork, 
                                taxiState->uniqueWayNodes);
        taxiState->nodeCoords = 
            (Vector *)malloc(taxiState->uniqueWayNodes*sizeof(Vector));

        // NOTE(brendan): initialize edges in graph
        for (int32 nodeIndex = 0;
             nodeIndex < taxiState->wayNodesCount; 
             ++nodeIndex) {
            Node *currentNode = &taxiState->wayNodes[nodeIndex];
            if (nodeIndex > 0) {
                Node *previousNode = &taxiState->wayNodes[nodeIndex - 1];
                if (currentNode->wayId == previousNode->wayId) {
                    // NOTE(brendan): add two-way street edge
                    real32 distance = findDistance(currentNode->dis,
                                                   previousNode->dis);
                    addEdge(&taxiState->roadNetwork, currentNode->vertex,
                            previousNode->vertex, distance);
                    addEdge(&taxiState->roadNetwork, previousNode->vertex,
                            currentNode->vertex, distance);
                }
            }
            taxiState->nodeCoords[currentNode->vertex] =
                mapUnitsToPixels(taxiState, taxiState->wayNodes[nodeIndex].dis);
        }

        makeAllShortestPaths(&taxiState->roadNetwork);

        // NOTE(brendan): init taxis
        for (int32 taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
            int32 taxiStartVertex = randomVertex(&taxiState->roadNetwork);
            initTaxiCab(&taxiState->taxis[taxiIndex], taxiState, 0, false, 0,
                        taxiStartVertex);
            taxiState->taxis[taxiIndex].schedule = 0;
        }
    }

    // NOTE(brendan): paint on the canvas
    SDL_RenderPresent(taxiState->renderer);
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------

inline Vector
mapUnitsToPixels(TaxiState *taxiState, Vector unitsVec)
{
    Vector result;
    result.x = (real32)taxiState->screenWidth*
               unitsVec.x/taxiState->mapCorners.x;
    result.y = (real32)taxiState->screenHeight*
               unitsVec.y/taxiState->mapCorners.y;
    return result;
}

// NOTE(brendan): INPUT: (x1, y1) and (x2, y2). OUTPUT: real32 distance between
// the two points
inline real32
findDistance(Vector vectorOne, Vector vectorTwo)
{
    real32 deltaX = vectorTwo.x - vectorOne.x;
    real32 deltaY = vectorTwo.y - vectorOne.y;
    real32 result = sqrt(deltaX*deltaX + deltaY*deltaY);
    return result;
}

#if 0
// TODO(brendan): testing; remove
internal void
debugPrintList(List<Query> *list)
{
    for (; list; list = list->next) {
        printf(list->next ? "%d -> " : "%d\n", list->item.vertex);
    }
}
#endif

// NOTE(brendan): INPUT: min taxi-query, taxi to set, pickup and
// dropoff insertion points. OUTPUT: none. UPDATE: minTaxiQuery is
// conditionally updated if the given insertion points are cheaper
internal void
setMinTaxiQuery(TaxiQuery *minTaxiQuery, Taxi *currentTaxi, 
                InsertionPoint *pickup, InsertionPoint *dropoff)
{
    // TODO(brendan): assert inputs != 0
    real32 insertWeight = pickup->weight + dropoff->weight;
    if (insertWeight < minTaxiQuery->weight) {
        minTaxiQuery->taxi = currentTaxi;
        minTaxiQuery->weight = insertWeight;
        minTaxiQuery->pickupIndex = pickup->index;
        minTaxiQuery->dropoffIndex = dropoff->index;
    }
}

// NOTE(brendan): INPUT: insertion-point, schedule, mid-point vertex
// OUTPUT: none. UPDATE: insertion point is updated; this is done once
// every loop iteration in the query-insertion alg.
internal void
updateInsertionPoint(InsertionPoint *insertPoint, List<Query> *schedule,
                     int32 midPoint)
{
    int nextEndPoint = schedule->next ? schedule->next->item.vertex : NO_PATH;
    real32 nextPointWeight =
        netPathWeight(schedule->item.vertex, midPoint, nextEndPoint);
    initInsertionPoint(insertPoint, schedule->item.vertex, nextEndPoint,
                       insertPoint->index + 1, nextPointWeight);
}

// NOTE(brendan): INPUT: insertion-point, start and end points, index and
// weight. OUTPUT: none. UPDATE: the insertion-point; it is initialized
internal void
initInsertionPoint(InsertionPoint *insertPoint, int32 start, int32 end,
                   int32 index, real32 weight)
{
    // TODO(brendan): assert insertPoint != 0
    insertPoint->start = start;
    insertPoint->end = end;
    insertPoint->index = index;
    insertPoint->weight = weight;
}

// NOTE(brendan): INPUT: a start point, a mid point and an end point.
// OUTPUT: the net weight from replacing start->end to start->mid->end
internal real32
netPathWeight(int32 startPoint, int32 midPoint, int32 endPoint)
{
    if (startPoint == NO_PATH) {
        return getShortestPath(midPoint, endPoint)->totalWeight;
    }
    if (endPoint == NO_PATH) {
        return getShortestPath(startPoint, midPoint)->totalWeight;
    }
    real32 taxiAddedWeight = getShortestPath(startPoint, midPoint)->totalWeight;
    taxiAddedWeight += getShortestPath(midPoint, endPoint)->totalWeight;
    return taxiAddedWeight - getShortestPath(startPoint, endPoint)->totalWeight;
}

inline int32 randomVertex(EdgeWeightedDigraph *graph)
{
    return (int32)(((real64)rand()/((real64)RAND_MAX + 1.0))*
                 (real64)graph->vertices);
}

// NOTE(brendan): INPUT: taxi, taxistate. OUTPUT: none. UPDATE: taxi; taxi's
// velocity is set based on the next edge in its shortestPath. If its 
// shortestPath has been traversed, its shortestPath is set to 0
// and its velocity is set to 0
internal void
setTaxiVelocity(Taxi *taxi, TaxiState *taxiState)
{
    Stopif((taxi == 0) || (taxiState == 0), ,
           "Error: null taxi or taxiState passed to setTaxiVelocity\n");
    if (taxi->shortestPath) {
        DirectedEdge *currentEdge = taxi->shortestPath->item;
        // TODO(brendan): generate random speeds for the roads
        local_persist real32 speed = 50.0f;
        Vector node = taxiState->nodeCoords[currentEdge->to];
        real32 distance = findDistance(taxi->position, node);
        if (distance) {
            real32 deltaX = node.x - taxi->position.x;
            real32 deltaY = node.y - taxi->position.y;
            taxi->velocity.x = SPEED_FACTOR*speed*deltaX/distance;
            taxi->velocity.y = SPEED_FACTOR*speed*deltaY/distance;
        }
        else {
            taxi->velocity.x = 0.0f;
            taxi->velocity.y = 0.0f;
        }
    }
    else {
        taxi->velocity.x = 0.0f;
        taxi->velocity.y = 0.0f;
    }
}

// TODO(brendan): change/remove?
// INPUT: taxi, taxi-state. OUTPUT: the vertex that that taxi is currently
// at, according to its x and y position
internal int32
getTaxiCurrentVertex(Taxi *taxi, TaxiState *taxiState)
{
    return taxi->currentVertex;
}

// NOTE(brendan): INPUT: taxi. OUTPUT: none. UPDATE: changes taxi's number
// of passengers
inline void
removeTaxiQuery(Taxi *taxi, TaxiState *taxiState)
{
    if (taxi->schedule->item.pickup) {
        ++taxi->passengerCount;
        taxiState->drawPickups = 
            List<int32>::deleteFromList(taxi->schedule->item.vertex, 
                                        taxiState->drawPickups);
    }
    else {
        --taxi->passengerCount;
        --taxi->queryCount;
        taxiState->drawDropoffs = 
            List<int32>::deleteFromList(taxi->schedule->item.vertex,
                                        taxiState->drawDropoffs);
    }
    taxi->schedule = List<Query>::removeHead(taxi->schedule);
}

// INPUT: taxi-state, taxi, schedule. OUTPUT: none. UPDATE: the taxi is
// updated; its shortestPath is set based on its schedule, and its
// position and velocity are set based on its shortestPath
internal void
initTaxiCab(Taxi *taxi, TaxiState *taxiState, int32 passengerCount,
            bool32 changePath, int32 queryCount, int32 vertex)
{
    Stopif((taxi == 0) || (taxiState == 0), ,
           "Error: null taxi or taxiState passed to initTaxiCab\n");
    taxi->passengerCount = passengerCount;
    taxi->queryCount = queryCount;
    taxi->changePath = changePath;
    taxi->position = taxiState->nodeCoords[vertex];
    taxi->currentVertex = vertex;
    if (taxi->schedule) {
        int32 taxiCurrentVertex = getTaxiCurrentVertex(taxi, taxiState);
        if (taxiCurrentVertex == taxi->schedule->item.vertex) {
            removeTaxiQuery(taxi, taxiState);
        }
        else {
            int32 nextScheduled = taxi->schedule->item.vertex;
            taxi->shortestPath =
                getShortestPath(taxiCurrentVertex, nextScheduled)->edgeList;
        }
        setTaxiVelocity(taxi, taxiState);
    }
    else {
        taxi->velocity.x = 0.0f;
        taxi->velocity.y = 0.0f;
        taxi->shortestPath = 0;
    }
}

// NOTE(brendan): calculates the distance between two (longitude, latitude)
// pairs in km
// source for formula: http://www.movable-type.co.uk/scripts/latlong.html
// INPUT: longitude/latitude in degrees
// OUTPUT: Approximation to aerial distance between points, in km
internal Vector
findDisplacement(real64 startLongitude, real64 startLatitude,
                 real64 endLongitude, real64 endLatitude) 
{
    local_persist real64 earthRadius = 6371.0;
    local_persist real64 degreesToRadians = M_PI/180.0; 
    // NOTE(brendan): convert to radians
    startLatitude = startLatitude*degreesToRadians;
    startLongitude = startLongitude*degreesToRadians;
    endLatitude = endLatitude*degreesToRadians;
    endLongitude = endLongitude*degreesToRadians;
    real64 meanLatitude = (startLatitude + endLatitude)/2.0;
    Vector result;
    result.x = (real32)(earthRadius*
                        (endLongitude - startLongitude)*cos(meanLatitude));
    result.y = (real32)(earthRadius*(endLatitude - startLatitude));
    return result;
}

// NOTE(brendan): INPUT: void pointers to Nodes. OUTPUT: -1 if nodeOne's
// id is less than nodeTwo's, 0 if the id's are equal and +1 if nodeOne's
// id is greater than nodeTwo's
internal int32 
compNode(const void *pNodeOne, const void *pNodeTwo)
{
    uint64 nodeOneId = ((Node *)pNodeOne)->id;
    uint64 nodeTwoId = ((Node *)pNodeTwo)->id;
    if (nodeOneId < nodeTwoId) {
        return -1;
    }
    else if (nodeOneId > nodeTwoId) {
        return 1;
    }
    else {
        return 0;
    }
}

// -------------------------------------------------------------------------
// SDL functions
// ------------------------------------------------------------------------

// NOTE(brendan): places the image at point
inline void
placeImage(SDL_Renderer *renderer, Image *image, Vector point) 
{
    SDL_Rect destRect;
    destRect.x = (int32)point.x;
    destRect.y = (int32)point.y;
    destRect.w = image->width;
    destRect.h = image->height;
    SDL_RenderCopy(renderer, image->texture, NULL, &destRect ); 
}

// NOTE(brendan): INPUT: taxi state, image index, point. OUTPUT: none.
// UPDATE: places the image at point on the renderer. For use with
// traverseList
internal void
drawListImages(void *pTaxiState, int32 imageIndex, int32 coordIndex)
{
    TaxiState *taxiState = (TaxiState *)pTaxiState;
    Vector offsetCoords = taxiState->nodeCoords[coordIndex];
    offsetCoords.x -= 0.5f*taxiState->images[imageIndex].width;
    offsetCoords.y -= taxiState->images[imageIndex].height;
    placeImage(taxiState->renderer, &taxiState->images[imageIndex],
               offsetCoords);
}

// -------------------------------------------------------------------------
// XML parsing functions
// ------------------------------------------------------------------------

// NOTE(brendan): INPUT: xmlDocPtr and xmlXPathObjecPtr to "way" nodes in
// OpenStreetMap file. OUTPUT: none. UPDATE: none.
internal void
parseWays(TaxiState *taxiState, xmlDocPtr doc, xmlXPathObjectPtr ways)
{
    xmlNodeSetPtr waysNodeSet = ways->nodesetval;
    for (int32 wayIndex = 0; wayIndex < waysNodeSet->nodeNr; ++wayIndex) {
        // NOTE(brendan): narrowing down to refs on streets
        bool32 validHighway = false;
        xmlNodePtr pWay = waysNodeSet->nodeTab[wayIndex];
        for (xmlNodePtr pWayChild = pWay->children;
             pWayChild;
             pWayChild = pWayChild->next) {
            if (xmlStrcmp(pWayChild->name, (xmlChar *)"tag") == 0) {
                xmlChar *tagKey = xmlGetProp(pWayChild, (xmlChar *)"k");
                if (xmlStrcmp(tagKey, (xmlChar *)"highway") == 0) {
                    xmlChar *tagVal = xmlGetProp(pWayChild, (xmlChar *)"v");
                    if ((xmlStrcmp(tagVal, (xmlChar *)"residential") == 0) ||
                        (xmlStrcmp(tagVal, (xmlChar *)"primary") == 0) ||
                        (xmlStrcmp(tagVal, (xmlChar *)"secondary") == 0) ||
                        (xmlStrcmp(tagVal, (xmlChar *)"tertiary") == 0)) {
                        validHighway = true;
                        break;
                    }
                }
            }
        }
        if (validHighway) {
            const char *wayId = 
                (const char *)xmlGetProp(pWay, (xmlChar *)"id");
            for (xmlNodePtr pWayChild = pWay->children;
                 pWayChild;
                 pWayChild = pWayChild->next) {
                // NOTE(brendan): getting the (x, y) data from the
                // reference
                if (xmlStrcmp(pWayChild->name, (xmlChar *)"nd") == 0) {
                    const char *ref = 
                        (const char *)xmlGetProp(pWayChild, (xmlChar *)"ref");
                    Node myRefNode = {};
                    myRefNode.id = strtoull(ref, 0, 10);
                    Node *pNode =
                        (Node *)bsearch((void *)&myRefNode, 
                                        (const void *)taxiState->nodes, 
                                        taxiState->nodesCount, sizeof(Node), 
                                        compNode);
                    // NOTE(brendan): only take those nodes that were in our
                    // bounding box; only count those that haven't been added 
                    // yet
                    if (pNode) {
                        uint64 wayIdVal = strtoull(wayId, 0, 10);
                        if (pNode->wayId == 0) {
                            pNode->wayId = wayIdVal;
                            pNode->vertex = taxiState->uniqueWayNodes;
                            ++taxiState->uniqueWayNodes;
                        }
                        taxiState->wayNodes[taxiState->wayNodesCount].dis = 
                            pNode->dis;
                        taxiState->wayNodes[taxiState->wayNodesCount].id = 
                            pNode->id;
                        taxiState->wayNodes[taxiState->wayNodesCount].wayId = 
                            wayIdVal;
                        taxiState->wayNodes[taxiState->wayNodesCount].vertex = 
                            pNode->vertex;
                        ++taxiState->wayNodesCount;
                    }
                }
            }
        }
    }
    // NOTE(brendan): no longer need taxiState->nodes; free it
    free(taxiState->nodes);
    taxiState->nodes = 0;
    printf("uniqueWayNOdes: %d\n", taxiState->uniqueWayNodes);
    printf("wayNodesCount: %d\n", taxiState->wayNodesCount);
}

// NOTE(brendan): INPUT: TaxiState, xmlDocPtr and xmlXPathObjectPtr (nodes).
// parses all the ids of nodes and puts them in an array, sorted by id and 
// with values being (long, lat). UPDATE: taxiState; initializes nodes array
internal void
parseNodes(TaxiState *taxiState, xmlDocPtr doc, xmlXPathObjectPtr nodes)
{
    // TODO(brendan): could retrieve these from osm file
    // NOTE(brendan): define image boundaries
    local_persist real64 minLon = -79.420;
    local_persist real64 maxLat = 43.787;
    local_persist real64 maxLon = -79.376;
    local_persist real64 minLat = 43.763;

    taxiState->mapCorners = findDisplacement(minLon, maxLat, 
                                             maxLon, minLat);

    xmlNodeSetPtr nodeset = nodes->nodesetval;
    for (int32 nodeIndex = 0; nodeIndex < nodeset->nodeNr; ++nodeIndex) {
        const char *id = 
            (const char *)xmlGetProp(nodeset->nodeTab[nodeIndex], 
                                     (xmlChar *)"id");
        const char *longitude = 
            (const char *)xmlGetProp(nodeset->nodeTab[nodeIndex], 
                                     (xmlChar *)"lon");
        const char *latitude = 
            (const char *)xmlGetProp(nodeset->nodeTab[nodeIndex], 
                                     (xmlChar *)"lat");
        real64 nodeLongitude = strtod(longitude, 0);
        real64 nodeLatitude = strtod(latitude, 0);
        // NOTE(brendan): only accept nodes that are within our bounding
        // box
        if (nodeLongitude >= minLon && nodeLongitude <= maxLon &&
            nodeLatitude >= minLat && nodeLatitude <= maxLat) {
            taxiState->nodes[nodeIndex].dis = 
                findDisplacement(minLon, maxLat,
                                 nodeLongitude, nodeLatitude);
            taxiState->nodes[nodeIndex].id = strtoull(id, 0, 10);
            // TODO(brendan): remove; debugging
            ++taxiState->nodesCount;
        }
    }
    printf("Nodes total count: %d\n", nodes->nodesetval->nodeNr);
    qsort(taxiState->nodes, taxiState->nodesCount, sizeof(Node), compNode);
}

// NOTE(brendan): INPUT: name of file. OUTPUT: none. Parses XML, then finds
// all nodes matching XPath for title elements and all nodes matching XPath
// for the links. Source: 21st Century C by Klemens
internal int32 
parse(TaxiState *taxiState, char const *infile)
{
    xmlDocPtr doc = xmlParseFile(infile);
    Stopif(!doc, return -1, "Error: unable to parse file \"%s\"\n", infile);

    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    Stopif(!context, return -2, "Error: unable to create new XPath context\n");

    const xmlChar *nodespath = (xmlChar *)"//node";
    xmlXPathObjectPtr nodes = xmlXPathEvalExpression(nodespath, context);
    Stopif(!nodes, return -3, "Xpath '//node failed.");

    const xmlChar *waypath = (xmlChar *)"//way";
    xmlXPathObjectPtr ways = xmlXPathEvalExpression(waypath, context);
    Stopif(!ways, return -3, "Xpath '//way/nd failed.");

    parseNodes(taxiState, doc, nodes);

    parseWays(taxiState, doc, ways);

    xmlXPathFreeContext(context);
    xmlXPathFreeObject(ways);
    xmlXPathFreeObject(nodes);
    // NOTE(brendan): can comment this out and let the OS free to load faster
    xmlFreeDoc(doc);
    return 0;
}
