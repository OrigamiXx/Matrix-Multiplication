#include "graph.h"
#include <cstdio>
#include <string>
#include <cstdlib>

using namespace std;

bool randomEdge(bool hasEdge, unsigned long label_u, unsigned long label_v, void * data){

  return lrand48() % 2 == 0;

}

bool removeGiven(unsigned long label, void * data){

  return label == (unsigned long)data;

}

int main(int argc, char * argv[]){

  srand48(time(NULL));

  Graph g(10);

  printf("%d\n", g.hasEdge(9,9));
  g.print();
  printf("==========\n");
 
  g.addEdge(1,1);
  g.addEdge(9,9);
  g.addEdge(0,9);
  g.addEdge(1,2);
  g.print();
  printf("==========\n");
  
  g.mapDeleteVertices(removeGiven, (void *)2L);

  g.print();
  printf("==========\n");

  g.mapDeleteVertices(removeGiven, (void *)2L);

  g.print();
  printf("==========\n");

  g.mapDeleteVertices(removeGiven, (void *)3L);

  g.print();
  printf("==========\n");

  g.mapDeleteVertices(removeGiven, (void *)1L);
  
  g.print();
  printf("==========\n");

  g.mapEdges(randomEdge, NULL);

  g.print();
  printf("==========\n");

  g.mapEdges(randomEdge, NULL);

  g.print();
  printf("==========\n");

  Graph g2(g);

  g2.print();
  printf("==========\n");

  g2.mapDeleteVertices(removeGiven, (void *)8L);

  g.print();
  printf("==========\n");

  g2.print();
  printf("==========\n");

  
  
  return 0;
}
