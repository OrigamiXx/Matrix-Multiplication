import copy

def swap_ind(list, ind1, ind2):
    list1 = copy.deepcopy(list)
    el1 = list1[ind1]
    el2 = list1[ind2]
    list1[ind1] = el2
    list1[ind2] = el1
    return list1

def swap(list, el1, el2):
    list1 = copy.deepcopy(list)
    ind1 = list1.index(el1)
    ind2 = list1.index(el2)
    list1[ind1] = el2
    list1[ind2] = el1
    return list1

def apply_swap_to_whole_list(list_of_lists, tup):
    new_list = []
    for l in list_of_lists:
        new_list.append(swap(l, tup[0], tup[1]))

    return new_list

def max_length_in_list(l):
    max = l[0]
    for i in l:
        if len(max) < len(i):
            max = i
    return max

"""def count_helper(keys):
    top_node = find_top_node(keys)
    #base case
    if top_node[0] or top_node[1] == [1]:
        return 1
    #recursive casr
    else:
        if top_node[0] and top_node[1] != [0]:
            count = count + 1
        keys.pop(top_node)
        coucount_helper(keys) + count

"""
"""
#def extract_nodes(string):
    #one_split_list = string
"""