/* 
   Class for representing sets of permutations.
*/

#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <map>
#include "permutation.h"
#include <vector>
#include <cassert>

using namespace std;

typedef struct _transpose {
  
  uint8_t a;  
  uint8_t b;

} transpose;

typedef struct _PiDD_Node {
  
  // Children.  No meaning for leaf nodes.
  struct _PiDD_Node * left;
  struct _PiDD_Node * right;

  // Elements transposed by this node.  Both distinct if internal
  // nodes, otherwise equal 0, 1 to indicate leaf nodes.
  transpose t;

  // Number of permutations in DD rooted here.
  uint64_t size;
  
  // Tracks number of parents.  Deallocate when 0.
  uint16_t ref_count;

 
} PiDD_Node;



class PiDD_Factory;

class PiDD {
    
  private:

  friend class PiDD_Factory;
  
  PiDD_Node * root;

  PiDD();
  PiDD(PiDD_Node * root);
  
public:

  // Copy constructor
  PiDD& operator=(const PiDD& other);

  ~PiDD();
  
  bool operator==(const PiDD& dd) const; 

  // Union operator.
  PiDD operator|(const PiDD& other) const;

  // Intersection operator.
  PiDD operator&(const PiDD& other) const;

  // Apply transpose.
  PiDD operator*(const transpose t) const;
  // Cartesian Product.
  PiDD operator*(const PiDD& other) const;
  
  uint64_t size() const;

  bool is_empty() const;

  vector<perm *> enlist(int dim) const;
  void print_perms() const;
  
  bool validate() const;


  
};


/*
  Static class for creating PiDDs and performing operations.
*/

unsigned int PiDD_count = 0;
bool initialized = false;
PiDD_Node zero_node = (PiDD_Node){NULL, NULL, (transpose){0,0}, 0, 0};
PiDD_Node * zero = &zero_node;
PiDD_Node one_node = (PiDD_Node){NULL, NULL, (transpose){1,1}, 1, 0};
PiDD_Node * one = &one_node;
map<string, PiDD_Node *> node_cache;  // Could be replaced with node parent LLs to reduce memory overhead.
vector<PiDD_Node *> node_cache_recent;

class PiDD_Factory {

private:

  friend class PiDD;
    
  static PiDD_Node * create_node(transpose t, PiDD_Node * left, PiDD_Node * right){

    PiDD_Node node;
    node.left = left;
    node.right = right;
    node.t = t;
    node.size = 0;
    node.ref_count = 0;

    
    PiDD_Node * res = cache_lookup(&node);
    if (res == NULL){
      res = new PiDD_Node();
      assert(res != NULL);
      *res = node;
      increment_node(left);
      increment_node(right);
      if (t.a == t.b) {
	res -> size = t.a;
      } else {
	res -> size = left -> size + right -> size;
      }
      cache_insert(res);
    }
    
    return res;
  }

  static void clean_up_node_cache(){

    // Must be done in forward order because it may deallocate
    // children which will be ahead of parents in the recent
    // allocations.
    for (auto n : node_cache_recent){
      if (n -> ref_count == 0){
	n -> ref_count++;
	decrement_node(n);
      }
    }

    // Empty the list.
    node_cache_recent.clear();
  }
  
  static void increment_node(PiDD_Node * node){
    if (node != NULL && node -> t.a != node -> t.b){
      assert(node -> ref_count < UINT16_MAX);
      node -> ref_count++;
      //printf("Increment %p, %d\n", node, node -> ref_count);
    }
  }

  static void decrement_node(PiDD_Node * node){
    if (node != NULL && node -> t.a != node -> t.b){
      assert(node -> ref_count != 0);
      node -> ref_count--;
      //printf("ref_count = %d\n", node -> ref_count);
      if (node -> ref_count == 0){  // Don't delete zero and one nodes.
	decrement_node(node -> left);
	decrement_node(node -> right);
	cache_remove(node);
	delete node;
      }
    }
  }

  static string node_to_key(PiDD_Node * node){
    ostringstream ss;
    ss << node -> t.a << "|" << node -> t.b << "|" << node -> left << "|" << node -> right;
    return ss.str();
  }
  
  static void cache_remove(PiDD_Node * node){
    string key = node_to_key(node);
    node_cache.erase(key);
  }

  static void cache_insert(PiDD_Node * node){
    string key = node_to_key(node);
    assert(cache_lookup(node) == NULL);
    node_cache.insert(pair<string, PiDD_Node *>(key, node));
    node_cache_recent.push_back(node); // So we can deallocate if it isn't permanent.
  }

