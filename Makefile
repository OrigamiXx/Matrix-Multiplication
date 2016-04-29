
permutation.o:
	gcc -o permutation.o -c permutation.c

linklist.o:
	gcc -o linkedlist.o -c linkedlist.c

ll-tester: linklist.o
	gcc linkedlist.o linkedlist_tester.c -o ll-tester

perm-tester: permutation.o
	gcc permutation.o permutation_tester.c -o perm-tester

clean:
	rm -f *.o *tester *~
