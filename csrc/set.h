#ifndef __SET_H__
#define __SET_H__

typedef int set;

set create_empty_set();
set create_one_element_set(int element);
set set_union(set a, set b);
set set_intersect(set a, set b);
int is_empty_set(set a);
int is_membership(int element, set a);
set set_complement(int universe, set a);
void print_set(set a);


#endif
