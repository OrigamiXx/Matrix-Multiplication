#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash_table.h"
#include "constants.h"



// Returns entry of key in t, on ith probe.  Uses double hashing.
unsigned int hash(hash_table * t, void * key, int i) {

  return (t -> h1(key) + i * t -> h2(key)) % t -> capacity;

}

// Constructor. Creates an empty hash table of size capacity.
hash_table * create_hash_table(int capacity, hash_func h1, hash_func h2, equals_func eq){

  hash_table * t = (hash_table *)malloc(sizeof(hash_table));

  assert(t != NULL);
  t -> capacity = capacity;
  t -> h1 = h1;
  t -> h2 = h2;
  t -> eq = eq;

  t -> num_deleted = 0;
  t -> size = 0;
  t -> entries = (hash_table_entry *)malloc(sizeof(hash_table_entry)*capacity);
  
  assert(t -> entries != NULL);

  bzero(t -> entries,sizeof(hash_table_entry)*capacity);

  return t;
}

void * no_copy(void * x){
  return x;
}


// Copy constructor.
hash_table * copy_hash_table(hash_table * t, double cap_factor) {

  return copy_hash_table_deep(t,cap_factor,no_copy,no_copy);

}

// Copy constructor.  Copies all entries.
hash_table * copy_hash_table_deep(hash_table * t1, double cap_factor, copy_func copy_key, copy_func copy_value) {

  int new_capacity = (int)((t1 -> capacity) * cap_factor);
  hash_table * t2 = create_hash_table(new_capacity,t1 -> h1, t1 -> h2, t1 -> eq);

  int i;
  for (i = 0; i < t1 -> capacity; i++){
    
    hash_table_entry * e = &(t1 -> entries[i]);

    if (e -> flag == HASH_OCCUPIED)
      insert_in_hash_table(t2,copy_key(e -> key), copy_value(e -> value));
    
  }

  return t2;

}



void rehash(hash_table * t1, double cap_factor) {

  hash_table * t2 = copy_hash_table(t1,cap_factor);

  hash_table_entry * old_entries = t1 -> entries;

  *t1 = *t2;
  t2 -> entries = old_entries;
  destroy_hash_table(t2);
  
}


// Destructor.  Just Deallocates the table, not all entries.
void destroy_hash_table(hash_table * t) {

  free(t -> entries);
  free(t);

}

// Destructor.  Destroys everything.
void destroy_hash_table_deep(hash_table *t, destroy_func destroy_key, destroy_func destroy_value) {

  int i;
  for (i = 0; i < t -> capacity; i++){
    
    hash_table_entry * e = &(t -> entries[i]);

    if (e -> flag == HASH_OCCUPIED){
      destroy_key(e -> key);
      destroy_value(e -> value);
    }

  }

  free(t -> entries);
  free(t);

}

// Inserts (key, value) in hash_table.
void insert_in_hash_table(hash_table * t, void * key, void * value) {

  int i = 0;
  unsigned int index;

  do {
    index = hash(t,key,i);
    if (t -> entries[index].flag != HASH_OCCUPIED) {
      t -> entries[index].key = key;
      t -> entries[index].value = value;
      t -> entries[index].flag = HASH_OCCUPIED;
      t -> size++;

      if (t -> size > (t -> capacity / 2.0)) {
	rehash(t,2.0);
      }
     
      return;
    }
    i++;
  } while (i != t -> capacity);

  assert(false);

}

// Search for key in hash_table, returns true if found, false otherwise.
// Associated value is returned to value_ptr.
int search_in_hash_table(hash_table * t, void * key, void ** value_ptr) {

  int i = 0;
  unsigned int index;

  do {
    index = hash(t,key,i);
    if (t -> entries[index].flag == HASH_OCCUPIED && t -> eq(t -> entries[index].key,key)) {
      (*value_ptr) = t -> entries[index].value;
      return true;
    }
    i++;
  } while (i != t -> capacity && t -> entries[index].flag != HASH_FREE);
 
  *value_ptr = NULL;
  return false;
}

// Same as search, but also removes entry from hash table.
int delete_in_hash_table(hash_table * t, void * key, void ** value_ptr) {

  int i = 0;
  unsigned int index;

  do {
    index = hash(t,key,i);
    if (t -> entries[index].flag == HASH_OCCUPIED && t -> eq(t -> entries[index].key,key)) {
      (*value_ptr) = t -> entries[index].value;
      t -> entries[index].flag = HASH_DELETED;
      t -> num_deleted++;
      t -> size--;

      if (t -> size < t -> capacity / 8.0)
	rehash(t,0.5);
      else if ((t -> num_deleted + t -> size) > t -> capacity / 2.0)
	rehash(t,1.0);
      
      return true;
    }
    i++;
  } while (i != t -> capacity && t -> entries[index].flag != HASH_FREE);

  *value_ptr = NULL; 
  return false;

}



void print_compact_hash_table(hash_table * t, print_func print_key, print_func print_value){

  printf("capacity = %d, size = %d, num_deleted = %d\n",t -> capacity, t -> size, t -> num_deleted);

  int i;
  for (i = 0; i < t -> capacity; i++){
    
    hash_table_entry * e = &(t -> entries[i]);

    if (e -> flag == HASH_OCCUPIED) {
      printf("%8d: ",i);
      print_key(e -> key);
      printf(", ");
      print_value(e -> value);
      printf("\n");
    }

  }

}

void print_hash_table(hash_table * t, print_func print_key, print_func print_value) {

  printf("capacity = %d, size = %d, num_deleted = %d\n",t -> capacity, t -> size, t -> num_deleted);

  int i;
  for (i = 0; i < t -> capacity; i++){
    
    hash_table_entry * e = &(t -> entries[i]);

    printf("%8d: ",i);
    if (e -> flag == HASH_OCCUPIED) {
      print_key(e -> key);
      printf(", ");
      print_value(e -> value);
      printf("\n");
    } else if (e -> flag == HASH_FREE) {
      printf("FREE\n");
    } else {
      printf("DELETED\n");
    }

  }

}
