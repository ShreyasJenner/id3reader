# Compiler and flags
CC=gcc
CFLAGS=-Wall -Iinclude

# Directories
SRCDIR=src
OBJDIR=obj
LIBDIR=lib
BINDIR=bin

# Files
SRC=$(wildcard $(SRCDIR)/*.c)
OBJ=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
LIB=$(LIBDIR)/id3reader.a
BIN=$(BINDIR)/id3reader

# compile objects 
all: $(BIN)

$(BIN): $(LIB)
	$(CC) example/example.c -o $(BIN) $(OBJ) $(CFLAGS)

$(LIB): $(OBJ) | $(LIBDIR)
	ar -rcs $@ $(OBJ) 

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(BIN)
	rm -f $(LIB)
	rm -rf $(OBJDIR)/*.o


info:
	$(info SRC=$(SRC))

.PHONY: all clean
