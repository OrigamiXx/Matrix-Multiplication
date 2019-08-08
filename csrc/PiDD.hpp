/* 
 *  Class for representing sets of permutations.  Based on "piDD: A
 *  New Decision Diagram for Efficient Problem Solving in Permutation
 *  Space.", Shin-ichi Minato.
 *
 *  This C++ implementation is based on Akriti's python3
 *  implementation.
 *
 *  XXX - This file should be refactored to put its contents in a .cpp file.
 *
 *  Summer 2019.
 */

#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include "permutation.h"
#include <vector>
#include <cassert>

using namespace std;

// Data structure for transpose.  
typedef struct _transpose {
  
  uint8_t a;  
  uint8_t b;

} transpose;

// Data structure for the nodes of PiDDs.
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

  // Used to prevent double recursion for linear traversal of a PiDD.
  int magic;

} PiDD_Node;

// Global used with magic field of nodes to prevent double recursion.
int magic_counter = 0;

class PiDD_Factory;

/*
 * Class for representing sets of permutations.  Very light weight,
 * maintains a pointer to a node the factory has generated.
 */
class PiDD {
    
  private:
 
  friend class PiDD_Factory;

  PiDD_Node * root;

  // Real constructor
  PiDD(PiDD_Node * root);
  
public:

  // Create empty PiDD.
  PiDD();
  // Create copy of other PiDD.
  PiDD(const PiDD &other) : PiDD(other.root) {}
  // Assign to contents of other.
  PiDD& operator=(const PiDD& other);

  // Destructor.  Deallocates nodes in factory that are no longer
  // referenced.
  ~PiDD();

  // Determines (in)equality.  O(1).
  bool operator==(const PiDD& dd) const;
  bool operator!=(const PiDD& dd) const; 

  // Union operator.
  PiDD operator|(const PiDD& other) const;

  // Intersection operator.
  PiDD operator&(const PiDD& other) const;
  
  // Cartesian Product.
  PiDD operator*(const PiDD& other) const;
  
  // Apply transpose from right.
  PiDD operator*(const transpose t) const;

  // Returns number of permutations.
  uint64_t size() const;
  // Returns number of nodes.
  uint64_t node_size() const;
  // Returns dimension, i.e. larger variable moved.
  int dim() const;
  // Return true iff size is 0.
  bool is_empty() const;

  // Returns a vector containing all permutations.  Must deallocate
  // individually.
  vector<perm *> enlist() const;
  // Print stats about the set.
  void print_stats() const;
  // Prints stats and all perms in the set.
  void print_perms() const;
  // Returns true iff the data structure is consistent with its invariants.  For debugging.
  bool validate() const;
  
};


/*
 * Static class for creating PiDDs and performing operations.  Lots of
 * globals, because static initialization of class variables is
 * confusing.
 */

// Total number of PiDDs, used for memory debugging.
unsigned int PiDD_count = 0;
// Globals representing the leaf nodes in PiDDs.  Is only every one
// copy of each.  They do not appear in the node cache.
PiDD_Node zero_node = (PiDD_Node){NULL, NULL, (transpose){0,0}, 0, 0, 0};
PiDD_Node * zero = &zero_node;
PiDD_Node one_node = (PiDD_Node){NULL, NULL, (transpose){1,1}, 1, 0, 0};
PiDD_Node * one = &one_node;
// Map of the nodes being remembered using a hash of their children
// and label.
unordered_map<string, PiDD_Node *> node_cache; 
// A list of the nodes that have been recently allocated, but may not
// be permanently referenced by a PiDD.  Use to garbage collect.
vector<PiDD_Node *> node_cache_recent;

class PiDD_Factory {

private:

  friend class PiDD;

  // Creates a new node with the given content.  Ensure that nodes are
  // never duplicated.  Adds to node_cache if not existing, otherwise
  // returns the address of the pre-existing node.
  static PiDD_Node * create_node(transpose t, PiDD_Node * left, PiDD_Node * right){

    // Initialize.
    PiDD_Node node;
    node.left = left;
    node.right = right;
    node.t = t;
    node.size = 0;
    node.ref_count = 0;
    node.magic = 0;

    // Check to see if it already exists.
    PiDD_Node * res = cache_lookup(&node);
    if (res == NULL){
      // Create when it doesn't exist.
      res = new PiDD_Node();
      assert(res != NULL);
      *res = node;
      // Update reference counts for children.
      increment_node(left);
      increment_node(right);
      if (t.a == t.b) {
	res -> size = t.a;
      } else {
	res -> size = left -> size + right -> size;
      }
      // Add to cache.
      cache_insert(res);
    }
    
    return res;
  }

