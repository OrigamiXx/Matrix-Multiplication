#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__


#define true 1
#define false 0

#ifdef DEBUG3
#define DEBUG2
#endif

#ifdef DEBUG2
#define DEBUG1
#endif

#ifdef DEBUG1
#define DEBUG1_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG1_PRINTF(...) (void)0
#endif

#ifdef DEBUG2
#define DEBUG2_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG2_PRINTF(...) (void)0
#endif

#ifdef DEBUG3
#define DEBUG3_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG3_PRINTF(...) (void)0
#endif

auto inline MIN(auto A, auto B){
  if (A < B)
    return A;
  else
    return B;
}

auto inline MAX(auto A, auto B){
  if (A > B)
    return A;
  else
    return B;
}
//#define MIN(A,B) (((A) < (B)) ? (A) : (B))
//#define MAX(A,B) (((A) > (B)) ? (A) : (B))

#endif
