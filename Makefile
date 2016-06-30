# Debug level 1 use CFLAGS=-DDEBUG1
# Debug level 2 use CFLAGS=-DDEBUG2
# Debug level 3 use CFLAGS=-DDEBUG3


CC=g++ 
CCFLAGS=-c -Wall -O3 -pg -std=c++11
LDFLAGS=-lm -pg
RMFLAGS=-f
OBJ-SOURCES=usp.c permutation.c puzzle.c set.c
EXE-SOURCES=usp_tester.c permutation_tester.c puzzle_tester.c set_tester.c
OBJDIR=objs
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ-SOURCES:.c=.o))
EXES=$(EXE-SOURCES:.c= )

#EXECUTABLE=hello

#all: $(OBJS) $(EXECUTABLES)

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $(CCFLAGS) $< -o $@

% : %.c $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $@.c -o $@ 

all: $(OBJECTS) $(EXES)

#$(EXECUTABLE): $(OBJECTS) 
#    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

#.cpp.o:
#    $(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(RMFLAGS) $(OBJDIR)/*.o $(EXES) *~ *.out
