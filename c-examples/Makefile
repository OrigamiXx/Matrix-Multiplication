
linkedlist.o: linkedlist.c linkedlist.h
	gcc -c linkedlist.c

ll-tester: linkedlist.o
	gcc linkedlist.o linkedlist_tester.c -o ll-tester

all: ll-tester

clean:
	rm -f *.o *~
	rm -f ll-tester
