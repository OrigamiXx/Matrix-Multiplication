import random, copy, isSUSP_BruteForce as bf

def add_col_to_puzzle(col, puzzle):
    """
    Adds the column to the puzzle with each element of the nested list added to respective row,
    :param col: The column to be added
    :param puzzle: The puzzle to which it must be added
    :return: the puzzle with the added column
    """
    new_puzzle = copy.deepcopy(puzzle)
    for i in range(len(col)):
        new_puzzle[i] = new_puzzle[i] + col[i]
    return new_puzzle

def all_cols(s):
    to_return = []
    list_of_cols = all_cols_helper(s)
    for col in list_of_cols:
        to_append = create_empty_puzzle(s)
        for j in range(s):
            to_append[j].append(col[j])
        to_return.append(to_append)
    return to_return



def all_cols_helper(s):
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
        for col in all_cols_helper(s-1):
            for j in range(1, 4):
                col_copy = copy.deepcopy(col)

                col_copy = col_copy + [j]
                to_return.append(col_copy)
        return to_return


def find_the_best_column(list_of_cols, puz):
    '''
    Iterates through all the possible columns to find the best possible column.
    :param s: number of rows
    :param puz: the puzzle
    :return: Best possible column
    '''
    best_col_so_far = list_of_cols[0]
    for i in list_of_cols[1:]:
        best_piDD = bf.piDD_of_all_possible_perms(best_col_so_far).copy()
        best_piDD.intersection(bf.piDD_of_all_possible_perms(puz))
        best_count = best_piDD.count()
        current_piDD = bf.piDD_of_all_possible_perms(i).copy()
        current_piDD.intersection(bf.piDD_of_all_possible_perms(puz))
        current_count = current_piDD.count()
        if current_count < best_count:
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

def choose_first_col(list_of_cols):
    best_col = list_of_cols[0]
    for i in list_of_cols[1::]:
        if bf.piDD_of_all_possible_perms(i).count()< bf.piDD_of_all_possible_perms(best_col).count():
            best_col = i
    return best_col


def make_puzzle(num_of_rows):
    '''
    Creates a SUSP of the specified size.
    :param num_of_rows: size of the
    :return: puzzle
    '''
    list_of_cols = all_cols(num_of_rows)
    puzzle = choose_first_col(list_of_cols)
    while bf.piDD_of_all_possible_perms(puzzle).count() != 0:
        best_col = find_the_best_column(list_of_cols, puzzle)
        puzzle = add_col_to_puzzle(best_col, puzzle)
        # bf.print_puzzle(puzzle)
        # print()
    return puzzle

if __name__ == "__main__":
    #print(all_cols(3))
    bf.print_puzzle(make_puzzle(2))
    print()
    bf.print_puzzle(make_puzzle(3))
    print()
    bf.print_puzzle(make_puzzle(4))
    print()
    bf.print_puzzle(make_puzzle(5))
    print()




