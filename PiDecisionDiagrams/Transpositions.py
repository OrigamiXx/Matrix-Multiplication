#Summer Research 2019: Matrix Multiplication
#Transpositions Class
#Date: 27-June-2019
#Author: Akriti Dhasmana

class transposition:
    def __init__(self, x, y):
        """
        Creates the instance of a simple transposition
        """
        self.transposition = (x, y)

    def return_transposition(self):
        return self.transposition

    def return_first(self):
        """
        Returns the number to be shifted
        :return: an integer
        """
        return self.transposition[0]

    def return_second(self):
        """
        Return tne number to be exchanged with.
        :return: an integer
        """
        return self.transposition[1]


