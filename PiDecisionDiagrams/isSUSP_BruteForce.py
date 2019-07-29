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
        col = utilities.swap_ind(col, i-1, perm.perm.index(i))
    return col

def comine_p2_p3(p2, p3):
    length = len(p2.perm)
    list = copy.deepcopy(p2.perm)
    for i in p3.perm:
        list.append(i+length)
    perm = Permutations.Permutation(list)
    return perm

def create_identity_perm(original_puzzle):
    list = []
    for i in range(1, len(original_puzzle) + 1):
        list.append(i)

    perm = Permutations.Permutation(list)
    return perm

def create_set_perms(ident_perm):
    to_return = []
    for i in itertools.permutations(ident_perm.perm):
        perm = Permutations.Permutation(list(i))
        to_return.append(perm)
    return to_return


def is_SUSP(puz):
    '''
    Takes a puzzle and returns 0 if it's an SUSP, returns the number of possible permutations of the puzzle otherwise.
    :param puz:
    :return: integer
    '''
    p1 = create_identity_perm(puz)
    S1 = create_set_perms(p1)
    num_of_unique_permutations = 0
    DD = PiDD.piDD()
    ident_p2 = True
    for p2 in S1:  
        ident_p3 = True
        for p3 in S1:
            if not ident_p2 or not ident_p3:
                result = False
                for r in range(len(puz)):
                    for c in range(len(puz[r])):
                        if ((puz[r][c] == 1) + (puz[p2.perm[r] - 1][c] == 2) + (puz[p3.perm[r] - 1][c] == 3)) == 2:
                            result = True
                if not result:
                    pi1 = PiDD.piDD()
                    pi1.single_perm(comine_p2_p3(p2, p3))
                    num_of_unique_permutations += 1
                    DD.union(pi1)
            ident_p3 = False
        ident_p2 = False
        
    return (DD, num_of_unique_permutations)


def is_SUSP_rec(column):
    '''Takes a column of a puzzle and returns a PiDD describing the set of
    pairs of permutations which make it not a SUSP. '''

    ## Compute counts for each entry.
    c = [0,0,0]
    for entry in column:
        c[entry - 1] += 1

    ## Form tuple to track remaining
    ## elements to distribute.
    cs = (tuple(c), tuple(c), tuple(c))

    abc = 0
    rows = len(column)

    DD = is_SUSP_rec_helper(abc, cs, rows, rows)

    perm = Permutations.Permutation(list(range(1, rows + 1)))
    ## XXX - aDD must be set of permtutation pairs for all automorphisms of column.
    aDD = PiDD.piDD() 
    DD.cartesian_product(aDD)
    aDD.cartesian_product(DD)
    return aDD
    
def is_SUSP_rec_helper(abc, cs, rows, rows_left):
    '''Takes an index of a row type abc as an integer between in [0..26],
    and the remaining counts to distribute.  Returns a PiDD describing
    the set of pairs of permutations which make it not a SUSP.

    '''

    res = PiDD.piDD()
    
    if abc == 3**3:
        if rows_left == 0:
            return res   
        else:
            return None  # Failed to fill up puzzle.
    else:

        found_something = False
        ## Compute row contents by converting from trinary.
        c = abc % 3
        b = ((abc - c) // 3) % 3 
        a = abc // 9

        ## Compute max number of times row abc can appear.
        max_count = min(cs[a][0], cs[b][1], cs[c][2])

        for m in range(0, max_count + 1):
            ## Compute residual counts.
            cs2 = (list(cs[0]), list(cs[1]), list(cs[2]))
            cs2[a][0] -= m
            cs2[b][1] -= m
            cs2[c][2] -= m
            cs2 = (tuple(cs2[0]), tuple(cs2[1]), tuple(cs2[2]))

            DD = is_SUSP_rec_helper(abc + 1, cs2, rows, rows_left - m)

            if DD != None:

                ## XXX - Correct permutation.
                perm = Permutations.Permutation(list(range(1,rows+1)))
                DD2 = PiDD.piDD()
                DD2.single_perm(perm)
                DD2.cartesian_product(DD)
                
                found_something = True
                res.union(DD)

    if found_something:
        return res
    else:
        return None

if __name__ == "__main__":

    print(is_SUSP_rec([1,2,3]).piDD)
    
    puzzle1 = [[1, 3], [2, 1]]
    print_puzzle(puzzle1)
    print(is_SUSP(puzzle1)[1])

    # puzzle1 =

    ["13" , "21"]
    # print_puzzle(puzzle1)
    # print(is_SUSP(puzzle1))

    puzzle2 = [[1, 1, 1], [3, 2, 1], [3, 3, 2]]
    print_puzzle(puzzle2)
    print(is_SUSP(puzzle2)[1])

    puzzle3 = [[3, 2, 3, 2], [1, 1, 3, 2], [1, 2, 1, 3], [3, 1, 1, 3], [1, 3, 2, 1]]
    print_puzzle(puzzle3)
    print(is_SUSP(puzzle3)[1])

    puzzle3 = [[3, 1, 3, 2], [1, 2, 3, 2], [1, 1, 1, 3], [3, 2, 1, 3], [3, 3, 2, 3]]
    print_puzzle(puzzle3)
    print(is_SUSP(puzzle3)[1])

    puzzle4 = [[3, 3, 3, 2], [1, 2, 3, 1], [1, 3, 1, 3], [3, 2, 2, 2], [1, 3, 2, 3]]
    print_puzzle(puzzle4)
    print(is_SUSP(puzzle4)[1])


    
    # puzzle5 = [[1,2,3,1,2],
    #            [2,3,2,1,2],
    #            [1,2,2,3,2],
    #            [2,3,3,3,1],
    #            [1,3,3,2,1],
    #            [2,2,1,2,1],
    #            [3,2,3,2,2],
    #            [1,3,1,3,1]]
    # print_puzzle(puzzle5)
    # print(is_SUSP(puzzle5))
