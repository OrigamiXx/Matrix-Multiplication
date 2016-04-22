

linklist.o:
	gcc -o linkedlist.o -c linkedlist.c

tester: linklist.o
	gcc linkedlist.o linkedlist_tester.c -o tester

clean:
	rm -f linkedlist.o tester *~
