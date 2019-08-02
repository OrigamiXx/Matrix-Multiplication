#include "PiDD.hpp"

 
using namespace std;

#include <cstdio> 
#include <string>
#include <cstdlib>

int main(int argc, char * argv[]){ 
   
          
  int n = 10;               
  printf("PiDD Tester\n");           
   
  printf("Empty\n");     
  PiDD dd = PiDD_Factory::make_empty();        
  dd.print_perms();
  
  printf("Identity\n");
  perm * pi = create_perm_identity(n);
  PiDD dd2 = PiDD_Factory::make_singleton(pi);
  dd2.print_perms();
  
  printf("Empty | Identity\n");
  PiDD dd3 = dd | dd2;
  dd3.print_perms();

  printf("Empty & Identity\n");
  PiDD dd4 = dd & dd2;
  dd4.print_perms();
 
  next_perm(pi); 
 
  printf("First transpose\n");
  PiDD dd5 = PiDD_Factory::make_singleton(pi);
  dd5.print_perms();  

  printf("Identity | First Transpose\n");
  PiDD dd6 = dd2 | dd5;
  dd6.print_perms(); 

  printf("Identity & First Transpose\n"); 
  PiDD dd7 = dd2 & dd5;
  dd7.print_perms(); 
 
  printf("Apply transpose to identity\n");
  PiDD dd9 = dd2 * (transpose){1,2}; 
  dd9.print_perms();
  printf("Apply transpose again\n");
  PiDD dd10= dd9 * (transpose){1,2};
  dd10.print_perms();
  
  printf("PiDD count = %d\n", PiDD_count);  
  
  /* pi = create_perm_identity(n);  */
  /* int count = 0; */
  /* while (true){ */
  /*   //print_perm_cycle(pi);  */
  /*   count++; */
  /*   PiDD dd8 = PiDD_Factory::make_singleton(pi); */
  /*   dd = dd | dd8; */
  /*   //printf("count = %d\n", count); */
  /*   //printf("size(dd) = %u\n", dd.size()); */
  /*   /\* printf("Cache size = %u\n", PiDD_Factory::size()); *\/ */
  /*   /\* printf("PiDD count = %d\n", PiDD_count); *\/ */
  /*   if (is_last_perm(pi))  */
  /*     break; */
  /*   next_perm(pi); */
  /*   if (count % 1000 == 0) { */
  /*     printf("\rcount = %d, cache = %u", count, PiDD_Factory::size()); */
  /*     fflush(stdout);  */
  /*   } */
  /* }      */
  
  /* dd = dd & dd; */
  /* printf("self intersect, size(dd) = %u\n", dd.size());  */
  /* printf("Cache sizes = %u\n", PiDD_Factory::size()); */
  /* printf("PiDD count = %d\n", PiDD_count);     */    
  
  dd = PiDD_Factory::make_identity(); 
  printf("Cache size = %lu\n", PiDD_Factory::size());    
  printf("PiDD count = %d\n", PiDD_count);  

  /*
  for (int a = 1; a < (uint8_t)n; a++){    
    for (int b = a - 1; b >=0; b--){
      printf("a, b = %d, %d\n", a, b);
      PiDD dd9 = PiDD_Factory::make_identity();  
      dd9 = dd9 | (dd9 * (transpose){(uint8_t)a, (uint8_t)b});  
      printf("Product term\n");    
      dd9.print_perms(); 
      dd = dd9 * dd;   
      printf("Cumulative\n"); 
      //dd.print_perms();   
    } 
  }
  
  printf("max product, size(dd) = %lu\n", dd.size());   
  printf("Cache size = %lu\n", PiDD_Factory::size());
  printf("PiDD count = %d\n", PiDD_count);        
  */
  
  // Permutation Networks Example from [Minato11].
  PiDD id = PiDD_Factory::make_identity();
  PiDD P = id;
  PiDD T = id;
  for (int a = 1; a < n; a++){
    int b = a - 1;
    T = T | (id * (transpose){(uint8_t) a, (uint8_t) b});
  }

  T.print_perms();
  
  for (int i = 0; i <= n * (n - 1) / 2; i++){
    P = P * T;
    printf("i = %d, cache size = %lu\n", i, PiDD_Factory::size()); 
  }

  printf("Permutation Network, size(dd) = %lu\n", P.size());   
  printf("Cache size = %lu\n", PiDD_Factory::size()); 
  printf("PiDD count = %d\n", PiDD_count);
   
  return 0;
}
