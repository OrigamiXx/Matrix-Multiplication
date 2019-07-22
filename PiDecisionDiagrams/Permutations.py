#Summer Research 2019: Matrix Multiplication
#Permutations Class
#Date: 27-June-2019
#Author: Akriti Dhasmana

import Transpositions, utilities as util, copy


class Permutation:

    def __init__(self, list = []):
        """
        Creates an object for a single permutation.
        :param tup: tuple with each item denoting the new position of its index.
        """
        self.perm = list

    def dim(self):
        """
        Returns the greatest number that has been moved from its original position in the permutation
        :return: integer
        """
        max = 1
        list = self.perm
        for i in range(len(list)):
            if i+1 != list[i]:
                if i+1 > max:
                    max = i+1
        return max


    def decompose(self):

        """
        Converts from the simple representation of the permutation to a sequence of transpositions.
        :return: list of transpositions
        """
        decomposed = []
        to_decompose = Permutation(copy.deepcopy(self.perm))
        while to_decompose.dim() != 1:
            tup = (to_decompose.dim(), to_decompose.perm[to_decompose.dim()-1])
            decomposed.append(tup)
            to_decompose.perm = util.swap(to_decompose.perm, to_decompose.dim(), to_decompose.perm[to_decompose.dim()-1])
        decomposed.reverse()
        return decomposed

    def transposition_to_perm(self, list):
        """
        takes a list of transpostion and return the corresponding permutation
        :param list: list of transpositions
        :return: None
        """
        to_return = []
        for i in range(list[len(list) - 1]):
            to_return[i] = i + 1
        for i in list:
            to_return = util.swap(to_return, list[i][0], list[i][1])
        self.perm = to_return


