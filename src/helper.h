#ifndef HELPER_H
#define HELPER_H

// NOTE(brendan): to change static so that we can distinguish between
// global variables etc. then we can e.g. search and fix global vars
#define internal static
#define local_persist static
#define global_variable static

struct Point {
  int x;
  int y;
};

struct Rectangle {
  Point topLeft;
  Point bottomRight;
};

struct Circle {
  Point center;
  int radius;
};

#define MAX(x,y) ((x) < (y) ? (y) : (x))

#endif /* HELPER_H */