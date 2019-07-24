def find_top_node_as_tuple(self):
    """
    returns the topmost node of the piDD
    :return: tuple
    """
    tup = (int(max(self.return_keys()).split('|')[0][1::].strip('()').split(', ')[0]),
           int(max(self.return_keys()).split('|')[0][1::].strip('()').split(', ')[1]))
    return tup


def find_raw_form_of_node(self, node):
    """
    Return the node in the form that it appears in the dictionary
    :param node: tuple
    :return: string
    """
    if node == [0] or node == [1] or node == '[0]' or node == '[1]' or node == 'None':
        return str(node)
    else:
        for i in list(self.piDD.keys()):
            if i.split('|')[0].strip('[]') == str(node):
                if i.split('|')[2].strip('[]') == str(self.find_children_nodes(node)[1]).strip('[]'):
                    return i


def extract_node_as_tuple(self, node):
    if node == '[0]' or node == '[1]':
        return tuple(node)
    else:
        tup = (int(node.split('|')[0][1::].strip('()').split(', ')[0]),
               int(node.split('|')[0][1::].strip('()').split(', ')[1]))
        return tup
