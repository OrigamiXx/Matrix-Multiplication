# OINK multiple-machine Makefile

SHELL = /bin/sh
#.IGNORE:

# Definitions

ROOT =	oink
EXE =	$(ROOT)_$@
SRC =	$(wildcard *.cpp)
INC =	$(wildcard *.h)
OBJ = 	$(SRC:.cpp=.o)

# List of all targets

help:
	@echo ''
	@echo 'make clean-all           delete all object files'
	@echo 'make clean-machine       delete object files for one machine'
	@echo 'make tar                 oink_src.tar.gz of src dir'
	@echo 'make makelib             update Makefile.lib for library build'
	@echo 'make makelist            update Makefile.list used by old makes'
	@echo ''
	@echo 'make machine             build OINK where machine is one of:'
	@echo ''
	@files="`ls MAKE/Makefile.*`"; \
	  for file in $$files; do head -1 $$file; done
	@echo ''

# Build the code

.DEFAULT:
	@test -f MAKE/Makefile.$@
	@if [ ! -d Obj_$@ ]; then mkdir Obj_$@; fi
	@python Make.py
	@cp -p *.cpp *.h Obj_$@
	@cp MAKE/Makefile.$@ Obj_$@/Makefile
	@cd Obj_$@; \
	$(MAKE) $(MFLAGS) "OBJ = $(OBJ)" "INC = $(INC)" "EXE = ../$(EXE)" ../$(EXE)
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

# Update Makefile.lib and Makefile.list

makelib:
	@$(SHELL) Make.sh style
	@$(SHELL) Make.sh Makefile.lib

makelist:
	@$(SHELL) Make.sh style
	@$(SHELL) Make.sh Makefile.list