  // Deallocates nodes that were recently created, but are no longer
  // referenced.  Should be called after recursive operations to
  // prevent memory leaks.
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

  // Increments the reference count on nodes, indicating that other
  // objects know about them, and so they shouldn't be deallocated.
  static void increment_node(PiDD_Node * node){
    if (node != NULL && node -> t.a != node -> t.b){
      assert(node -> ref_count < UINT16_MAX);
      node -> ref_count++;
    }
  }

  // Decrements the reference count on nodes.  The node is permanently
  // deallocated if it's count becomes 0.  This has no effect on the
  // leaf nodes zero or one.
  static void decrement_node(PiDD_Node * node){
    if (node != NULL && node -> t.a != node -> t.b){ // Don't delete zero and one nodes.
      assert(node -> ref_count != 0); 
      node -> ref_count--;
      if (node -> ref_count == 0){
	// Delete this node.
	// Recursively decrement children.
	decrement_node(node -> left);
	decrement_node(node -> right);
	// Remove from the cache.
	cache_remove(node);
	// Deallocate.
	delete node;
      }
    }
  }

  // Helper functions to maintain the hash table caching the nodes.
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


  // Helper functions to maintain the hash table caching the
  // operations being performed on a PiDD.  The hash functions all
  // concatenate the data of a node and operation into a string which
  // keys the table.
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
    ss << "P" << node1 << "|" << node2;
    return ss.str();
  } 

  static void op_cache_remove(unordered_map<string, PiDD_Node *> &memo, string key){
    memo.erase(key);
  }

  static void op_cache_insert(unordered_map<string, PiDD_Node *> &memo, string key, PiDD_Node * value){
    memo.insert(pair<string, PiDD_Node *>(key, value));
  }

  static PiDD_Node * op_cache_lookup(unordered_map<string, PiDD_Node *> &memo, string key){
    auto it = memo.find(key);
    if (it != memo.end())
      return it -> second;
    else
      return NULL;
  }
  
  // Static class, no constructor.
  PiDD_Factory(){}

