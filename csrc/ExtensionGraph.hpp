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

    // Remove all vertices corresponding to rows that cannot extend puzzle.
    if (full)
      // Check all rows.
      reduceVertices(
		     [p_tmp](unsigned long index, unsigned long label, unsigned long degree) -> bool{
		       p_tmp -> puzzle[p_tmp -> s - 1] = (puzzle_row) label;
		       return IS_USP == check(p_tmp);});
    else {
      // Use existing edges to determine.
      assert(p -> s > 0);
      unsigned long last_index = getIndex(p -> puzzle[p -> s - 1]);
      bool edges[size()];
      for (unsigned long u = 0; u < size(); u++)
	edges[u] = hasEdge(last_index, u);

      reduceVertices(
		     [&edges](unsigned long index, unsigned long label, unsigned long degree) -> bool{
		       return edges[index];
		     });
      
    }
    p_tmp -> s++;

    // Place edges which correspond to double extensions.
    if (full)
      // Check all edges.
      mapEdges(
	       [p_tmp](bool edge, unsigned long label1, unsigned long label2) -> bool{
		 p_tmp -> puzzle[p_tmp -> s - 2] = (puzzle_row) label1;
		 p_tmp -> puzzle[p_tmp -> s - 1] = (puzzle_row) label2;
		 return IS_USP == check(p_tmp);
	       });
    else
      // Check only edges that previously existed.
      reduceEdges(
		  [p_tmp](unsigned long label1, unsigned long label2) -> bool{
		    p_tmp -> puzzle[p_tmp -> s - 2] = (puzzle_row) label1;
		    p_tmp -> puzzle[p_tmp -> s - 1] = (puzzle_row) label2;
		    return IS_USP == check(p_tmp);
		  });

    destroy_puzzle(p_tmp);

  }

};
