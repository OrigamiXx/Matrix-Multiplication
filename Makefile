# Debug level 1 use CFLAGS=-DDEBUG1
# Debug level 2 use CFLAGS=-DDEBUG2
# Debug level 3 use CFLAGS=-DDEBUG3


MPICC=mpic++
CC=g++
CCFLAGS=-c -Wall -O3 -pg -ggdb
LDFLAGS=-lm -pg -pthread -lgurobi_c++ -lgurobi75
RMFLAGS=-f
# Put additional object sources in list below.
OBJ-SOURCES=usp.c permutation.c puzzle.c set.c usp_bi.c matching.c 3DM_to_SAT.c checkUSP_mip.c
# Put additional executable sources in list below.
EXE-SOURCES=permutation_tester.c puzzle_tester.c set_tester.c generate_puzzle.c usp_exp.c matching_tester.c test.c usp_construct.c usp_test_file.c usp_greedy.c checkUSP_mip_tester.c usp_tester.c 3DM_to_SAT_tester.c
# Put additional parallel / cluster executable sources in list below, must end with "_para".
PARA-SOURCES=usp_para.c

OBJDIR=objs
BINDIR=bin
SRCDIR=csrc
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ-SOURCES:.c=.o))
SOLVER_OBJECTS=$(SOLVER_SRC_PATH)/Solver.or
EXES=$(EXE-SOURCES:.c= )
PARA-EXES=$(PARA-SOURCES:.c= )
SOLVER-EXES=minisat_solver
BINS=$(addprefix $(BINDIR)/,$(SOLVER-EXES)) $(addprefix $(BINDIR)/,$(EXES)) $(addprefix $(BINDIR)/,$(PARA-EXES))
MRMPI_SRC_PATH=./MRMPI/src/
MRMPI_LIB=libmrmpi_mpicc.a
MRMPI_L=$(MRMPI_SRC_PATH)$(MRMPI_LIB)
SOLVER_SRC_PATH=./SAT/core/
export MROOT=$(shell pwd)/SAT/

$(MRMPI_L):
	make -e -C $(MRMPI_SRC_PATH)  mpicc

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) -I $(GUROBI_HOME)/include -I ./SAT $(CFLAGS) $(CCFLAGS) $< -o $@


$(BINDIR)/%_solver:
	make -C $(SOLVER_SRC_PATH) rs
	cp $(SOLVER_SRC_PATH)/minisat_static $(BINDIR)/minisat_solver

$(BINDIR)/%_para: $(SRCDIR)/%_para.c $(MRMPI_L)
	$(MPICC) -I $(MRMPI_SRC_PATH) $(OBJECTS) $(SOLVER_OBJECTS) $(LDFLAGS) -L $(GUROBI_HOME)/lib $< $(MRMPI_L) -o $@

$(BINDIR)/% : $(SRCDIR)/%.c $(OBJECTS)
	$(CC) -I $(GUROBI_HOME)/include $(OBJECTS) $(SOLVER_OBJECTS) -L /home/andersm2/gurobi751/linux64/lib $(LDFLAGS) $< -o $@

tmp_dirs:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

all: tmp_dirs $(MRMPI_L) $(SOLVER_EXES) $(OBJECTS) $(BINS)

.PHONY: clean

clean:
	rm -f *~ *.out
	$(MAKE) -C $(MRMPI_SRC_PATH) clean-all
	$(MAKE) -C $(SOLVER_SRC_PATH) clean
	rm -f $(MRMPI_SRC_PATH)$(MRMPI_LIB)
	rm -fr $(BINS)
	rm -fr $(OBJECTS)
