import Permutations, PiDD, utilities

perm1 = Permutations.Permutation([4, 3, 2, 1, 5])
print(perm1.decompose())

perm2 = Permutations.Permutation([4, 1, 2, 5, 3])
print(perm2.decompose())

perm3 = Permutations.Permutation([5, 3, 1, 2, 4])
print(perm3.decompose())

piDD1 = PiDD.piDD()
piDD2 = PiDD.piDD()
piDD3 = PiDD.piDD()

piDD1.single_perm(perm1)
piDD2.single_perm(perm2)
piDD3.single_perm(perm3)


piDD1.union(piDD2)
piDD1.union(piDD3)
print(piDD1.enlist())