public:

  // ================ Accessors =========================
  
  // Returns the total number of nodes actively being referenced.
  static uint64_t size(){
    return node_cache.size() + 2;
  }

  // Returns the node size at a PiDD with the given root ignoring
  // nodes matching magic.
  static uint64_t node_size(PiDD_Node * root, int magic){

    if (root -> magic == magic)
      return 0;

    root -> magic = magic;

    if (root -> t.a == root -> t.b)
      return 1;

    return 1 + node_size(root -> left, magic) + node_size(root -> right, magic);

  }

  // ================ Constructors =========================
  
  // Returns a PiDD representing the empty set.
  static PiDD make_empty(){
    return PiDD(zero);
  }

  // Returns a PiDD representing the singleton set with the identity
  // permutation.
  static PiDD make_identity(){
    return PiDD(one);
  }

  // Returns a PiDD representing the singleton set with the given
  // permutation.
  static PiDD make_singleton(perm * pi){

    // Use algorithm implicit in [Minato, Thm 1].
    
    vector<transpose> ts;
    
    pi = invert_perm(copy_perm(pi));
    
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

  // Returns a PiDD representing the set of permutations in the given
  // vector.
  static PiDD make_vec(vector<perm *> pis){

    PiDD dd;
    for (perm * pi : pis)
      dd = dd | make_singleton(pi);
  
    return dd;
    
  }

  // ================ Utility Functions ===================
  
  // Compares the relative transposes of nodes in a PiDD.  Returns -1
  // if the t1 should appear lower than t2, 0 if they are equal and 1
  // if t1 should appear above t2.
  static int compare_transposes(transpose t1, transpose t2){

    if (t1.a == t2.a and t1.b == t2.b) 
      return 0;
    
    if (t1.a < t2.a || (t1.a == t2.a && t1.b > t2.b))
      return -1;
    
    return 1;
    
  }
  
  // Helper function that reorders nodes in PiDD to maintain data
  // structure invariant.
  static PiDD_Node * push_down(const transpose &t, PiDD_Node * new_left,
			       PiDD_Node * new_right, unordered_map<string, PiDD_Node*> &memo){
    // XXX - Note, this seems to be implementing a special case of union
    // when unioning new_right with (t, [0], new_right).
    
    PiDD_Node * res = NULL;
    if (new_left == zero && new_left == one) {
      res = create_node(t, new_left, new_right);
    } else if (compare_transposes(new_left -> t, t) == 0) {
      res = create_node(t, new_left -> left, set_union(new_left -> right, new_right, memo));
    } else if (compare_transposes(new_left -> t, t) == 1) {
      res = create_node(new_left -> t, push_down(t, new_left -> left, new_right, memo), new_left -> right);
    } else {
      res = create_node(t, new_left, new_right);
    }

    return res;
  }

  // ================ PiDD Operations =========================
  
  // Returns the PiDD that result from transposing dd by t.
  static PiDD set_transpose(const PiDD& dd, const transpose& t){
    unordered_map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_transpose(dd.root, t, memo));
    clean_up_node_cache();
    return res;
  }

  // Recursive helper function to help compute transpose.  It may not
  // fully deallocate, need to call clean_up_node_cache() afterwards.
  static PiDD_Node * set_transpose(PiDD_Node * root, const transpose& t,
				   unordered_map<string, PiDD_Node *> &memo){

    // Implement a variant of [Minato, Sec 4.2].
    
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
      res = root;
    } else if (root == one) {
      res = create_node(t, zero, one);
    } else if (u > x) {
      res = create_node(t, zero, root);
    } else if (x == u && y == v){
      if (root -> left == zero)
	res = root -> right;
      else
	res = set_union(set_transpose(root -> left, t, memo), root->right, memo);
    } else {

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

      PiDD_Node * new_left = set_transpose(root -> left, t, memo);
      PiDD_Node * new_right = set_transpose(root -> right, t1, memo);

      // Push new_root down until it's left child's transpose is below t0.
      res = push_down(t0, new_left, new_right, memo);

      // XXX - The paper's construction doesn't ensure the variable
      // ordering invariant.  We considered using the general
      // construction here to ensure the invariant is maintained.  It
      // is much slower.  We resolve this by above using push_down to
      // re-establish the variable ordering invariant, when the new
      // left child of a node would have a label that is too large.

      //res = create_node(t0, new_left, new_right);  // Paper
      //res = set_union(new_left, set_transpose(new_right, t0, memo), memo); // General.
    }

    op_cache_insert(memo, key, res);

    return res;
    
  }


  // Returns the PiDD that result from unioning dd1 with dd2.
  static PiDD set_union(const PiDD& dd1, const PiDD& dd2){
    unordered_map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_union(dd1.root, dd2.root, memo));
    clean_up_node_cache();
    return res;

  }

  // Recursive helper function to help compute union.  It may not
  // fully deallocate, need to call clean_up_node_cache() afterwards.
  static PiDD_Node * set_union(PiDD_Node * root1, PiDD_Node * root2,
			       unordered_map<string, PiDD_Node *> &memo){

    // Implement a variant of [Minato, Sec 4.1].
    
    PiDD_Node * res = NULL;

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

    return res;
  }

  // Returns the PiDD that result from intersecting dd1 with dd2.
  static PiDD set_intersection(const PiDD& dd1, const PiDD& dd2){
    unordered_map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_intersection(dd1.root, dd2.root, memo));
    clean_up_node_cache();
    return res;
    
  }

  // Recursive helper function to help compute intersection.  It may
  // not fully deallocate, need to call clean_up_node_cache()
  // afterwards.
  static PiDD_Node * set_intersection(PiDD_Node * root1, PiDD_Node * root2,
				      unordered_map<string, PiDD_Node *> &memo){

    // Implement a variant of [Minato, Sec 4.1].
    
    PiDD_Node * res = NULL;

    string key = intersection_op_key(root1, root2);
    
    PiDD_Node * done = op_cache_lookup(memo, key);
    if (done != NULL)
      return done;
    
    if (root1 == zero || root2 == zero){
      res = zero;
    } else if (root1 == one || root2 == one){
      PiDD_Node * curr = root1;
      if (root1 == one)
	curr = root2;
      while (curr != zero && curr != one) 
	curr = curr -> left;
      res = curr;
    } else if (compare_transposes(root1 -> t, root2 -> t) == 0) {
      PiDD_Node * new_left = set_intersection(root1 -> left, root2 -> left, memo);
      PiDD_Node * new_right = set_intersection(root1 -> right, root2 -> right, memo);
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
  
  // Returns the PiDD that result from product dd1 with dd2.
  static PiDD set_product(const PiDD& dd1, const PiDD& dd2){
    unordered_map<string, PiDD_Node *> memo;
    PiDD res = PiDD(set_product(dd1.root, dd2.root, memo));
    clean_up_node_cache();
    return res;
    
  }

  // Recursive helper function to help compute Cartesian product.  It
  // may not fully deallocate, need to call clean_up_node_cache()
  // afterwards.
  static PiDD_Node * set_product(PiDD_Node * root1, PiDD_Node * root2,
				 unordered_map<string, PiDD_Node *> &memo){

    // Implement a variant of [Minato, Sec 4.3].
        
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
      res = set_union(set_product(root1, root2 -> left, memo),
      		      set_transpose(set_product(root1, root2 -> right, memo), root2 -> t, memo),
      		      memo);
    }

    op_cache_insert(memo, key, res);

    return res;
    
  }

  // Returns a vector of the permutations store in the PiDD assuming
  // the size of the universe is n.
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
  
  // Recursively validate the invariants of the PiDD.  Extensive, but
  // not exhaustive.
  static bool validate(PiDD_Node * root, int magic){

    if (root -> magic == magic)
      return true;

    root -> magic = magic;
    
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
      // Internal node.
      if (root -> t.a <= root -> t.b) {
	printf("Internal node with transpose out of order: %d, %d\n", root -> t.a, root -> t.b);
	return false;
      } else if (root -> left == NULL || root -> right == NULL) {
	printf("Internal node with NULL children.\n");
	return false;
      } else if (!validate(root -> left, magic) || !validate(root -> right, magic)) {
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
      } else if (root -> ref_count > PiDD_count + PiDD_Factory::size() - 1) {
	// XXX - can't robustly check ref_count is correct.
	printf("Ref_count higher than number of things that could ref it! %d vs. %lu\n",
	       root -> ref_count,  PiDD_count + PiDD_Factory::size() - 1);
	return false;
      }
      
    }
    return true;    
  }
};

