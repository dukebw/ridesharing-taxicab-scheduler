SDL_CFLAGS=`sdl2-config --cflags`
SDL_LDFLAGS=`sdl2-config --libs` -lSDL2_image

CFLAGS=$(SDL_CFLAGS) -c -Wall -std=c++11 -g -O3
INCLUDE_FLAGS=-I/usr/include/libxml2
LDFLAGS=$(SDL_LDFLAGS) -lxml2

CC=g++

SOURCES=sdl2_taxicab.cpp edge_weighted_digraph.cpp taxicab.cpp\
				dijkstra_sp.cpp index_min_pq.cpp taxi_searching.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=taxicab_scheduler
# Build directory
BUILD = ../build/

all: directories $(SOURCES) $(BUILD)$(EXECUTABLE)

$(BUILD)$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< $(INCLUDE_FLAGS) -o $@

MKDIR_P = mkdir -p

.PHONY: directories OUT_DIR clean spotless

OUT_DIR = ../build/

directories: OUT_DIR

OUT_DIR:
	$(MKDIR_P) $(OUT_DIR)

clean:
	rm -f *.o

spotless:
	rm -f *.o *~ .*~ $(BUILD)$(EXECUTABLE)

sdl2_taxicab.o: helper.h taxicab.h
taxicab.o: taxicab.h edge_weighted_digraph.h dijkstra_sp.h taxi_searching.h\
	linked_list.h
edge_weighted_digraph.o: edge_weighted_digraph.h helper.h linked_list.h
dijkstra_sp.o: edge_weighted_digraph.h index_min_pq.h linked_list.h helper.h
index_min_pq.o: helper.h
taxi_searching.o: linked_list.h dijkstra_sp.h helper.h edge_weighted_digraph.h
