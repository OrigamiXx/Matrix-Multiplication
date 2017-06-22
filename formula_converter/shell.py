import os
import subprocess
#puz = "\"( (2 2 1 3 2) (1 3 1 3 2) (2 1 3 3 2 ) (1 1 1 1 2 ) )\""

def data_to_puzzle(data):
	puzzles = []
	increment = (data.index("\n"))+1
	n = (data.index("\n"))
	while (n <= len(data)):
    		puzzle = ""
    		#print(data[n-increment:n])
    		for i in data[:n]:
        		i = i[:-1]
        		i = i.replace(""," ")[1:-1]
        		puzzle += " (" + i + ") "
    		puzzle = "\"( " + puzzle +" )\""
    		puzzles.append(puzzle)
    		data = data[n+1:]
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

usp = open('3by3USP.txt', 'r')
uspdata = usp.readlines()
outputs = []
for p in data_to_puzzle(uspdata):
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
print("finished for usps")








usp.close()
#nonusp.close()
