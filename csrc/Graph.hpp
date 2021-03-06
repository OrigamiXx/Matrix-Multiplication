/* 
   Class for representing undirected graphs.  Basically a wrapper for
   Nauty's dense graph representation.  Each vertex has an index,
   label and degree.  Includes functional operations for modifying
   graph.

   Author: Matt.
*/


#pragma once
#include "nauty.h"
#include <cassert>
#include <cstdio>
#include <string>
#include "constants.h"

using namespace std;

class Graph {

  // Nauty representation of graph.
  graph * g;
  // Number of vertices.
  unsigned long n;
  // Initially same as index, but changes if vertices removed.
  unsigned long * labels;
  unsigned long * degrees;
  // Number of WORDSIZE-size values used to represent each row of graph in Nauty.
  int m; 

 public:

  // Constructs empty graph with n vertices.
  Graph(unsigned long n)
    : n(n)
  {
    
    m = SETWORDSNEEDED(n);
    
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    
    g = new graph[m * n];
    EMPTYGRAPH(g,m,n);
    
    labels = new unsigned long[n];
    degrees = new unsigned long[n];
    for (unsigned long i = 0; i < n; i++) {
      labels[i] = i;
      degrees[i] = 0;
    }

  }

  // Copy constructor.
  Graph(const Graph &G)
    : n(G.n), m(G.m)
  {
    
    g = new graph[m * n];
    memcpy(g, G.g, sizeof(graph) * m * n);
    
    labels = new unsigned long[n];
    memcpy(labels, G.labels, sizeof(unsigned long) * n);
    
    degrees = new unsigned long[n];
    memcpy(degrees, G.degrees, sizeof(unsigned long) * n);
    
  }

  // Default destructor.
  ~Graph(){
    
    delete g;
    delete labels;
    delete degrees;
    
  }

  // Returns the number of vertices in the graph.
  unsigned long size(){
    return n;
  }

  // Returns whether there is an edge between vertexes indexed by u
  // and v.
  bool hasEdge(unsigned long u, unsigned long v){
    assert(u < n && v < n);
    return ISELEMENT(GRAPHROW(g, u, m), v);
  }
  
  // Adds undirected edge between vertexes indexed by u and v.
  // Idempotent.
  void addEdge(unsigned long u, unsigned long v){
    if (!hasEdge(u,v)){
      ADDONEEDGE(g, u, v, m);
      degrees[u]++;
      if (u != v)
	degrees[v]++;
    }
  }

  // Removes undirected edge between vertexes indexed by u and v.
  // Idempotent.
  void removeEdge(unsigned long u, unsigned long v){
    if (hasEdge(u,v)){
      DELELEMENT(GRAPHROW(g, u, m), v);
      assert(degrees[u] > 0);
      degrees[u]--;
      if (u != v){
	DELELEMENT(GRAPHROW(g, v, m), u);
	assert(degrees[v] > 0);
	degrees[v]--;
      }
    }
  }

  // Removes all undirected edges incident the vertex indexed by u.
  void removeEdges(unsigned long u){
    for (unsigned long v = 0; v < n; v++)
      removeEdge(u,v);
  }

  // Returns the degree of the vertex indexed by u.
  unsigned long getDegree(unsigned long u){
    return degrees[u];
  }
  
  // Returns the minimum degree
  unsigned long getMinDegree(){
    unsigned long min_degree = degrees[0];
    for (unsigned long u = 1; u < n; u++)
      min_degree = MIN(min_degree, degrees[u]);
    return min_degree;
  }
  
  // Returns the minimum degree
  unsigned long getMaxDegree(){
    unsigned long max_degree = degrees[0];
    for (unsigned long u = 1; u < n; u++)
      max_degree = MAX(max_degree, degrees[u]);
    return max_degree;
  }

  
  
  // Returns the label of the vertex indexed by u.
  unsigned long getLabel(unsigned long u){
    return labels[u];
  }

