# Debug level 1 use CFLAGS=-DDEBUG1
# Debug level 2 use CFLAGS=-DDEBUG2
# Debug level 3 use CFLAGS=-DDEBUG3


MPICC=mpic++
CC=g++ 
CCFLAGS=-c -Wall -O3 -pg
# Comment out line below on cluster.
CCFLAGS += -std=c++11
LDFLAGS=-lm -pg
RMFLAGS=-f
# Put additional object sources in list below.
OBJ-SOURCES=usp.c permutation.c puzzle.c set.c usp_bi.c
# Put additional executable sources in list below.
EXE-SOURCES=usp_tester.c permutation_tester.c puzzle_tester.c set_tester.c generate_puzzle.c
# Put additional parallel / cluster executable sources in list below, must end with "_para".
PARA-SOURCES=usp_para.c
OBJDIR=objs
BINDIR=bin
SRCDIR=src
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ-SOURCES:.c=.o))
EXES=$(EXE-SOURCES:.c= )
PARA-EXES=$(PARA-SOURCES:.c= )
BINS=$(addprefix $(BINDIR)/,$(EXES)) $(addprefix $(BINDIR)/,$(PARA-EXES))
MRMPI_SRC_PATH=./mrmpi-7Apr14/src/
MRMPI_LIB=libmrmpi_mpicc.a
MRMPI_L=$(MRMPI_SRC_PATH)$(MRMPI_LIB)

$(MRMPI_L): 
	make -C $(MRMPI_SRC_PATH)  mpicc

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(CCFLAGS) $< -o $@

$(BINDIR)/%_para: $(SRCDIR)/%_para.c $(MRMPI_L)
	echo $(BINS)
	$(MPICC) -I $(MRMPI_SRC_PATH) $(OBJECTS) $(LDFLAGS) $< $(MRMPI_L) -o $@

$(BINDIR)/% : $(SRCDIR)/%.c $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $< -o $@ 


tmp_dirs:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

all: tmp_dirs $(MRMPI_L) $(OBJECTS) $(BINS)

.PHONY: clean

clean:
	rm $(RMFLAGS) $(OBJDIR)/*.o $(EXES) *~ *.out
	rm -f $(PARA-EXES)
	make -C $(MRMPI_SRC_PATH) clean-all
	rm -f $(MRMPI_SRC_PATH)$(MRMPI_LIB)
	rm -fr $(BINDIR)
	rm -fr $(OBJDIR)
