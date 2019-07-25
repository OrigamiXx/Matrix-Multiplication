import random, copy, isSUSP_BruteForce as bf




def make_new_col(row_num):
    '''
    Creates a nested list of list with each list within list representing a row
    :param row_num: desired number of rows in the puzzle.
    :return: nested list of list
    '''
    col = []
    for i in range(row_num):
        col.append(random.randint(1, 3))
    return col




def add_col_to_puzzle(col, puzzle):
    """
    Adds the column to the puzzle with each element of the nested list added to respective row,
    :param col: The column to be added
    :param puzzle: The puzzle to which it must be added
    :return: the puzzle with the added column
    """
    new_puzzle = copy.deepcopy(puzzle)
    for i in range(len(col)):
        new_puzzle[i].append(col[i])
    return new_puzzle

def all_cols(s):
    '''
    Generates all possible columns of size s.
    :param s: integer
    :return: List of all possible columns
    '''
    #Base Case:
    if s == 1:
        return [[1], [2], [3]]
    #Recursive Case:
    else:
        to_return = []
        for col in all_cols(s-1):
            for j in range(1,4):
                col_copy = col.copy()
                col_copy.append(j)
                to_return.append(col_copy)
        return to_return


def find_the_best_column(s, puzzle):
    '''
    Iterates through all the possible columns to find the best possible column.
    :param s: number of rows
    :param puzzle: the puzzle
    :return: Best possible column
    '''
    list_of_cols = all_cols(s)
    best_col_so_far = list_of_cols[0]
    for i in list_of_cols[1:]:
        best_puzzle = add_col_to_puzzle(best_col_so_far, puzzle)
        best_score_so_far = bf.is_SUSP(best_puzzle)
        current_puzzle = add_col_to_puzzle(i, puzzle)
        if bf.is_SUSP(current_puzzle) < best_score_so_far:
            best_col_so_far = i
    return best_col_so_far


def create_empty_puzzle(num_of_rows):
    '''
    creates an empty puzzle.
    :param num_of_rows: size of the puzzle.
    :return: creates an empty puzzle
    '''
    puzzle = []
    for i in range(num_of_rows):
        row = []
        puzzle.append(row)
    return puzzle


def make_puzzle(num_of_rows):
    '''
    Creates a SUSP of the specified size.
    :param num_of_rows: size of the
    :return: puzzle
    '''
    puzzle = create_empty_puzzle(num_of_rows)
    col = make_new_col(num_of_rows)
    puzzle = add_col_to_puzzle(col, puzzle)
    while bf.is_SUSP(puzzle) != 0:
        best_col = find_the_best_column(num_of_rows, puzzle)
        #print(best_col)
        puzzle = add_col_to_puzzle(best_col, puzzle)
        #bf.print_puzzle(puzzle)
        #print()
    return puzzle

if __name__ == "__main__":
    bf.print_puzzle(make_puzzle(5))