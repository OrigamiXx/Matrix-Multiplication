# Module Name: testing
# Author: Akriti Dhasmana
# Honor Code Statement: I affirm that I have carried out all of my academic endeavors with full academic honesty.

import random, test_suite, PiDD, Permutations

def create_random_permutation(dim):
    list = []
    for i in range(1, dim+1):
        list.append(i)
    random.shuffle(list)
    return list

def construct_complex_piDD(number_of_permutations, dim):
    list = []
    for i in range(number_of_permutations):
        list.append(create_random_permutation(dim))
    construct_complex_piDD_helper(list)

def construct_complex_piDD_helper(list):
    if len(list) == 1:
        pi1 = PiDD.PiDD()
        pi1.single_perm(list[0])
        return pi1
    elif len(list) == 2:
        pi1 = PiDD.PiDD()
        pi1.single_perm(list[0])
        pi2 = PiDD.PiDD()
        pi2.single_perm(list[0])
        pi1.union(pi2)
        return pi1
    else:





def test_union(dim, attempts):

    """
    Runs tests for compare to function
    Prints out the summary for all the tests.

    """
    print('Testing the union function for ' + str(dim) + '-dimension permutations.')
    suite = test_suite.create()
    for i in range(attempts):
        list1 = create_random_permutation(dim)
        list2 = create_random_permutation(dim)
        list = [list1] + [list2]
        list.sort()

        print(list1)
        print(list2)

        perm1 = Permutations.Permutation(list1)
        perm2 = Permutations.Permutation(list2)

        PiDD1 = PiDD.piDD()
        PiDD1.single_perm(perm1)
        PiDD2 = PiDD.piDD()
        PiDD2.single_perm(perm2)


        PiDD1.union(PiDD2)
        try:
            test_suite.assert_equals(suite,
                             "The list of permutations represented by piDD obtained by the union of the two piDDs is" + str(list),
                             list,
                             PiDD1.enlist())
        except ValueError or IndexError or TypeError:

            print("------------------------------------------------------------------------------------")
            print("ERROR RAISED")
            print(list1)
            print(list2)
            print(PiDD1.piDD)
            print("")

            print("------------------------------------------------------------------------------------")


    test_suite.print_summary(suite)


def test_intersection(dim, attempts):
    """
    Runs tests for compare to function
    Prints out the summary for all the tests.

    """
    print('Testing the intersection function for ' + str(dim) + '-dimension permutations.')
    suite = test_suite.create()
    for i in range(attempts):
        PiDD1 = PiDD.piDD()
        PiDD2 = PiDD.piDD()
        list1 = create_random_permutation(dim)
        list2 = create_random_permutation(dim)
        list3 = create_random_permutation(dim)
        list4 = create_random_permutation(dim)
        list5 = create_random_permutation(dim)
        list6 = create_random_permutation(dim)

        combined_list_1 = [list1] + [list3] + [list6]
        combined_list_2 = [list2] + [list4] + [list5]

        intersection = []
        for i in combined_list_1:
            if i in combined_list_2:
                intersection.append(i)

        print(combined_list_1)
        print(combined_list_2)



        perm1 = Permutations.Permutation(list1)
        perm2 = Permutations.Permutation(list2)
        perm3 = Permutations.Permutation(list3)
        perm4 = Permutations.Permutation(list4)
        perm5 = Permutations.Permutation(list5)
        perm6 = Permutations.Permutation(list6)


        PiDD1.single_perm(perm1)
        PiDD3 = PiDD.piDD()
        PiDD3.single_perm(perm3)
        PiDD1.union(PiDD3)
        PiDD6 = PiDD.piDD()
        PiDD6.single_perm(perm6)
        PiDD1.union(PiDD6)



        PiDD2.single_perm(perm2)
        PiDD4 = PiDD.piDD()
        PiDD4.single_perm(perm4)
        PiDD2.union(PiDD4)
        PiDD5 = PiDD.piDD()
        PiDD5.single_perm(perm5)
        PiDD2.union(PiDD5)

        PiDD1.intersection(PiDD2)



        try:
            test_suite.assert_equals(suite,
                             "The intersection of the two piDDs is " ,
                             intersection,
                             PiDD1.enlist())
        except ValueError or IndexError or TypeError:

            print("------------------------------------------------------------------------------------")
            print("ERROR RAISED")
            print(combined_list_1)
            print(combined_list_2)
            print(PiDD1.piDD)
            print("")

            print("------------------------------------------------------------------------------------")


    test_suite.print_summary(suite)


if __name__ == "__main__":

    test_intersection(5, 100)


