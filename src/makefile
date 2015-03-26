SDL_CFLAGS=`sdl2-config --cflags`
SDL_LDFLAGS=`sdl2-config --libs`

CFLAGS=$(SDL_CFLAGS) -c -Wall -std=c++11
LDFLAGS=$(SDL_LDFLAGS)

CC=g++

SOURCES=sdl2_taxicab.cpp edge_weighted_digraph.cpp taxicab.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=taxicab_scheduler
# Build directory
BUILD = ../build/

all: directories $(SOURCES) $(BUILD)$(EXECUTABLE)

$(BUILD)$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

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
taxicab.o: taxicab.h edge_weighted_digraph.h
edge_weighted_digraph.o: edge_weighted_digraph.h helper.h linked_list.h
