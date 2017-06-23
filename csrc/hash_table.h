#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#define HASH_FREE 0
#define HASH_DELETED 1
#define HASH_OCCUPIED 2

typedef union {

  void * void_ptr_val;
  double d_val;
  int i_val;

} hash_val;

typedef unsigned int (*hash_func)(hash_val k);
typedef void (*destroy_func)(hash_val p);
typedef hash_val (*copy_func)(hash_val p);
typedef int (*equals_func)(hash_val k1, hash_val k2);
typedef void (*print_func)(hash_val x);

typedef struct _hash_table_entry{

  hash_val key;
  hash_val value;
  int flag;

} hash_table_entry;

typedef struct _hash_table{

  int capacity;
  int size;
  int num_deleted;
  hash_table_entry * entries;
  hash_func h1;
  hash_func h2;
  equals_func eq;

} hash_table; 

// Constructor. Creates an empty hash table of size capacity.
hash_table * create_hash_table(int capacity, hash_func h1, hash_func h2, equals_func eq);

// Copy constructor.
hash_table * copy_hash_table(hash_table * t, double cap_factor);

// Copy constructor.  Copies all entries.
hash_table * copy_hash_table_deep(hash_table * t, double cap_factor, copy_func copy_key, copy_func copy_value);

// Destructor.  Just Deallocates the table, not all entries.
void destroy_hash_table(hash_table * t);

// Destructor.  Destroys everything.
void destroy_hash_table_deep(hash_table *t, destroy_func destroy_key, destroy_func destroy_value);

// Inserts (key, value) in hash_table.
void insert_in_hash_table(hash_table * t, hash_val key, hash_val value);

// Search for key in hash_table, returns true if found, false otherwise.
// Associated value is returned to value_ptr.
int search_in_hash_table(hash_table * t, hash_val key, hash_val ** value_ptr);

// Same as search, but also removes entry from hash table.
int delete_in_hash_table(hash_table * t, hash_val key, hash_val ** value_ptr);

void print_compact_hash_table(hash_table * t, print_func print_key, print_func print_value);

void print_hash_table(hash_table * t, print_func print_key, print_func print_value);

// XXX - should write an apply function.

void noop_helper(hash_val x);

void print_helper(hash_val x);

int equals_helper(hash_val x, hash_val y);

unsigned int identity_helper(hash_val x);

#endif
