#pragma once
#include "nauty.h"
#include <cassert>
#include <cstdio>
#include <string>

using namespace std;

class Graph {

  graph * g;
  unsigned long n;
  unsigned long * labels;
  unsigned long * degrees;
  int m;

 public:
  
 Graph(unsigned long n)
   : n(n)
  {
    m = SETWORDSNEEDED(n);

    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    
    g = new graph[m * n];
    EMPTYGRAPH(g,m,n);
    
    labels = new unsigned long[n];
    degrees = new unsigned long[n];
    for (int i = 0; i < n; i++) {
      labels[i] = i;
      degrees[i] = 0;
    }

  }
  
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
  
  ~Graph(){
    
    delete g;
    delete labels;
    delete degrees;
    
  }

  void addEdge(unsigned long u, unsigned long v){
    if (!hasEdge(u,v)){
      ADDONEEDGE(g, u, v, m);
      degrees[u]++;
      if (u != v)
	degrees[v]++;
    }
  }

  void removeEdge(unsigned long u, unsigned long v){
    if (hasEdge(u,v)){
      DELELEMENT(GRAPHROW(g, u, m), v);
      DELELEMENT(GRAPHROW(g, v, m), u);
      degrees[u]--;
      if (u != v)
	degrees[v]--;
    }
  }

  bool hasEdge(unsigned long u, unsigned long v){
    assert(u < n && v < n);
    return ISELEMENT(GRAPHROW(g, u, m), v);
  }

  unsigned long size(){
    return n;
  }

  void removeEdges(unsigned long u){
    for (unsigned long v = 0; v < n; v++)
      removeEdge(u,v);
  }

  unsigned long getDegree(unsigned long u){
    return degrees[u];
  }

  unsigned long getLabel(unsigned long u){
    return labels[u];
  }
  
  void reduceVertices(bool (*func)(unsigned long, unsigned long, void *), void * user_data = NULL, bool reach_fixed_point = false){

    bool not_valid[n];
    unsigned long count = 0;
    bzero(not_valid, sizeof(bool) * n);

    bool progress = false;
    do {
      progress = false;
      for (unsigned long u = 0; u < n; u++){
	if (!not_valid[u] && !func(labels[u], degrees[u], user_data)){
	  removeEdges(u);
	  not_valid[u] = true;
	  count++;
	  progress = true;
	}
      }
    } while (reach_fixed_point && progress);

    unsigned long new_n = n - count;
    unsigned long * new_labels = new unsigned long[new_n];
    unsigned long * new_degrees = new unsigned long[new_n];
    int new_m = SETWORDSNEEDED(new_n);
    graph * new_g = new graph[new_m * new_n];

    int new_u = 0;
    for (int u = 0; u < n; u++){
      if (not_valid[u]) continue;
      new_labels[new_u] = labels[u];
      new_degrees[new_u] = degrees[u];
      int new_v = 0;
      for (int v = 0; v < n; v++){
	if (not_valid[v]) continue;
	
	if (hasEdge(u,v)) ADDONEEDGE(new_g, new_u, new_v, new_m);
	new_v++;
      }
      new_u++;
    }

    delete labels;
    delete g;
    n = new_n;
    m = new_m;
    labels = new_labels;
    degrees = new_degrees;
    g = new_g;
    
  }

  void reduceEdges(bool (*func)(unsigned long, unsigned long, void *), void * user_data = NULL){
  
    for (int u = 0; u < n; u++)
      for (int v = u; v < n; v++)
	if (hasEdge(u,v) && !func(labels[u],labels[v], user_data))
	  removeEdge(u,v);
    
  }
  
  void mapEdges(bool (*func)(bool, unsigned long, unsigned long, void *), void * user_data = NULL){
    
    for (int u = 0; u < n; u++)
      for (int v = u; v < n; v++)
	if (func(hasEdge(u,v), labels[u],labels[v], user_data))
	  addEdge(u,v);
	else
	  removeEdge(u,v);
    
  }

  void print(){

    for (int u = 0; u < n; u++)
      printf("%5ld ", labels[u]);
    printf("\n");

    for (int u = 0; u < n; u++)
      printf("%5ld ", degrees[u]);
    printf("\n");
      
    for (int u = 0; u < n; u++){
      for (int v = 0; v < n; v++){
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