  static PiDD_Node * cache_lookup(PiDD_Node * node){
    string key = node_to_key(node);
    auto it = node_cache.find(key);
    if (it != node_cache.end())
      return it -> second;
    else
      return NULL;
  }


  
  static string transpose_op_key(PiDD_Node * node, const transpose &t){
    ostringstream ss;
    ss << "T" << t.a << "|" << t.b << "|" << node;
    return ss.str();
  }

  static string union_op_key(PiDD_Node * node1, PiDD_Node *node2){
    ostringstream ss;
    ss << "U" << node1 << "|" << node2;
    return ss.str();
  }

  static string intersection_op_key(PiDD_Node * node1, PiDD_Node *node2){
    ostringstream ss;
    ss << "I" << node1 << "|" << node2;
    return ss.str();
  } 


  static string product_op_key(PiDD_Node * node1, PiDD_Node *node2){
    ostringstream ss;
    //ss << "P" << node1 << "|" << node2;
    ss << "P" << ((long int)node1 ^ (long int)node2);
    return ss.str();
  } 

  // XXX - Op cache is not active, because it is slow / broken.
  static void op_cache_remove(map<string, PiDD_Node *> &memo, string key){
    memo.erase(key);
  }

  static void op_cache_insert(map<string, PiDD_Node *> &memo, string key, PiDD_Node * value){
    memo.insert(pair<string, PiDD_Node *>(key, value));
  }

  static PiDD_Node * op_cache_lookup(map<string, PiDD_Node *> &memo, string key){
    auto it = memo.find(key);
    if (it != memo.end())
      return it -> second;
    else
      return NULL;
  }
  
  // Static class, so no constructor.
  PiDD_Factory(){
  }

public:

  static uint64_t size(){
    return node_cache.size() + 2;
  }

  static PiDD make_empty(){
    return PiDD(zero);
  }

  static PiDD make_identity(){
    return PiDD(one);
  }
  
  static PiDD make_singleton(perm * pi){

    vector<transpose> ts;

    pi = copy_perm(pi);
    
    int i = pi -> n - 1;
    while (i >= 0){
      if (pi -> arrow[i] == i){
	i--;
      } else {

	ts.push_back((transpose){(uint8_t)i, (uint8_t)pi -> arrow[i]});
	// XXX - could make more efficient by compute inverse of pi.
	int i2 = 0;
	for ( ; i2 < i && pi -> arrow[i2] != i; i2++);  
	int temp = pi -> arrow[i];
	pi -> arrow[i] = pi -> arrow[i2];
	pi -> arrow[i2] = temp;
      }
    }

    reverse(ts.begin(), ts.end());
    PiDD_Node * prev = one;
    for (transpose t : ts){
      prev = create_node(t, zero, prev);
    }

    destroy_perm(pi);
    
    return PiDD(prev);
    
  }

  static PiDD make_vec(vector<perm *> pis){

    PiDD dd;
    for (perm * pi : pis){
      dd = dd | make_singleton(pi);
    }

    return dd;
    
  }