// =================== Implementation of PiDD =================

// Largely a wrapper for the factory's functionality.

PiDD::PiDD() : PiDD(zero) {}


PiDD::PiDD(PiDD_Node * root)
  : root(root)
{
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
  PiDD_Factory::decrement_node(root); 
}


bool PiDD::operator==(const PiDD& dd) const{
  return this -> root == dd.root;
}


bool PiDD::operator!=(const PiDD& dd) const{
  return this -> root != dd.root;
}


PiDD PiDD::operator|(const PiDD& other) const {
  return PiDD_Factory::set_union(*this, other);
}


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


uint64_t PiDD::node_size() const {
  return PiDD_Factory::node_size(root, ++magic_counter);
}


int PiDD::dim() const{

  int dim = 0;
  if (root != zero && root != one)
    dim = root -> t.a + 1;
  return dim;
  
}


bool PiDD::is_empty() const {
  return this -> root == zero;
}

vector<perm *> PiDD::enlist() const{
  return PiDD_Factory::enlist(root, dim());
}


void PiDD::print_stats() const{
  
  printf("size = %lu, node_size = %lu, node_cache = %lu, dim = %d\n",
	 size(), node_size(), PiDD_Factory::size(), dim());
}


void PiDD::print_perms() const{
  
  vector<perm *> pis = enlist();
  
  assert(size() == pis.size());
  print_stats();
  printf("[\n");
  for (auto pi : pis){
    print_perm_cycle(pi);
    destroy_perm(pi);
  }
  printf("]\n");
  
}


bool PiDD::validate() const {
  return PiDD_Factory::validate(root, ++magic_counter);
}
