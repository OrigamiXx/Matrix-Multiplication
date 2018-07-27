/*
  Testing code for graph module.

  Author: Matt.
*/

#include "Graph.hpp"
#include "ExtensionGraph.hpp"
#include <cstdio>
#include <string>
#include <cstdlib>


using namespace std;

bool randomEdge(bool edge, unsigned long label_u, unsigned long label_v, void * data){

  return lrand48() % 2 == 0;

}

bool removeGiven(unsigned long label, unsigned degree, void * data){

  return label != (unsigned long)data;

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
  
  g.mapVertices(removeGiven, (void *)2L);

  g.print();
  printf("==========\n");

  g.mapVertices(removeGiven, (void *)2L);

  g.print();
  printf("==========\n");

  g.mapVertices(removeGiven, (void *)3L);

  g.print();
  printf("==========\n");

  g.mapVertices(removeGiven, (void *)1L);

  g.print();
  printf("==========\n");
  

  g.mapEdgesB(randomEdge, NULL);

  g.print();
  printf("==========\n");

  g.mapEdgesB(randomEdge, NULL);

  g.print();
  printf("==========\n");

  Graph g2(g);

  g2.print();
  printf("==========\n");

  g2.mapVertices(removeGiven, (void *)8L);

  g.print();
  printf("==========\n");

  g2.print();
  printf("==========\n");

  puzzle * p = create_puzzle_from_string("111\n");
  
  ExtensionGraph eg(p);

  eg.print();
  printf("==========\n");

  ExtensionGraph eg2(p);
  eg2.print();
  printf("==========\n");

  p = create_puzzle_from_string("111\n232\n");
  eg2.update(p);

  eg.print();
  printf("==========\n");
  
  eg2.print();
  printf("==========\n");


  
  return 0;
}
