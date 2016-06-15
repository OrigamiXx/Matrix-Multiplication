matrix.o:
	gcc -o matrix.o -c matrix.c

groups.o:
	gcc -o groups.o -c groups.c

permutation.o:
	gcc -o permutation.o -c permutation.c

linklist.o:
	gcc -o linkedlist.o -c linkedlist.c

CheckUSP.o: 
	gcc -o CheckUSP.o -c CheckUSP.c

ll-tester: linklist.o
	gcc linkedlist.o linkedlist_tester.c -o ll-tester

perm-tester: permutation.o
	gcc permutation.o permutation_tester.c -o perm-tester

groups_tester: groups.o permutation.o
	gcc permutation.o groups.o groups_tester.c -o groups_tester

matrix_tester: groups.o permutation.o matrix.o CheckUSP.o
	gcc permutation.o groups.o matrix.o CheckUSP.o matrix_tester.c -o matrix_tester -lm

CU-tester: CheckUSP.o permutation.o
	gcc permutation.o CheckUSP.o CheckUSP_tester.c -o CU-tester

clean:
	rm -f *.o *tester *~