  // Returns the index of label of the vertex label by l.
  unsigned long getIndex(unsigned long l){
    for (unsigned long u = 0; u < n; u++)
      if (labels[u] == l)
	return u;
    assert(0 == "Unreachable"); // Should be unreachable.
  }

  

  
  // Takes a function called func whose argument will be the label and
  // degree of a vertex.  reduceVertices calls func(label_u,degree_u)
  // on every vertex u.  If func returns false, vertex u is removed
  // from the graph.  If reach_fixed_point is set to true, this
  // process repeats until no vertices are removed.  This function
  // will shrink the size of the graph when vertices are removed.
  // WARNING: When the graph shrinks vertex labels are not changed,
  // but vertex indexes may changed.
  void reduceVertices(auto func, //bool (*func)(unsigned long, unsigned long),
		      bool reach_fixed_point = false){

    bool not_valid[n];
    unsigned long count = 0;
    bzero(not_valid, sizeof(bool) * n);

    bool progress = false;
    do {
      progress = false;
      for (unsigned long u = 0; u < n; u++){
	if (!not_valid[u] && !func(u, labels[u], degrees[u])){
	  removeEdges(u);
	  not_valid[u] = true;
	  count++;
	  progress = true;
	}
      }
    } while (reach_fixed_point && progress);

    if (count == 0) return;
    
    unsigned long new_n = n - count;
    unsigned long * new_labels = new unsigned long[new_n];
    unsigned long * new_degrees = new unsigned long[new_n];
    int new_m = SETWORDSNEEDED(new_n);
    graph * new_g = new graph[new_m * new_n];
    bzero(new_g, sizeof(graph) * new_m * new_n);

    int new_u = 0;
    for (unsigned long u = 0; u < n; u++){
      if (not_valid[u]) continue;
      new_labels[new_u] = labels[u];
      new_degrees[new_u] = degrees[u];
      int new_v = 0;
      for (unsigned long v = 0; v < n; v++){
	if (not_valid[v]) continue;
	
	if (hasEdge(u,v)) ADDONEEDGE(new_g, new_u, new_v, new_m);
	new_v++;
      }
      new_u++;
    }

    delete labels;
    delete g;
    delete degrees;
    n = new_n;
    m = new_m;
    labels = new_labels;
    degrees = new_degrees;
    g = new_g;
    
  }

  // Takes a function called func whose argument will be the labels of
  // two vertices which have an edge between them.  reduceEdges calls
  // func(label_u,label_v) on every pair of vertices u and v
  // who have an edge between them.  The edge is removed if func
  // returns false.
  void reduceEdges(auto func){//bool (*func)(unsigned long, unsigned long)){
  
    for (unsigned long u = 0; u < n; u++)
      for (unsigned long v = u; v < n; v++)
	if (hasEdge(u,v) && !func(labels[u],labels[v]))
	  removeEdge(u,v);
    
  }

  // Takes a function called func whose argument will be the labels of
  // two vertices, whether there is an edge between the vertices.
  // reduceEdges calls func(has_edge,label_u,label_v) on
  // every pair of vertices u and v.  The edge is added if func
  // returns true and removed if func returns false.
  void mapEdges(auto func){//bool (*func)(bool, unsigned long, unsigned long)){
    
    for (unsigned long u = 0; u < n; u++)
      for (unsigned long v = u; v < n; v++)
	if (func(hasEdge(u,v), labels[u],labels[v]))
	  addEdge(u,v);
	else
	  removeEdge(u,v);
    
  }

  // Prints the graph to the console.
  void print(){

    for (unsigned long u = 0; u < n; u++)
      printf("%5ld ", labels[u]);
    printf("\n");

    for (unsigned long u = 0; u < n; u++)
      printf("%5ld ", degrees[u]);
    printf("\n");
      
    for (unsigned long u = 0; u < n; u++){
      for (unsigned long v = 0; v < n; v++){
	if (hasEdge(u,v))
	  printf("1");
	else
	  printf("0");
      }
      printf("\n");
    }
  }
    
 private:

  
};
