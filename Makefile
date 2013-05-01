CC = g++
CFLAGS = -O3 -funroll-loops -c -Wall -I include
LDFLAGS = -lm

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(addprefix bin/,$(notdir $(SOURCES:.cpp=.o)))
EXECUTABLE = bin/MyBot

all: bin_dir clean $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

bin/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $^

clean: 
	rm -f bin/*.o $(EXECUTABLE)

bin_dir:
	mkdir -p bin

.PHONY: all clean
