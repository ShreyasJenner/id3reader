# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude

# Directories
SRCDIR = src
OBJDIR = obj
LIBDIR = lib

# Files
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
LIB = $(LIBDIR)/id3reader.a

# compile objects 
all: $(LIB)

$(LIB): $(OBJ)
	ar -rcs $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)/*.o
	rm $(LIBDIR)/$(LIB)

.PHONY: all clean
