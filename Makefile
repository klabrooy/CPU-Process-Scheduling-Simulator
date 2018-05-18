#
# Sample Makefile for project 1
#
# The program used for illustration purposes is
# a simple program using prime numbers
#


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS =	-Wall -Wextra -std=gnu99 -g -lm


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		IO.c list.c memory.c
OBJ =		IO.o list.o memory.o
EXE = 		swap

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ)

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE)

## Dependencies
IO.o:	list.h memory.h
list.o:	list.h memory.h
memory.o: list.h memory.h
