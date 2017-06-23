# linux = Linux desktop machine, g++, MPICH

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/linker settings
# specify flags and libraries needed for your compiler and MPI

CC =		g++
CCFLAGS =	-O -DMPICH_IGNORE_CXX_SEEK
SHFLAGS =	-fPIC
DEPFLAGS =	-M

ARCHIVE =	ar
ARFLAGS =	-rc
SHLIBFLAGS =	-shared
SHLIB =         -lmpich -lmpl -lpthread

# ---------------------------------------------------------------------
# build rules and dependencies
# no need to edit this section

# Library targets

lib:	$(OBJ)
	$(ARCHIVE) $(ARFLAGS) $(EXE) $(OBJ)

shlib:	$(OBJ)
	$(CC) $(CCFLAGS) $(SHFLAGS) $(SHLIBFLAGS) -o $(EXE) $(OBJ) $(SHLIB)

# Compilation rules

%.o:%.cpp
	$(CC) $(CCFLAGS) $(SHFLAGS) -c $<

%.d:%.cpp
	$(CC) $(CCFLAGS) $(DEPFLAGS) $< > $@

# Individual dependencies

DEPENDS = $(OBJ:.o=.d)
include $(DEPENDS)
