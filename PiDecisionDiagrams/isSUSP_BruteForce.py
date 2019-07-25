import itertools , PiDD, Permutations, utilities, copy


def compare_element(e1, e2):
    '''Takes two numbers (1, 2, 3).  If equal returns 1, otherwise 0.'''
    if e1 == e2:
        return 1
    else:
        return 0


def print_puzzle(puzzle):
    for row in puzzle:
        print(row)

def apply_permutation_to_column(perm, col):
    for i in perm.perm:
        col = utilities.swap_ind(col, i, perm.perm[i])
    return col

def comine_p2_p3(p2, p3):
    len = len(p2.perm)
    list = copy.deepcopy(p2.perm)
    for i in p3.perm:
        list.append(i+len)
    perm = Permutations.Permutation(list)
    return perm

def is_SUSP(original_puzzle, op_perm):
    '''
    Takes a puzzle and returns 0 if it's an SUSP, returns the number of possible permutations of the puzzle otherwise.
    :param original_puzzle:
    :return: integer
    '''
    p1 = tuple(original_puzzle)# itertools produces tuples not lists.
    num_of_unique_permutations = 0
    DD = PiDD.piDD()
    ident_p2 = True
    toreturn = True
    for p2 in itertools.permutations(op_perm):  # Must use separate iterators
        ident_p3 = True
        for p3 in itertools.permutations(op_perm):
            if not ident_p2 or not ident_p3:
                result = False
                for row_num in range(len(p1)):
                    for i in range(len(original_puzzle[row_num])):
                        #print(p3[row_num][i])
                        if (compare_element(original_puzzle[row_num][i], 1) +
                            compare_element(apply_permutation_to_column(p2[row_num], original_puzzle[p1.index(p2[row_num])])[i], 2) +
                            compare_element(apply_permutation_to_column(p3[row_num], original_puzzle[p1.index[p3[row_num]]])[i], 3) == 2):
                            result = True
                if not result:
                    pi1 = PiDD.piDD()
                    pi1.single_perm(comine_p2_p3(p2, p3))
                    num_of_unique_permutations += 1
                    DD.union(pi1)
                    toreturn = False
            ident_p3 = False
        ident_p2 = False
    if toreturn:
        return (DD, 0)
    else:
        return (DD, num_of_unique_permutations)


if __name__ == "__main__":
    puzzle1 = [[1, 3], [2, 1]]
    print_puzzle(puzzle1)
    print(is_SUSP(puzzle1, [Permutations.Permutation([1, 2]), Permutations.Permutation([1, 2])])[1])

    # puzzle1 = ["13" , "21"]
    # print_puzzle(puzzle1)
    # print(is_SUSP(puzzle1))

