# linux = g++, MPICH in /usr/bin/local

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/linker settings
# specify flags and libraries needed for your compiler and MPI installation

CC =		g++4
CCFLAGS =	-g -O -I../../src -DMPICH_IGNORE_CXX_SEEK 
DEPFLAGS =	-M
LINK =		g++4
LINKFLAGS =	-g -O -L../../src
LIB =           -lmrmpi_linux -lmpich -lpthread
ARCHIVE =	ar
ARFLAGS =	-rc
SIZE =		size

# ---------------------------------------------------------------------
# build rules and dependencies
# no need to edit this section

# Link target

$(EXE):	$(OBJ)
	$(LINK) $(LINKFLAGS) $(OBJ) $(LIB) -o $(EXE)
	$(SIZE) $(EXE)

# Library target

lib:	$(OBJ)
	$(ARCHIVE) $(ARFLAGS) $(EXE) $(OBJ)

# Compilation rules

%.o:%.cpp
	$(CC) $(CCFLAGS) $(EXTRA_INC) -c $<

%.d:%.cpp
	$(CC) $(CCFLAGS) $(EXTRA_INC) $(DEPFLAGS) $< > $@

# Individual dependencies

DEPENDS = $(OBJ:.o=.d)
include $(DEPENDS)
