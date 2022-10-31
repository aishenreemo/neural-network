TARGET := ml

INCLUDE := include
DIST := dist
SRC := src

FLAGS := -I$(INCLUDE) -Wall -O3 -march=native
LIBS := -lSDL2 -lcollections
CC := gcc

DIST_DIRS := $(patsubst $(SRC)/%,$(DIST)/%,$(shell find $(SRC) -type d -links 2))
OBJECTS := $(patsubst $(SRC)/%.c,$(DIST)/%.o,$(shell find $(SRC) -type f -iname *.c))

all: $(DIST_DIRS) $(DIST)/$(TARGET)

.PHONY: clean

$(DIST_DIRS):
	mkdir -p $@

$(DIST)/%.o: $(SRC)/%.c | $(DIST_DIRS)
	$(CC) -c -o $@ $< $(FLAGS)

$(DIST)/$(TARGET): $(OBJECTS) | $(DIST_DIRS)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm -rf $(DIST)

run:
	@$(DIST)/$(TARGET)
