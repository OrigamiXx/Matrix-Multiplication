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

  unsigned long target = 0;
  
  auto removeTarget =
    [&target](unsigned long index, unsigned long label, unsigned long degree) -> bool{
    return label != target;
  };

  auto randomEdge = [](bool edge, unsigned long label_u, unsigned long label_v) -> bool{
    return lrand48() % 2 == 0;
  };
  
  target = 2L;
  g.reduceVertices(removeTarget);

  g.print();
  printf("==========\n");

  target = 3L;
  g.reduceVertices(removeTarget);

  g.print();
  printf("==========\n");

  g.reduceVertices(removeTarget);

  g.print();
  printf("==========\n");

  target = 1L;
  g.reduceVertices(removeTarget);

  g.print();
  printf("==========\n");
  
  g.mapEdges(randomEdge);

  g.print();
  printf("==========\n");

  g.mapEdges(randomEdge);

  g.print();
  printf("==========\n");

  Graph g2(g);

  g2.print();
  printf("==========\n");

  target = 8L;
  g2.reduceVertices(removeTarget);

  g.print();
  printf("==========\n");

  g2.print();
  printf("==========\n");

  puzzle * p = create_puzzle_from_string((char *)"111\n");
  
  ExtensionGraph eg(p);

  eg.print();
  printf("==========\n");

  ExtensionGraph eg2(p);
  eg2.print();
  printf("==========\n");

  p = create_puzzle_from_string((char *)"111\n232\n");
  eg2.update(p);

  eg.print();
  printf("==========\n");
  
  eg2.print();
  printf("==========\n");


  
  return 0;
}