  static PiDD set_transpose(const PiDD& dd, const transpose& t){
    map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_transpose(dd.root, t, memo));
    clean_up_node_cache();
    return res;
  }

  static PiDD_Node * push_down(const transpose &t, PiDD_Node * new_left, PiDD_Node * new_right, map<string, PiDD_Node*> &memo){
    // XXX - Note, this seems to be implementing a special case of union
    // when unioning new_right with (t, [0], new_right).
    
    PiDD_Node * res = NULL;
    if (new_left == zero && new_left == one) {
      res = create_node(t, new_left, new_right);
    } else if (compare_transposes(new_left -> t, t) == 0) {
      //printf("new_left matches new_root\n");
      res = create_node(t, new_left -> left, set_union(new_left -> right, new_right, memo));
    } else if (compare_transposes(new_left -> t, t) == 1) {
      //printf("new_left is above new_root: %d %d, %d %d\n", new_left->t.a, new_left->t.b, t0.a, t0.b);
      res = create_node(new_left -> t, push_down(t, new_left -> left, new_right, memo), new_left -> right);
    } else {
      //printf("new_left is below new_root\n");
      res = create_node(t, new_left, new_right);
    }

    return res;
  }
  
  static PiDD_Node * set_transpose(PiDD_Node * root, const transpose& t, map<string, PiDD_Node *> &memo){

    // //printf("Start transpose\n");
    
    string key = transpose_op_key(root, t);    
    PiDD_Node * done = op_cache_lookup(memo, key);
    if (done != NULL)
      return done;

    PiDD_Node * res = NULL;

    uint8_t x = root -> t.a;
    uint8_t y = root -> t.b;
    uint8_t u = t.a;
    uint8_t v = t.b;
    
    if (root == zero) {
      //printf("Root zero\n");
      res = root;
    } else if (root == one) {
      //printf("Root one\n");
      res = create_node(t, zero, one);
    } else if (u > x) {
      //printf("Transpose above\n");
      res = create_node(t, zero, root);
    // } else if (x != u && x != v && y != u && y != v){
    //   printf("Transpose disjoint\n");
    //   res = create_node(root -> t, set_transpose(root -> left, t, memo), set_transpose(root -> right, t, memo));
    } else if (x == u && y == v){
      //printf("Transpose equal\n");
      if (root -> left == zero)
	res = root -> right;
      else
	res = set_union(set_transpose(root -> left, t, memo), root->right, memo);
        //create_node(t, root -> right, root -> left); // XXX - might cause variable ordering violation.
    } else {
      //printf("Transpose general\n");
      uint8_t up = u;
      uint8_t yp = y;

      if (y == u) {
	up = u;
	yp = v;
      } else if (y == v) {
	up = u;
	yp = u;
      } else if (x == u) {
	up = y;
	yp = y;
      }


      transpose t0 = (x > yp ? (transpose){x, yp} : (transpose){yp, x});
      transpose t1 = (up > v ? (transpose){up,v} : (transpose){v,up});
      assert(x != yp);
      assert(up != v);
      //assert(compare_transposes(t0,t1) == 1);
      //assert(compare_transposes(t0,t) == 1);

      // XXX - The paper's construction doesn't ensure the variable
      // ordering invariant.  We have to use a more general
      // construction here to ensure the invariant is maintained.  It
      // is much slower and harder to exclude memory leaks.
      
      //res = create_node(t0, set_transpose(root -> left, t, memo), set_transpose(root -> right, t1, memo));

      PiDD_Node * new_left = set_transpose(root -> left, t, memo);
      PiDD_Node * new_right = set_transpose(root -> right, t1, memo);

      // printf("top = (%d, %d), t = (%d, %d) ->  new_top = (%d, %d), left = (%d, %d), right = (%d, %d)\n",
      //  	     x, y, u, v, x, yp, u, v, up, v);

      // Push new_root down until it's left child's transpose is below t0.
      // XXX - not sure why it's necesssary to push more than one level, but it seems to be.
      // Appears to be giving correct invariants, and is much faster than literal implemenetation below.
      // Still has insufficient deallocation.
      res = push_down(t0, new_left, new_right, memo);

      // XXX - Literal implementation of construction.
      //res = set_union(new_left, set_transpose(new_right, t0, memo), memo);
    }

    op_cache_insert(memo, key, res);
    //assert(validate(res));
    //printf("End transpose\n");
    return res;
    
  }

  static int compare_transposes(transpose t1, transpose t2){

    if (t1.a == t2.a and t1.b == t2.b) 
      return 0;
    
    if (t1.a < t2.a || (t1.a == t2.a && t1.b > t2.b))
      return -1;
    
    return 1;
    
  }
  
  static PiDD set_union(const PiDD& dd1, const PiDD& dd2){
    map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_union(dd1.root, dd2.root, memo));
    clean_up_node_cache();
    return res;

  }

  static PiDD_Node * set_union(PiDD_Node * root1, PiDD_Node * root2, map<string, PiDD_Node *> &memo){

    PiDD_Node * res = NULL;

    //printf("Start union\n");
    string key = union_op_key(root1, root2);

    PiDD_Node * done = op_cache_lookup(memo, key);
    if (done != NULL)
      return done;    
    
    if (root1 == zero && root2 == zero) {
      res = zero;
    } else if ((root1 == zero || root1 == one) && (root2 == zero || root2 == one)) {
      res = one;
    } else if (root1 == zero) {
      res = root2;
    } else if (root2 == zero) {
      res = root1;
    } else if (root1 == one) {
      res = create_node(root2 -> t, set_union(root1, root2 -> left, memo), root2 -> right);
    } else if (root2 == one) {
      res = create_node(root1 -> t, set_union(root2, root1 -> left, memo), root1 -> right);
    } else if (compare_transposes(root1 -> t, root2 -> t) == 0) {
      res = create_node(root1 -> t,
			set_union(root1 -> left, root2 -> left, memo),
			set_union(root1 -> right, root2 -> right, memo));
    } else if (compare_transposes(root1 -> t, root2 -> t) == -1) {
      res = create_node(root2 -> t, set_union(root2 -> left, root1, memo), root2 -> right);	
    } else {
      res = create_node(root1 -> t, set_union(root1 -> left, root2, memo), root1 -> right);	
    }

    op_cache_insert(memo, key, res);
    //assert(validate(res));
    //printf("End union\n");
    return res;
  }
  
  static PiDD set_intersection(const PiDD& dd1, const PiDD& dd2){
    map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_intersection(dd1.root, dd2.root, memo));
    clean_up_node_cache();
    return res;
    
  }

  static PiDD_Node * set_intersection(PiDD_Node * root1, PiDD_Node * root2, map<string, PiDD_Node *> &memo){
    
    PiDD_Node * res = NULL;

    string key = intersection_op_key(root1, root2);
    
    PiDD_Node * done = op_cache_lookup(memo, key);
    if (done != NULL)
      return done;
    
    if (root1 == zero || root2 == zero){
      res = zero;
    } else if (root1 == one && root2 == one){
      res = one;
    } else if (root1 == one || root2 == one){
      PiDD_Node * curr = root1;
      if (root1 == one)
	curr = root2;
      while (curr != zero && curr != one) 
	curr = curr -> left;
      res = curr;
    } else if (compare_transposes(root1 -> t, root2 -> t) == 0) {
      PiDD_Node * new_left = set_intersection(root2 -> left, root2 -> left, memo);
      PiDD_Node * new_right = set_intersection(root2 -> right, root2 -> right, memo);
      if (new_right != zero) {
	res = create_node(root1 -> t, new_left, new_right);
      } else {
	res = new_left;
      }
    } else if (compare_transposes(root1 -> t, root2 -> t) == -1) {
      res = set_intersection(root1, root2 -> left, memo);
    } else {
      res = set_intersection(root1 -> left, root2, memo);
    }

    op_cache_insert(memo, key, res);
    return res;
    
  }

  static PiDD set_product(const PiDD& dd1, const PiDD& dd2){
    map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_product(dd1.root, dd2.root, memo));
    clean_up_node_cache();
    return res;
    
  }

  static PiDD_Node * set_product(PiDD_Node * root1, PiDD_Node * root2, map<string, PiDD_Node *> &memo){

    //printf("Start product\n");
    
    PiDD_Node * res = NULL;

    string key = product_op_key(root1, root2);
    
    PiDD_Node * done = op_cache_lookup(memo, key);
    if (done != NULL)
      return done;

    if (root2 == zero)
      res = zero;
    else if (root2 == one)
      res = root1;
    else {

      // XXX - Deallocation is broken by this nesting of operations,
      // because it only deallocates things that are eventually
      // connected to PiDD objects which are themselves deallocated.
      // I'm not sure how to cleanly fix this.  Perhaps by maintaing a
      // list of recently created nodes and then deallocating at the
      // the end of the call (otherwise it also interferes with the op
      // cache (which is currently broken and disabled)).
      res = set_union(set_product(root1, root2 -> left, memo),
		      set_transpose(set_product(root1, root2 -> right, memo), root2 -> t, memo),
		      memo);
    }

    op_cache_insert(memo, key, res);
    //assert(validate(res));
    //printf("End product\n");
    return res;
    
  }


  static vector<perm *> enlist(PiDD_Node * root, int n){

    if (root == zero){
      vector <perm *> pis;
      return pis;
    } else if (root == one){
      vector <perm *> pis;
      perm * pi = create_perm_identity(n);
      pis.push_back(pi);
      return pis;
    } else {
      transpose t = root -> t;
      assert(t.a <= n && t.b < n);
      vector<perm *> left_pis = enlist(root -> left, n);
      vector<perm *> right_pis = enlist(root -> right, n);

      
      for (auto pi : right_pis){
	int temp = pi -> arrow[t.a];
	pi -> arrow[t.a] = pi -> arrow[t.b];
	pi -> arrow[t.b] = temp;
      }
      left_pis.insert( left_pis.end(), right_pis.begin(), right_pis.end() );
      
      return left_pis;
    }
  }
  
  
  static bool validate(PiDD_Node * root){

    if (root -> t.a == root -> t.b){
      // A 0 or 1 leaf.
      if (root -> t.a != 0 && root -> t.a != 1) {
	printf("Leaf node with t not 0,1: %d\n", root -> t.a); 
	return false;
      } else if (root -> left != NULL || root -> right != NULL) {
	printf("Leaf node with non-NULL children\n");
	return false;
      } else if (root -> size != root -> t.a) {
	printf("Leaf node with inconsistent size: %lu != %d\n", root -> size, root -> t.a);
	return false;
      } else if (root -> ref_count != 0) {
	printf("Leaf node with non-zero ref_count: %d\n", root -> ref_count);
	return false;
      }
    } else {
      if (root -> t.a <= root -> t.b) {
	printf("Internal node with transpose out of order: %d, %d\n", root -> t.a, root -> t.b);
	return false;
      } else if (root -> left == NULL || root -> right == NULL) {
	printf("Internal node with NULL children.\n");
	return false;
      } else if (!validate(root -> left) || !validate(root -> right)) {
	return false;
      } else if (root -> left != zero && root -> left != one
		 && compare_transposes(root -> t, root -> left -> t) != 1){
	printf("Transposes out of order.\n");
	printf("root: %d %d, root -> left: %d %d\n", root -> t.a, root -> t.b,
	       root -> left -> t.a, root -> left -> t.b);
	
	return false;
      } else if (root -> right != zero && root -> right != one && root -> t.a <= root -> right -> t.a){
	printf("Transposes out of order.\n");
	printf("root: %d %d, root -> right: %d %d\n", root -> t.a, root -> t.b,
	       root -> right -> t.a, root -> right -> t.b);
	return false;
      } else if (root -> size != root -> left -> size + root -> right -> size) {
	printf("Internal node with size inconsistent with children: %lu != %lu + %lu\n",
	       root -> size, root -> left -> size, root -> right -> size);
	return false;
      // } else if (root -> ref_count < 1) {
      // 	printf("Internal node with nonpositive ref_count: %d\n", root -> ref_count);
      // 	return false;
      } else if (root -> ref_count > PiDD_count + PiDD_Factory::size() - 1) {
	printf("Ref_count higher than number of things that could ref it! %d vs. %lu\n",
	       root -> ref_count,  PiDD_count + PiDD_Factory::size() - 1);
	return false;
      }
      // XXX - can't robustly check ref_count.
    }
    return true;    
  }
};



