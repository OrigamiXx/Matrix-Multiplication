CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm -pg
RMFLAGS=-f
OBJ-SOURCES=CheckUSP.c hash_table.c permutation.c groups.c matrix.c	
EXE-SOURCES=groups_tester.c matrix_tester.c CheckUSP_tester.c  \
	permutation_tester.c hash_table_tester.c 
OBJDIR=objs
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ-SOURCES:.c=.o))
EXES=$(EXE-SOURCES:.c= )

#EXECUTABLE=hello

#all: $(OBJS) $(EXECUTABLES)

$(OBJDIR)/%.o : %.c
	$(CC) -c $< -o $@

% : %.c
	$(CC) $(OBJECTS) $(LDFLAGS) $@.c -o $@ 

all: $(OBJECTS) $(EXES)

#$(EXECUTABLE): $(OBJECTS) 
#    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

#.cpp.o:
#    $(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(RMFLAGS) $(OBJDIR)/*.o $(EXES) *~ *.out
