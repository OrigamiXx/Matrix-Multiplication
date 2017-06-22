
binary_tree.o: binary_tree.cc binary_tree.h
	gcc -c binary_tree.cc

tree-tester: binary_tree.o
	gcc binary_tree.o binary_tester.cc -o tree_tester

all: tree_tester

clean:
	rm -f *.o *~
	rm -f tree_tester
