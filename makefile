TARGET = ml

INCLUDE = ./include
DIST = ./dist
SRC = ./src

FLAGS = -I$(INCLUDE) -Wall
LIBS = -lncurses -lcollections
CC = gcc

OBJECTS = $(patsubst $(SRC)/%.c,$(DIST)/%.o,$(wildcard $(SRC)/*.c))

all: $(DIST)/$(TARGET)

.PHONY: clean

$(DIST):
	mkdir -p $@

$(DIST)/%.o: $(SRC)/%.c | $(DIST)
	$(CC) -c -o $@ $< $(FLAGS)

$(DIST)/$(TARGET): $(OBJECTS) | $(DIST)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm -rf $(DIST)
