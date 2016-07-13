# Debug level 1 use CFLAGS=-DDEBUG1
# Debug level 2 use CFLAGS=-DDEBUG2
# Debug level 3 use CFLAGS=-DDEBUG3


MPICC=mpic++
CC=g++ 
CCFLAGS=-c -Wall -O3 -pg
LDFLAGS=-lm -pg
RMFLAGS=-f
OBJ-SOURCES=usp.c permutation.c puzzle.c set.c usp_bi.c
EXE-SOURCES=usp_tester.c permutation_tester.c puzzle_tester.c set_tester.c
PARA-EXE=usp_cluster
OBJDIR=objs
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ-SOURCES:.c=.o))
EXES=$(EXE-SOURCES:.c= )

MRMPI_SRC_PATH=./mrmpi-7Apr14/src/
MRMPI_LIB=libmrmpi_mpicc.a
MRMPI_L=$(MRMPI_SRC_PATH)$(MRMPI_LIB)

#EXECUTABLE=hello

#all: $(OBJS) $(EXECUTABLES)

$(MRMPI_L): 
	make -C $(MRMPI_SRC_PATH)  mpicc

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $(CCFLAGS) $< -o $@

%_cluster: %_cluster.c $(MRMPI_L)
	$(MPICC) -I $(MRMPI_SRC_PATH) $(OBJECTS) $(LDFLAGS) $< $(MRMPI_L) -o $@

% : %.c $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $@.c -o $@ 


all: $(MRMPI_L) $(OBJECTS) $(EXES) $(PARA-EXE) 

.PHONY: clean

clean:
	rm $(RMFLAGS) $(OBJDIR)/*.o $(EXES) *~ *.out
	rm -f $(PARA-EXE)
	make -C $(MRMPI_SRC_PATH) clean-all
	rm -f $(MRMPI_SRC_PATH)$(MRMPI_LIB)
