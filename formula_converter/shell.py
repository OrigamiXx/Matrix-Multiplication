import os
import subprocess
puz = "\"( (1 2 3 3 1 2) (2 3 3 3 1 2) (2 1 1 1 2 2) (1 3 1 1 2 2) (3 2 2 1 2 2) (1 1 2 3 2 2) (1 3 3 1 3 2) (1 1 2 1 1 3) (3 1 3 1 1 3) (3 2 1 2 1 3)) \""
#(1 3 1 2 1 3) (3 3 1 1 2 3) (1 2 3 2 2 3) (3 3 3 2 2 3)) \""

'''


'''
def data_to_puzzle(data):
	puzzles = []
	increment = (data.index("\n"))+1
	n = 0
	while (n <= len(data)):
    		puzzle = ""
    		#print(data[n-increment:n])
    		for line in data[n:n+increment-1]:
        		line = line[:-1]
        		line = line.replace(""," ")
        		puzzle += " (" + line + ") "
    		puzzle = "\"( " + puzzle +" )\""
    		puzzles.append(puzzle)
    		n = n + increment
	#print(puzzles[len(puzzles)-2:])
	return puzzles
'''nonusp = open('3by3nonUSP.txt', 'r')
nonuspdata = nonusp.readlines()
outputs = []
for p in data_to_puzzle(nonuspdata):
    os.system("echo " + p + " | scheme -q boolean-converter.scm | tail -n+5 > output.cnf")
    #os.system("~/Desktop/matrix-multiplication/minisat ~/Desktop/matrix-multiplication/formula_converter/output.cnf result.txt")
    #test = subprocess.Popen(["echo", puzzle, "| scheme", "| -q", "boolean-converter.scm", "| tail -n+5", "> output.cnf"], stdout=subprocess.PIPE)
    result = subprocess.Popen(["time", "/home/jiz/Desktop/matrix-multiplication/minisat",
                                "/home/jiz/Desktop/matrix-multiplication/formula_converter/output.cnf", "result.txt"], stdout=subprocess.PIPE)
    output = result.communicate()[0]
    #outputs.append(output)
    if result.returncode != 10:
        print("error occurs for the big string circuit input, should be all SAT")
        print(p)
        break
print("finished for nonusps")'''

'''usp = open('4by4USP.txt', 'r')
uspdata = usp.readlines()
outputs = []
puzzledata = data_to_puzzle(uspdata)
for p in puzzledata:
    os.system("echo " + p + " | scheme -q boolean-converter.scm | tail -n+5 > output.cnf")
    #os.system("~/Desktop/matrix-multiplication/minisat ~/Desktop/matrix-multiplication/formula_converter/output.cnf result.txt")
    #test = subprocess.Popen(["echo", puzzle, "| scheme", "| -q", "boolean-converter.scm", "| tail -n+5", "> output.cnf"], stdout=subprocess.PIPE)
    result = subprocess.Popen(["time", "/home/jiz/Desktop/matrix-multiplication/minisat",
                                "/home/jiz/Desktop/matrix-multiplication/formula_converter/output.cnf", "result.txt"], stdout=subprocess.PIPE)
    output = result.communicate()[0]
    #outputs.append(output)
    if result.returncode != 20:
        print("error occurs for the big string circuit input, should be all UNSAT")
        print(p)
        break
print("finished for usps")'''
os.system("echo " + puz + " | scheme -q boolean-converter.scm | tail -n+5 > output.cnf")
result = subprocess.Popen(["time", "minisat_solver",
							"/home/jiz/Desktop/matrix-multiplication/formula_converter/output.cnf", "result.txt"], stdout=subprocess.PIPE)
output = result.communicate()[0]
print(output)






#usp.close()
#nonusp.close()