PiDD::PiDD()
  : root(zero)
{
  PiDD_count++;
  PiDD_Factory::increment_node(root);
  //assert(validate());
}

PiDD::PiDD(PiDD_Node * root)
  : root(root)
{
  //printf("Creating PiDD %p\n", root);
  PiDD_count++;
  PiDD_Factory::increment_node(root);
  //assert(validate());
}
  
PiDD& PiDD::operator=(const PiDD& other){

  if (root != other.root) {
    PiDD_Factory::decrement_node(root);
    root = other.root;
    PiDD_Factory::increment_node(root);
  }
  
  return *this;
}

PiDD::~PiDD(){
  PiDD_count--;
  //printf("Destroying PiDD %p\n", root);
  PiDD_Factory::decrement_node(root); 
}



bool PiDD::operator==(const PiDD& dd) const{
  return this -> root == dd.root;
}

// Union operator.
PiDD PiDD::operator|(const PiDD& other) const {
  return PiDD_Factory::set_union(*this, other);
}

// Intersection operator.
PiDD PiDD::operator&(const PiDD& other) const {
  return PiDD_Factory::set_intersection(*this, other);
}

PiDD PiDD::operator*(const transpose t) const {
  transpose t2 = t;
  if (t.a < t.b) {
    t2.a = t.b;
    t2.b = t.a;
  }
  return PiDD_Factory::set_transpose(*this, t2);
}

PiDD PiDD::operator*(const PiDD& other) const {
  return PiDD_Factory::set_product(*this, other);
}
  
uint64_t PiDD::size() const {
  return root -> size;
}

bool PiDD::is_empty() const {
  return this -> root == zero;
}

vector<perm *> PiDD::enlist(int dim) const{
  return PiDD_Factory::enlist(root, dim);
}

void PiDD::print_perms() const{

  int dim = 0;
  if (root != zero && root != one)
    dim = root -> t.a + 1;

  vector<perm *> pis = enlist(dim);
  
  assert(size() == pis.size());
  printf("size = %lu, node_cache = %lu, dim = %d\n", size(), PiDD_Factory::size(), dim);
  printf("[\n");
  for (auto pi : pis){
    print_perm_cycle(pi);
    destroy_perm(pi);
  }
  printf("]\n");
  
}

bool PiDD::validate() const {
  return true; // XXX - validate is slow!
  //return PiDD_Factory::validate(root);
}
