/* 
   Class for undirected graphs that represent the rows that a puzzle
   can be extended by.  Built on top of Graph.  Allows for efficient
   reduction of graph as puzzle grows larger.

   Author: Matt.
*/


#pragma once
#include "Graph.hpp"
#include "checker.h"


class ExtensionGraph : public Graph {

  // Has no fields of its own.
  
 public:

  // Constructs graph of the row extensions of the given puzzle.
  //
  // WARNING: This function is inefficient.  Do not use this function
  // if already have an ExtensionGraph constructed for p, call
  // update(p) instead.
  ExtensionGraph(puzzle * p) : Graph(p -> max_row) { update(p, true); }

  // Copy constructor.
  ExtensionGraph(const ExtensionGraph &eg) : Graph(eg) {}

  // Reduces the graph based on puzzle p.  Removes vertices
  // corresponding to rows that p cannot be extended with.  Removes
  // edges corresponding to pairs of rows that p cannot be extended
  // with.
  //
  // WARNING: Only call this function with a puzzle p which is an
  // extension of the previous puzzles used to create and update this
  // ExtensionGraph.
  void update(puzzle * p){
    
    update(p, false);
    
  }
  
 private:

  // Reduces the graph based on puzzle p.  If full is true it tests
  // every pair (u,v).  If full is false it only tests pairs (u,v)
  // with an edge.
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
  
  // Returns true iff the puzzle which is data can be extended by row label.
  static bool validVertex(unsigned long label, unsigned long degree, void * data){

    puzzle * p_tmp = (puzzle *)data;

    p_tmp -> puzzle[p_tmp -> s - 1] = (puzzle_row) label;

    return IS_USP == check(p_tmp);

  }

    // Returns true iff the puzzle which is data can be extended by row label.
  static bool validEdgeB(bool edge, unsigned long label1, unsigned long label2, void * data){

    return validEdge(label1, label2, data);
    
  }

  // Returns true iff the puzzle which is data can be extended by rows label1 and label2.
  static bool validEdge(unsigned long label1, unsigned long label2, void * data){

    puzzle * p_tmp = (puzzle *) data;

    p_tmp -> puzzle[p_tmp -> s - 2] = (puzzle_row) label1;
    p_tmp -> puzzle[p_tmp -> s - 1] = (puzzle_row) label2;
  
    return IS_USP == check(p_tmp);
    
  }
  

};
