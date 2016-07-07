// macros to simplify use of multi-page KMVs
// Karen Devine, March 2010

#ifndef _BLOCKMACROS_HPP
#define _BLOCKMACROS_HPP

// macros to loop over blocks when reduce multivalues may span more than 1 block
// use CHECK_FOR_BLOCKS initially to get # of blocks in the multivalue
// enclose code for each block between BEGIN_BLOCK_LOOP and END_BLOCK_LOOP
// NOTE: DO NOT put a semicolon afer these macros

#define CHECK_FOR_BLOCKS(multivalue, valuebytes, nvalues, totalnvalues)  \
  int macro_nblocks = 1; \
  totalnvalues = nvalues; \
  MapReduce *macro_mr = NULL; \
  if (!(multivalue)) { \
    macro_mr = (MapReduce *) (valuebytes); \
    totalnvalues = macro_mr->multivalue_blocks(macro_nblocks); \
  } 

#define BEGIN_BLOCK_LOOP(multivalue, valuebytes, nvalues)  \
  for (int macro_iblock = 0; macro_iblock < macro_nblocks; macro_iblock++) { \
    if (macro_mr)  \
      (nvalues) = macro_mr->multivalue_block(macro_iblock, \
                                             &(multivalue),&(valuebytes)); 

#define END_BLOCK_LOOP } 

#endif
