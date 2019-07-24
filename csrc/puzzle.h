#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include <string.h>
#include <stdio.h>



/*
 * Uses an integer to store a puzzle row, capable of representing rows
 * with at most 19 columns.
 */
typedef unsigned long puzzle_row;

//In this structure puzzle is stored in a 1D int array. In each cell,
//there is an int that can be tansfer into a trinary index
//of each row. E.g a 22 can be convert into 22=2*3^2+1*3^1+1*3^0=211
//=322 thus the raw is 322. If the row is longer than the converted
//data, then all the digits that are not covered will be replaced
//with 1. So for a 4 digit row a 22 would be 1322.
typedef struct puzzle {
  puzzle_row * puzzle;
  puzzle_row max_row;
  unsigned int s;
  unsigned int k;
  bool * tdm;
  bool tdm_valid;
}puzzle;


/*
 * Datatypes for expressing the return values of puzzle checkers and
 * heuristics.
 *
 * + IS_USP indicates that the puzzle is a strong USP.
 * + NOT_USP indicates that the puzzle is not a strong USP.
 *
 * + UNDET_USP indicates that it was not determined whether or not the
 *     puzzle is a strong USP.
 *
 *     Checkers should not return this value unless there is an
 *     intentional interruption of the function or an unrecoverable
 *     error.
 *
 *     Heuristics return this value in the event they are unable to
 *     decide whether the given puzzle is a strong USP.
 */
typedef enum check_val {NOT_USP, IS_USP, UNDET_USP} check_t;
typedef check_t (* checker_t)(puzzle *);

#define MAX_K 40

const puzzle_row MAX_ROWS[MAX_K+1] =
  {1, // 0
   ((puzzle_row)3),
   ((puzzle_row)3)*3,
   ((puzzle_row)3)*3*3,
   ((puzzle_row)3)*3*3*3,
   ((puzzle_row)3)*3*3*3*3,  // 5
   ((puzzle_row)3)*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3, // 10
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3, // 15
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3, // 20
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3, // 25
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3, // 30
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3, // 35
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
   ((puzzle_row)3)*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3 // 40
  };


puzzle * create_puzzle_copy(puzzle * p);

//create a puzzle that has one more row and same width as the input puzzle
// according to the given row_inde
//Again, this row_index is the trinary index
//MWA: Rename to add_row_to_puzzle.
puzzle * create_puzzle_from_puzzle(puzzle * p, puzzle_row row);

// Extends a puzzle adding num_rows_to_add rows with value 0 at the end
puzzle * extend_puzzle(puzzle * p, int num_rows_to_add);

//Creates a copy of puzzle p without the ith row.
puzzle * create_row_minor_puzzle(puzzle * p, int i);

//Create an empty puzzle with given row and cols length
puzzle * create_puzzle(int s, int k);

puzzle * create_puzzle_from_string(char * str);

//Read a file and return a puzzle. If the arguement is not a right file for
//puzzle, then do nothing
//Notice that the puzzle in the file is record as 1,2 or 3 since its for
//readable reason.
puzzle * create_puzzle_from_file(const char * filename);

//give width(column) and height(row) and the index of all the possible puzzle in this size
//return a puzzle
//index must be 0-(2^(k*s)-1)
//The index represents the whole puzzle
puzzle * create_puzzle_from_index(int s, int k, puzzle_row index);

// with a given puzzle return the index out of 0-(3^(r*c)-1)
int get_index_from_puzzle(puzzle * p);

//Return the number at the col_index digit of the row with given row_index.
inline int get_column_from_row(puzzle_row row, int c){
  return (row % MAX_ROWS[c + 1]) / MAX_ROWS[c] + 1;
}

/*
 * Returns the result of setting the entry at c of row_index to val.
 */
inline int set_entry_in_row(puzzle_row row, int c, int val) {
  return row + (val - get_column_from_row(row,c)) * MAX_ROWS[c];
}

/*
 * Returns the result of setting the entry at c of row_index to val.
 */
inline int get_entry(puzzle * p, int r, int c){
  return get_column_from_row(p -> puzzle[r], c);
}

inline void set_entry(puzzle * p, int r, int c, int val){
  p -> puzzle[r] = set_entry_in_row(p -> puzzle[r], c, val);
}

// Replaces data in puzzle with an random puzzle of the same
// dimensions.
void randomize_puzzle(puzzle * p);

// Sets the given puzzle p to a random strong USP of the same size.
// May not return if a strong USP of the given size does not exist.
// Is likely very slow except for small puzzle sizes.
void random_usp(puzzle * p);

// Sorts the rows of the puzzle in increasing order.  Invalidates 3DM.
void sort_puzzle(puzzle * p);

// Sorts the columns of the puzzle to separate those with fewer unique
// entries.  Then sorts the rows of the puzzle in increasing
// order. Invalidates 3DM.
void arrange_puzzle(puzzle * p);

// Print a puzzle to the specified open file.
void fprint_puzzle(FILE * f, puzzle * p);
// Print a puzzle to the console.
void print_puzzle(puzzle * p);

// Print a puzzle row to the specified open file.
void fprint_row(FILE * f, puzzle_row row, int k);
// Print a puzzle to the console.
void print_row(puzzle_row row, int k);



// Print a puzzle's 3DM instance to the specified open file.
void fprint_tdm(FILE * f, puzzle * p);
// Print a puzzle's 3DM instance to the console.
void print_tdm(puzzle * p);


/*
 * Returns true iff a length-k row that permutations map to u_1, u_2,
 * u_3, respectively, satisfies the inner condition of strong USPs.
 * It is false if this length-k row this mapping does not witness that
 * the puzzle is a strong USP.  Runtime is O(k).
 */
bool is_witness(puzzle * p, int r1, int r2, int r3);  //aka check_usp_rows
void compute_tdm(puzzle * p);
void simplify_tdm(puzzle * p);
int count_tdm(puzzle * p);

inline bool get_tdm_entry(puzzle * p, int r1, int r2, int r3){
  return p -> tdm[r1 * (p -> s) * (p -> s) + r2 * (p -> s) + r3];
}

inline bool set_tdm_entry(puzzle * p, int r1, int r2, int r3, bool val){
  return p -> tdm[r1 * (p -> s) * (p -> s) + r2 * (p -> s) + r3] = val;
}

inline void invalidate_tdm(puzzle * p){
  p -> tdm_valid = false;
}

inline void validate_tdm(puzzle * p){
  p -> tdm_valid = true;
}

//Write puzzle into a file
//void write_puzzle(puzzle * p, int index);

//free puzzle
void destroy_puzzle(puzzle * p);

//Return the next available puzzle from the open file f
puzzle * create_next_puzzle_from_file(FILE * f, int * lines_read);

//Takes in one line of puzzle and return the index of the line. Return -1 if line is an empty line.
int line_to_index(char * line);

#endif
