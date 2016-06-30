# Debug level 1 use CFLAGS=-DDEBUG1
# Debug level 2 use CFLAGS=-DDEBUG2
# Debug level 3 use CFLAGS=-DDEBUG3

CC=g++ -std=c++0x
CCFLAGS=-c -Wall -Wno-strict-aliasing -O3 -pg
LDFLAGS=-lm -pg
RMFLAGS=-f
OBJ-SOURCES=usp.c hash_table.c permutation.c groups.c matrix.c puzzle.c set.c
EXE-SOURCES=groups_tester.c matrix_tester.c usp_tester.c  \
	permutation_tester.c hash_table_tester.c puzzle_tester.c set_tester.c
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
