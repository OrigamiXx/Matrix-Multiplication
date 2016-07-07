# MR-MPI static library multiple-machine Makefile

SHELL = /bin/sh
#.IGNORE:

# Definitions

ROOT =	mrmpi
EXE =	lib$(ROOT)_$@.a
SRC =	$(wildcard *.cpp)
INC =	$(wildcard *.h)
OBJ = 	$(SRC:.cpp=.o)

# List of all targets

help:
	@echo ''
	@echo 'make clean-all           delete all object files'
	@echo 'make clean-machine       delete object files for one machine'
	@echo 'make tar                 mrmpi_src.tar.gz of src dir'
	@echo 'make stubs               build dummy MPI library in mpistubs'
	@echo 'make install-python      install MR-MPI wrapper in Python'

	@echo ''
	@echo 'make -f Makefile.shlib machine    build MR-MPI shared library for machine'
	@echo 'make machine             build MR-MPI static library for machine:'

	@echo ''
	@files="`ls MAKE/Makefile.*`"; \
	  for file in $$files; do head -1 $$file; done
	@echo ''

# Build the code

.DEFAULT:
	@test -f MAKE/Makefile.$@
	@if [ ! -d Obj_$@ ]; then mkdir Obj_$@; fi
	@cp -p $(SRC) $(INC) Obj_$@
	@cp MAKE/Makefile.$@ Obj_$@/Makefile
	@cd Obj_$@; \
	$(MAKE) $(MFLAGS) "OBJ = $(OBJ)" "INC = $(INC)" "SHFLAGS =" \
	  "EXE = ../$(EXE)" lib
	@if [ -d Obj_$@ ]; then cd Obj_$@; rm -f $(SRC) $(INC) Makefile*; fi

# Remove machine-specific object files

clean:
	@echo 'make clean-all           delete all object files'
	@echo 'make clean-machine       delete object files for one machine'

clean-all:
	rm -rf Obj_*

clean-%:
	rm -rf Obj_$(@:clean-%=%)

# Create a tarball of this dir

tar:
	@cd ..; tar cvzf src/$(ROOT)_src.tar.gz \
	  src/Make* src/MAKE src/*.cpp src/*.h --exclude=*/.svn
	@echo "Created $(ROOT)_src.tar.gz"

# Make MPI STUBS library

stubs:
	@cd STUBS; make clean; make

# install MR-MPI shared lib and Python wrapper for Python usage

install-python:
	@python ../python/install.py
