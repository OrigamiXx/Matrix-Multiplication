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


def piDD_of_all_possible_perms(puz):
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
        
    return DD


def is_SUSP_rec(column):
    '''Takes a column of a puzzle and returns a PiDD describing the set of
    pairs of permutations which make it not a SUSP. '''

    ## XXX - Doesn't work, cartesian_product and column_automorphisms are place holder.
    
    ## XXX - Also need to check against modifications in the C++
    ## implementation, I noticed bugs in this algorithm, but haven't
    ## translated the fixes back.

    rows = len(column)
    
    ## Compute counts for each entry.
    c = [0,0,0]
    for entry in column:
        c[entry - 1] += 1

    ## Compute permutations that map column to and from sorted.
    ## Applied to make column more uniform.
    sort_perm_list =  list(range(1, 2*rows + 1))
    sort_perm_inv_list = list(range(1, 2*rows + 1))
    c_sort = [0, c[0], c[0] + c[1]]
    for i in range(len(column)):
        entry = column[i] - 1
        sort_perm_list[i] = c_sort[entry] + 1
        sort_perm_list[i + rows] = c_sort[entry] + 1 + rows
        sort_perm_inv_list[c_sort[entry]] = i + 1
        sort_perm_inv_list[c_sort[entry] + rows] = i + 1 + rows
        c_sort[entry] += 1
    sort_perm = PiDD.piDD()
    sort_perm.single_perm(Permutations.Permutation(sort_perm_list))
    sort_perm_inv = PiDD.piDD()
    sort_perm_inv.single_perm(Permutations.Permutation(sort_perm_inv_list))
    
    ## Form tuple to track remaining
    ## elements to distribute.
    cs = (tuple(c), tuple(c), tuple(c))
    
    abc = 0

    ## Recursively determine the set of permutation pairs, up to
    ## automorphism, that witness column (when sorted) isn't an SUSP.
    memo = {}
    DD = is_SUSP_rec_helper(abc, cs, rows, rows, memo)

    ## Compute the set of column automorphisms.
    aDD = PiDD.piDD()
    aDD.column_automorphisms(c[0], c[1], c[2])

    ## Compose using cross products
    res = PiDD.piDD()
    res.cartesian_product(sort_perm_inv)
    res.cartesian_product(aDD)
    res.cartesian_product(DD)
    res.cartesian_product(aDD)
    res.cartesian_product(sort_perm)

    return res
    
def is_SUSP_rec_helper(abc, cs, rows, rows_left, memo):
    '''Takes an index of a row type abc as an integer between in [0..26],
    and the remaining counts to distribute.  Returns a PiDD describing
    the set of pairs of permutations which make it not a SUSP.

    '''
    
    ## Check memo table.
    key_str = str([abc,cs])
    if key_str in memo.keys():
        return memo[key_str]

    res = PiDD.piDD()
    res.single_perm(Permutations.Permutation(list(range(1, 2*rows + 1))))
    
    if abc == 3**3:
        if rows_left != 0:
            res = None
    elif (abc >= 3**2 and cs[0][0] != 0) \
       or (abc >= 3**2 * 2 and (cs[0][0] != 0 or  cs[1][0] != 0)):
        # Skipped over elements that must be mapped according to
        # identity in pi1, so no solution will be possible.
        res = None
    else:

        found_something = False
        ## Compute row contents by converting from trinary.
        c = abc % 3
        b = ((abc - c) // 3) % 3 
        a = abc // 9

        ## Compute max number of times row abc can appear.
        max_count = min(cs[a][0], cs[b][1], cs[c][2])

        for m in range(0, max_count + 1):
            ## Compute residual counts to reflect that m copies of abc appear.
            cs2 = (list(cs[0]), list(cs[1]), list(cs[2]))
            cs2[a][0] -= m
            cs2[b][1] -= m
            cs2[c][2] -= m
            cs2 = (tuple(cs2[0]), tuple(cs2[1]), tuple(cs2[2]))

            ## Recursively determine solution on residuals.
            DD = is_SUSP_rec_helper(abc + 1, cs2, rows, rows_left - m, memo)

            if DD != None:

                ## Compute pi2 and pi3 permutations in perm_list that
                ## create m copies of abc.
                L = [b, c]
                perm_list = list(range(1, 2*rows + 1))
                ## Loop for both pi2 and pi3.
                for e in [2, 3]:
                    j = e - 2 # Adjust index
                    if L[j] == 2 or L[j] == 3:
                        ## Shift the block of m copies of L[j] to the
                        ## place were the new entries are being added.
                        start = rows - rows_left
                        end = start + cs[0][j+1] + (0 if L[j] == 2 else cs[1][j+1])
                        shift = j * rows
                        ## Push elements < L[j] back.
                        for i in range(start, end):
                            perm_list[i + shift] = i + m + shift + 1  # +1 because permutations start at 1.
                        ## Pull elements = L[j] forward.
                        for i in range(end, end + m):
                            perm_list[i + shift] = start + (i - end) + shift + 1
                perm = Permutations.Permutation(perm_list)

                ## Compose the permutation induced by m copies of abc
                ## into the set of permutations determined by recusion.
                DD2 = PiDD.piDD()
                DD2.single_perm(perm)
                DD2.cartesian_product(DD)

                ## Include the permutation pairs founds in with those from other choices.
                found_something = True
                res.union(DD2)

        if not found_something:
            res = None

    ## Update memo table.
    memo[key_str] = res
    return res

if __name__ == "__main__":

    #print(is_SUSP_rec([1,2,3]).piDD)
    
    puzzle1 = [[1, 3], [2, 1]]
    print_puzzle(puzzle1)
    print(piDD_of_all_possible_perms(puzzle1))
    print(piDD_of_all_possible_perms(puzzle1)[0].count())



    puzzle2 = [[1, 1, 1], [3, 2, 1], [3, 3, 2]]
    print_puzzle(puzzle2)
    print(piDD_of_all_possible_perms(puzzle2))
    print(piDD_of_all_possible_perms(puzzle2).count())



    puzzle3 = [[3, 2, 3, 2], [1, 1, 3, 2], [1, 2, 1, 3], [3, 1, 1, 3], [1, 3, 2, 1]]
    print_puzzle(puzzle3)
    print(piDD_of_all_possible_perms(puzzle3))
    print(piDD_of_all_possible_perms(puzzle3).count())

    puzzle4 = [[3, 1, 3, 2], [1, 2, 3, 2], [1, 1, 1, 3], [3, 2, 1, 3], [3, 3, 2, 3]]
    print_puzzle(puzzle4)
    print(piDD_of_all_possible_perms(puzzle4))
    print(piDD_of_all_possible_perms(puzzle4).count())

    puzzle5 = [[3, 3, 3, 2], [1, 2, 3, 1], [1, 3, 1, 3], [3, 2, 2, 2], [1, 3, 2, 3]]
    print_puzzle(puzzle5)
    print(piDD_of_all_possible_perms(puzzle5))
    print(piDD_of_all_possible_perms(puzzle5).count())


    
    # puzzle5 = [[1,2,3,1,2],
    #            [2,3,2,1,2],
    #            [1,2,2,3,2],
    #            [2,3,3,3,1],
    #            [1,3,3,2,1],
    #            [2,2,1,2,1],
    #            [3,2,3,2,2],
    #            [1,3,1,3,1]]
    # print_puzzle(puzzle5)
    # print(piDD_of_all_possible_perms(puzzle5))
