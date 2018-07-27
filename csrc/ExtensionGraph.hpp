#pragma once
#include "Graph.hpp"
#include "checker.h"


class ExtensionGraph : public Graph {

 public:

  ExtensionGraph(puzzle * p) : Graph(p -> max_row) { update(p, true); }

  ExtensionGraph(const ExtensionGraph &eg) : Graph(eg) {}
  
  void update(puzzle * p){
    
    update(p, false);
    
  }
  
 private:

  void update(puzzle * p, bool full){

    puzzle * p_tmp = extend_puzzle(p, 2);
    p_tmp -> s--;
    reduceVertices(ExtensionGraph::validVertex, p_tmp, false);
    p_tmp -> s++;
    if (full)
      mapEdges(ExtensionGraph::validEdgeB, p_tmp);
    else
      reduceEdges(ExtensionGraph::validEdge, p_tmp);
    destroy_puzzle(p_tmp);

  }
  
     
  static bool validVertex(unsigned long label, unsigned long degree, void * data){

    puzzle * p_tmp = (puzzle *)data;

    p_tmp -> puzzle[p_tmp -> s - 1] = (puzzle_row) label;

    return IS_USP == check(p_tmp);

  }
  
  static bool validEdgeB(bool edge, unsigned long label1, unsigned long label2, void * data){

    return validEdge(label1, label2, data);
    
  }

  static bool validEdge(unsigned long label1, unsigned long label2, void * data){

    puzzle * p_tmp = (puzzle *) data;

    p_tmp -> puzzle[p_tmp -> s - 2] = (puzzle_row) label1;
    p_tmp -> puzzle[p_tmp -> s - 1] = (puzzle_row) label2;
  
    return IS_USP == check(p_tmp);
    
  }
  

};
