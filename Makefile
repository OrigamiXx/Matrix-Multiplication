hash_table.o:
	gcc -o hash_table.o -c hash_table.c -pg

matrix.o:
	gcc -o matrix.o -c matrix.c -pg

groups.o:
	gcc -o groups.o -c groups.c -pg

permutation.o:
	gcc -o permutation.o -c permutation.c -pg

linklist.o:
	gcc -o linkedlist.o -c linkedlist.c -pg

CheckUSP.o: 
	gcc -o CheckUSP.o -c CheckUSP.c -pg

ll-tester: linklist.o
	gcc linkedlist.o linkedlist_tester.c -o ll-tester -pg

perm-tester: permutation.o
	gcc permutation.o permutation_tester.c -o perm-tester -pg

groups_tester: groups.o permutation.o hash_table.o
	gcc permutation.o groups.o hash_table.o groups_tester.c -o groups_tester -pg -lm

hash_table_tester: hash_table.o
	gcc hash_table.o hash_table_tester.c -o hash_table_tester -pg

matrix_tester: groups.o permutation.o matrix.o CheckUSP.o hash_table.o
	gcc permutation.o groups.o matrix.o CheckUSP.o hash_table.o matrix_tester.c -o matrix_tester -lm -pg

CU-tester: CheckUSP.o permutation.o
	gcc permutation.o CheckUSP.o CheckUSP_tester.c -o CU-tester -pg

clean:
	rm -f *.o *tester *~
