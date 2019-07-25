#Summer Research 2019: Matrix Multiplication
#Pi Decision Diagrams Class
#Date: 27-June-2019
#Author: Akriti Dhasmana

import copy, utilities


# structure :
# key : (root_node_transposition, childnode0_key, childnode1_key)

class piDD:
    def __init__(self, dict = {}):
        """
        Creates an empty set of permutations
        """
        if dict == {}:
            self.zero_val()
        else:
            self.piDD = dict
            self.top_node = utilities.max_length_in_list(self.return_keys())
            self.dim = self.piDD[self.top_node][0][0]

    def copy(self):
        dict = copy.deepcopy(self.piDD)
        pi1 = piDD(dict)
        return pi1


    def single_perm(self, perm):
        """
        Creates a piDD for a single perm
        :param perm: permutation for which the piDD has to be created.
        :return: piDD
        """
        dict = {}
        decomposed = perm.decompose()
        if decomposed == []:
            dict["[1]"] = None
            self.piDD = dict
            self.top_node = "[1]"
            self.dim = 0
        else:
            dict["[0]"] = None
            dict["[1]"] = None
            lowest_node = decomposed[0]
            child_node = '[' + str(lowest_node) + '|' + "[0]" + '|' + "[1]" + ']'
            dict[child_node] = (lowest_node, '[0]', '[1]')
            for i in range(1, len(decomposed)):
                key = '[' + str(decomposed[i]) + '|' + "[0]" + '|' + child_node + ']'
                dict[key] = (decomposed[i], '[0]', child_node)
                child_node = key
            self.piDD = dict
            self.top_node = utilities.max_length_in_list(self.return_keys())
            self.dim = self.piDD[self.top_node][0][0]

    def multiple_perm(self, list_of_perms):
        p1 = self.construct_complex_piDD_helper(list_of_perms)
        self.union(pi)

    def construct_complex_piDD_helper(self, list):

        if len(list) == 1:
            pi1 = piDD()
            pi1.single_perm(list[0])
            return pi1
        elif len(list) == 2:
            pi1 = piDD()
            pi1.single_perm(list[0])
            pi2 = piDD()
            pi2.single_perm(list[1])
            pi1.union(pi2)
            return pi1
        else:
            if len(list) % 2 == 0:
                x = int(len(list)) // 2
            else:
                x = (int(len(list)) + 1) // 2
            pi3 = self.construct_complex_piDD_helper(list[0:x])
            pi4 = self.construct_complex_piDD_helper(list[x:int(len(list))])
            pi3.union(pi4)
            return pi3



    def return_keys(self):
        """
        Returns a list of nodes in the piDD
        :return: a list
        """

        keys = list(self.piDD.keys())
        return keys

    def return_values(self):
        """
        Returns a list of all the children nodes.
        :return: a list
        """

        values = list(self.piDD.values())
        return values

    def find_children_nodes(self, parent_node):
        if parent_node == '[0]' or parent_node == '[1]':
            return None
        else:
            return (self.piDD[parent_node][1], self.piDD[parent_node][2])

    def zero_val(self):
        """
        Returns a single zero node.
        :return:
        """
        self.piDD = {"[0]": 'None'}
        self.top_node = "[0]"
        self.dim = 0

    def identity(self):
        """
        Creates a piDD for the identity permutation.
        :return: piDD
        """
        self.piDD = {"[1]": 'None'}
        self.top_node = "[1]"
        self.dim = 0

    def return_identity_permutation(self):
        list = []
        for i in range(1, self.dim + 1):
            list.append(i)
        return list

    def is_empty(self):
        """
        Checks if the piDD is empty.
        :return: True, if empty.
                 False otherwise.
        """
        dict = self.piDD
        if dict == {"[0]": None}:
            return True
        else:
            return False\

    def equality_testing(self, piDD):
        """
        Returns 1 if the two piDDs have the same structure, 0 otherwise
        :param piDD: the pi Decision diagram to be checked against
        :return: 1 or 0
        """
        p = False
        if len(self.return_values())== len(piDD.return_values()):
            if len(self.return_keys()) == len(piDD.return_keys()):
                p = True
        for i in self.return_keys():
            if i in piDD.return_keys():
                p = True
        if p == True:
            for i in self.return_values():
                if i in piDD.return_values():
                    return 1
        return 0

    def enlist(self):
        """
        Returns the list of all possible permutations that can be achieved using a given piDD.
        :return: List of permutations
        """
        list = self.enlist_helper(self.top_node)
        list.sort()
        return list

    def enlist_helper(self, root_node):

        """

        :param root_node: the string key
        :return: list of permutations in the piDD whose root is rhe root_node
        """

        if root_node == '[0]':
            return []
        elif root_node == '[1]':
            list = self.return_identity_permutation()
            return [list]
        else:
            return self.enlist_helper(self.find_children_nodes(root_node)[0]) + utilities.apply_swap_to_whole_list(self.enlist_helper(self.find_children_nodes(root_node)[1]), self.piDD[root_node][0])

    def count(self):
        """
        Counts the number of permutations in the piDD.
        :return: an integer
        """
        return self.count_helper(self.top_node)

    def count_helper(self, root_node):
        # base case
        if self.find_children_nodes(root_node) == None:
            return 0
        elif self.find_children_nodes(root_node)[0] == '[1]' and self.find_children_nodes(root_node)[1] == '[1]':
            return 2
        elif self.find_children_nodes(root_node)[0] == '[1]' or self.find_children_nodes(root_node)[1] == '[1]':
            return 1
        # recursive case
        else:
            return self.count_helper(self.find_children_nodes(root_node)[0]) + self.count_helper(self.find_children_nodes(root_node)[1])

    def transpose(self, transposition):
        """
        Applies a given transposition to the piDD.
        :param transposition: an instance of transposition class.
        :return: piDD
        """
        top_node = self.top_node
        self.top_node = self.transpose_helper(top_node, transposition)
        int(self.top_node.split('|')[0][1::].strip('()').split(', ')[0])
        self.run_clean_up()

    def transpose_helper(self, top_node, transposition):
        #return top_node
        if top_node == '[0]':
            key = '[0]'
        elif top_node == '[1]':
            key ='[' + str(transposition) + '|' + '[0]' + '|' + '[1]' + ']'
            self.piDD[key] = (transposition, '[0]', '[1]')
        else:
            x = self.piDD[top_node][0][0]
            y = self.piDD[top_node][0][1]
            u = transposition[0]
            v = transposition[1]
            if u > x:
                key = "[" + str(transposition) + "|" + "[0]" + "|" + top_node + "]"
                self.piDD[key] = (transposition, '[0]', self.piDD[top_node][2])

            elif x > u:
                left_child = self.transpose_helper(self.piDD[top_node][1], (u, v))
                right_child = self.transpose_helper(self.piDD[top_node][2], (u, v))
                if y == u:
                    key = "[" + str((x, v)) + "|" + left_child + '|' + right_child + "]"
                    self.piDD[key] = ((x, v), left_child, right_child)
                elif y == v:
                    key = "[" + str((x, u))+ "|"+ left_child+ '|'+ right_child + "]"
                    self.piDD[key] = ((x, u), left_child, right_child)
            elif x == u:
                if y == v:
                    return "[1]"
                else:
                    left_child = self.transpose_helper(self.piDD[top_node][1], (u, v))
                    right_child = self.transpose_helper(self.piDD[top_node][2], (y, v))
                    key = "[" + str((x, y)) + "|" + left_child + '|' + right_child + "]"
                    self.piDD[key] = ((x, y), left_child, right_child)

        return key

    def run_clean_up(self):
        nodes = self.clean_up_helper(self.top_node)
        for i in self.return_keys():
            if i not in nodes:
                del self.piDD[i]

    def clean_up_helper(self, node):
        if node == '[0]' or node == '[1]':
            return [node]
        else:
            return [node] + self.clean_up_helper(self.piDD[node][1]) \
                   + self.clean_up_helper(self.piDD[node][2])

    def union(self, piDD2):
        """
        Takes the union of two piDDs to create a new one.
        :param piDD2: another instance of piDD
        :return: a new piDD
        """
        #For the two piDDs if the self has a top node (x1, y1) and piDD has the form (x2, y2) then x1>=x2 and y1>=y2
        top_node = piDD2.top_node
        self_top_node = self.top_node
        self.piDD.update(piDD2.piDD)
        self.top_node = self.union_helper(self_top_node, top_node)
        self.dim = self.piDD[self.top_node][0][0]
        self.run_clean_up()

    def union_helper(self,  self_top_node, top_node):
        #bass case
        if (self_top_node == '[1]' and top_node == '[0]' )\
                or (self_top_node == '[0]' and top_node == '[1]') \
                or (self_top_node == '[1]' and top_node == '[1]'):
            key = '[1]'
            return key
        elif self_top_node == '[0]' and top_node == '[0]':
            key = '[0]'
            return key
        elif top_node == '[1]':
            key = "[" + str(self.piDD[self_top_node][0]) + "|" + "[1]" + "|" + self.find_children_nodes(self_top_node)[1] + "]"
            self.piDD[key] = (self.piDD[self_top_node][0], "[1]", self.find_children_nodes(self_top_node)[1] )
            return key
        elif self_top_node == '[1]':
            key = "[" + str(self.piDD[top_node][0]) + "|" + "[1]" + "|" + self.find_children_nodes(top_node)[1] + "]"
            self.piDD[key] = (self.piDD[top_node][0], "[1]", self.find_children_nodes(top_node)[1])
            return key
        elif top_node == "[0]":
            return self_top_node
        elif self_top_node == "[0]":
            return top_node
        elif self.piDD[top_node][0][0] == self.piDD[self_top_node][0][0]:
            if self.piDD[top_node][0][1] == self.piDD[self_top_node][0][1]:
                left_child = self.union_helper(self.find_children_nodes(self_top_node)[0], self.find_children_nodes(top_node)[0])
                right_child = self.union_helper(self.find_children_nodes(self_top_node)[1], self.find_children_nodes(top_node)[1])
                key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                return key

            elif self.piDD[top_node][0][1] < self.piDD[self_top_node][0][1]:
                left_child = self.union_helper(self.find_children_nodes(self_top_node)[0],top_node)
                right_child = self.find_children_nodes(self_top_node)[1]
                key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                return key

            elif self.piDD[top_node][0][1] > self.piDD[self_top_node][0][1]:
                left_child = self.union_helper(self.find_children_nodes(top_node)[0], self_top_node)
                right_child = self.find_children_nodes(top_node)[1]
                key = "[" + str(self.piDD[top_node][0]) + "|" + left_child + '|' + right_child + "]"
                self.piDD[key] = (self.piDD[top_node][0], left_child, right_child)
                return key

        elif self.piDD[top_node][0][0] < self.piDD[self_top_node][0][0]:
                left_child = self.union_helper(self.find_children_nodes(self_top_node)[0], top_node)
                right_child = self.find_children_nodes(self_top_node)[1]
                key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                return key
        elif self.piDD[top_node][0][0] > self.piDD[self_top_node][0][0]:
            left_child = self.union_helper(self.find_children_nodes(top_node)[0], self_top_node)
            right_child = self.find_children_nodes(top_node)[1]
            key = "[" + str(self.piDD[top_node][0]) + "|" + left_child + '|' + right_child + "]"
            self.piDD[key] = (self.piDD[top_node][0], left_child, right_child)
            return key

    def intersection(self, piDD2):
        """
        Takes the intersection of the two piDDs to create a new one.
        :param piDD2: another instance of piDD
        :return: a new piDD
        """
        top_node = piDD2.top_node
        self_top_node = self.top_node
        self.piDD.update(piDD2.piDD)
        self.top_node = self.intersection_helper(self_top_node, top_node)
        self.dim = self.piDD[self.top_node][0][0]
        self.run_clean_up()

    def intersection_helper(self, self_top_node, top_node):
            # bass case
            if top_node == "[0]" or self_top_node == "[0]":
                key = '[0]'
                return key
            elif self_top_node == '[1]' and top_node == '[1]':
                key = '[1]'
                return key
            #Think of something for this
            elif top_node == '[1]':
                if self.piDD[self_top_node][1] == "[1]":
                    return "[1]"
                else:
                    return "[0]"
            elif self_top_node == '[1]':
                if self.piDD[top_node][1] == "[1]":
                    return "[1]"
                else:
                    return "[0]"

            elif self.piDD[top_node][0][0] == self.piDD[self_top_node][0][0]:
                if self.piDD[top_node][0][1] == self.piDD[self_top_node][0][1]:
                    left_child = self.intersection_helper(self.find_children_nodes(self_top_node)[0],
                                                   self.find_children_nodes(top_node)[0])
                    right_child = self.intersection_helper(self.find_children_nodes(self_top_node)[1],
                                                    self.find_children_nodes(top_node)[1])
                    key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                    self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                    return key

                elif self.piDD[top_node][0][1] < self.piDD[self_top_node][0][1]:
                    left_child = self.intersection_helper(self.find_children_nodes(self_top_node)[0], top_node)
                    right_child = "[0]"
                    key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                    self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                    return key

                elif self.piDD[top_node][0][1] > self.piDD[self_top_node][0][1]:
                    left_child = self.intersection_helper(self.find_children_nodes(top_node)[0], self_top_node)
                    right_child = "[0]"
                    key = "[" + str(self.piDD[top_node][0]) + "|" + left_child + '|' + right_child + "]"
                    self.piDD[key] = (self.piDD[top_node][0], left_child, right_child)
                    return key

            elif self.piDD[top_node][0][0] < self.piDD[self_top_node][0][0]:
                left_child = self.intersection_helper(self.find_children_nodes(self_top_node)[0], top_node)
                right_child = "[0]"
                key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                return key
            elif self.piDD[top_node][0][0] > self.piDD[self_top_node][0][0]:
                left_child = self.intersection_helper(self.find_children_nodes(top_node)[0], self_top_node)
                right_child = "[0]"
                key = "[" + str(self.piDD[top_node][0]) + "|" + left_child + '|' + right_child + "]"
                self.piDD[key] = (self.piDD[top_node][0], left_child, right_child)
                return key



    def difference(self, piDD2):
        """
        Takes the difference of the two piDDs to create a new one.
        :param piDD2: another instance of piDD
        :return: a new piDD
        """
        top_node = piDD2.top_node
        self_top_node = self.top_node
        self.piDD.update(piDD2.piDD)
        self.top_node = self.difference_helper(self_top_node, top_node)
        self.dim = self.piDD[self.top_node][0][0]
        self.run_clean_up()

    def difference_helper(self, self_top_node, top_node):
            # bass case
            if self_top_node == "[0]" or (self_top_node == '[1]' and top_node == '[1]'):
                key = '[0]'
                return key
            elif self_top_node == "[1]" and top_node == '[0]':
                key = "[1]"
                return key
            elif self_top_node == '[1]':
                if self.piDD[top_node][1] == '[1]':
                    return '[0]'
                else:
                    return '[1]'
            elif top_node == '[1]':
                if self.piDD[self_top_node][0] == '[1]':
                    key = "[" + str(self.piDD[self_top_node][0]) + "|" + "[0]" + '|' + self.piDD[self_top_node][2] + "]"
                    self.piDD[key] = (self.piDD[self_top_node][0], "[0]", self.piDD[self_top_node][2])
                    return key
                else:
                    key = self_top_node
                    return key
            elif top_node == "[0]":
                return self_top_node

            elif self.piDD[top_node][0][0] > self.piDD[self_top_node][0][0]:
                key = "[" + top_node + "|" + self.difference_helper(self_top_node, self.find_children_nodes(top_node)[0]) + "|" "[0]" + "]"
                self.piDD[key] = (self.difference_helper(self_top_node, self.find_children_nodes(top_node)[0]), "[0]")
                return key

            elif self.piDD[top_node][0][0] == self.piDD[self_top_node][0][0]:
                if self.piDD[top_node][0][1] == self.piDD[self_top_node][0][1]:
                    left_child = self.difference_helper(self.find_children_nodes(self_top_node)[0],
                                                   self.find_children_nodes(top_node)[0])
                    right_child = self.difference_helper(self.find_children_nodes(self_top_node)[1],
                                                    self.find_children_nodes(top_node)[1])
                    key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                    self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                    return key

                else:
                    left_child = self.intersection_helper(self.find_children_nodes(self_top_node)[0], top_node)
                    right_child = self.find_children_nodes(self_top_node)[1]
                    key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                    self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                    return key

            elif self.piDD[top_node][0][0] < self.piDD[self_top_node][0][0]:
                left_child = self.intersection_helper(self.find_children_nodes(self_top_node)[0], top_node)
                right_child = "[0]"
                key = "[" + str(self.piDD[self_top_node][0]) + "|" + left_child + '|' + right_child + "]"
                self.piDD[key] = (self.piDD[self_top_node][0], left_child, right_child)
                return key
