# Compiler and flags
CC=gcc
CFLAGS=-Wall -Iinclude -lFLAC

# Directories
SRCDIR=src
OBJDIR=obj
LIBDIR=lib
BINDIR=bin

# Files
SRC=$(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJ=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
LIB=$(LIBDIR)/metadata-reader.a
BIN=$(BINDIR)/metadata-reader

# compile objects 
all: $(BIN)

$(BIN): $(LIB)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS)

$(LIB): $(OBJ) | $(LIBDIR)
	ar -rcs $@ $(OBJ) 

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(BINDIR)/*
	rm -f $(LIBDIR)/*
	rm -rf $(OBJDIR)/**/*.o


info:
	$(info SRC=$(SRC))

.PHONY: all clean
