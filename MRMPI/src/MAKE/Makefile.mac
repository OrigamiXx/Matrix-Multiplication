# mac = Apple PowerBook G4 laptop, c++, native MPI

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/archive settings
# specify flags and libraries needed for your compiler and MPI installation

CC =		c++
CCFLAGS =	-O
SHFLAGS =	-fPIC
DEPFLAGS =	-M

ARCHIVE =	ar
ARFLAGS =	-rc
SHLIBFLAGS =	-shared
SHLIB =         

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
