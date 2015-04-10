#ifndef HELPER_H
#define HELPER_H

// NOTE(brendan): to change static so that we can distinguish between
// global variables etc. then we can e.g. search and fix global vars
#define internal static
#define local_persist static
#define global_variable static

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef float real32;
typedef double real64;

struct Vector {
    real32 x;
    real32 y;
};

struct Rectangle {
    Vector topLeft;
    Vector bottomRight;
};

struct Dimensions {
    int32 width; 
    int32 height;
};

#define MAX(x,y) ((x) < (y) ? (y) : (x))

#endif /* HELPER_H */
