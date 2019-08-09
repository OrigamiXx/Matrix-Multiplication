# Debug level 1 use CFLAGS=-DDEBUG1
# Debug level 2 use CFLAGS=-DDEBUG2
# Debug level 3 use CFLAGS=-DDEBUG3


MPICC=mpic++
CC=g++

NAUTY=nauty.o nautil.o nausparse.o naugraph.o schreier.o naurng.o
NAUTYDIR=nauty26r7

COMMONFLAGS= -Wall -O3 -ggdb -std=c++17 -g -pg
CCFLAGS=-c $(COMMONFLAGS)
CFLAGS=-I $(NAUTYDIR)/
LDFLAGS=-lm -pthread $(COMMONFLAGS) 
RMFLAGS=-f

# Put additional object sources in list below.
OBJ-SOURCES=checker.c permutation.c puzzle.c set.c matching.c 3DM_to_SAT.c timing.c heuristic.c construct.c clique_to_mip.c canonization.c search_MIP.c searcher2.c search_timer.c check_PiDD.c
# Put additional executable sources in list below.
TESTER-SOURCES=test_3DM_to_SAT.c test_permutation.c test_puzzle.c test_set.c test_matching.c test_scratch.c test_canonization.c test_graph.c test_search_MIP.c test_PiDD.c
UTIL-SOURCES=util_check_benchmark.c util_check_file.c util_timer.c util_generate_puzzle.c util_join.c util_construct.c util_submodular.c util_search_automorphism.c util_generate_puzzle_data.c
SEARCH-SOURCES=search_greedy.c search_ils.c search_clique.c search_astar.c search_run2.c search_really_greedy.c
EXE-SOURCES=$(TESTER-SOURCES) $(UTIL-SOURCES) $(SEARCH-SOURCES)

ifdef GUROBI_HOME
OBJ-SOURCES+= 3DM_to_MIP.c
EXE-SOURCES+= test_3DM_to_MIP.c
LDFLAGS+= -L $(GUROBI_HOME)/lib -lgurobi_c++ -lgurobi81
CFLAGS+=-D__GUROBI_INSTALLED__  -I $(GUROBI_HOME)/include
endif

# Put additional parallel / cluster executable sources in list below, must end with "_para".
PARA-SOURCES=search_para.c search_batch_para.c

OBJDIR=objs
BINDIR=bin
SRCDIR=csrc
LOCAL-OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ-SOURCES:.c=.o))
OBJECTS=$(LOCAL-OBJECTS) $(addprefix $(NAUTYDIR)/,$(NAUTY))
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

$(OBJDIR)/%.o : $(SRCDIR)/%.c  $(SRCDIR)/%.h
	$(CC) -I ./SAT $(CFLAGS) $(CCFLAGS) $< -o $@


$(BINDIR)/%_solver:
	make -C $(SOLVER_SRC_PATH) rs
	cp $(SOLVER_SRC_PATH)/minisat_static $(BINDIR)/minisat_solver

$(BINDIR)/%_para: $(SRCDIR)/%_para.c $(MRMPI_L)
	$(MPICC) -I $(MRMPI_SRC_PATH) $(OBJECTS) $(SOLVER_OBJECTS) $(CFLAGS) $(LDFLAGS) $< $(MRMPI_L) -o $@

$(BINDIR)/% : $(SRCDIR)/%.c $(OBJECTS)
	$(CC) $(OBJECTS) $(SOLVER_OBJECTS) $(CFLAGS) $(LDFLAGS)  $< -o $@

tmp_dirs:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p logs

all: tmp_dirs $(MRMPI_L) $(SOLVER_EXES) $(OBJECTS) $(BINS)

.PHONY: clean

clean:
	rm -f *~ *.out
	$(MAKE) -C $(MRMPI_SRC_PATH) clean-all
	$(MAKE) -C $(SOLVER_SRC_PATH) clean
	rm -f $(MRMPI_SRC_PATH)$(MRMPI_LIB)
	rm -fr $(BINS)
	rm -fr $(LOCAL-OBJECTS)
