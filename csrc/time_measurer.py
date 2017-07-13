#!/usr/bin/env python
#This script would test the running time of different USP checking function by
#a few given USPs

import subprocess, shlex
import os
#This function would return the runtime of a usp checker function with a given
#row and column
def time_same_size(s, k,makeFile_dir,origin_dir, start_end):
    row = str(s)
    column = str(k)

    os.chdir(makeFile_dir)
    subprocess.call("make all", shell = True)


    #Tell the command line to input time -p python test_File.py 2>&1.
    result =  subprocess.check_output("time -p usp_tester %d %d %d 2>&1"% (s, k, start_end), shell =True)

    subprocess.call("make clean", shell = True)

    #Slice out the user and sys time from the output of time command
    sys_time = float(slice_time(result)['time'])
    result = result[0:slice_time(result)['left']]
    user_time = float(slice_time(result)['time'])
    total_time = sys_time + user_time
    os.chdir(origin_dir)
    return total_time


    #p = subprocess.Popen(["(time", "python", "test_File.py)", ">", "timeTiming.csv", "2>&1"])
    #result = p.communicate[0]
    #print(result)
    #os.system("(time "+ "python "+ "test_File.py)"+ "> "+ "timeTiming.csv "+ "2>&1")

def slice_time(string):
    last_deci = string.rfind(".")
    leftEnd = string.rfind(" ",0,last_deci)+1
    rightEnd = last_deci+3
    return {'time':string[leftEnd: rightEnd],'left':leftEnd,'right':rightEnd}

makefile_dir = '/home/xua/Desktop/matrix-multiplication'
saving_dir = '/home/xua/Desktop/matrix-multiplication/csrc'

os.chdir(makefile_dir)
k = int(input('What is the column of the testing? '))
s = int(input('Up to how many row is to be tested? '))
iteration = int(input('How many test time for each size? '))
os.chdir(saving_dir)
f = open('timeTiming.csv', 'w')
for x in range(1, s+1):
   f.write(str(x)+"*"+str(k)+","*iteration+"\n")

   for y in range(iteration):
        if(y==0):
            f.write(str(time_same_size(x,k,makefile_dir,saving_dir, 1)) + ",")
        elif(y == (iteration -1)):
            f.write(str(time_same_size(x,k,makefile_dir,saving_dir, 2)) + ",")
        else:
            f.write(str(time_same_size(x,k,makefile_dir,saving_dir, 0)) + ",")
       #f.write("--  ")
   f.write("\n")
f.close()


'''with f as open('timeTiming.csv', 'a'):
    for ea_line in f:
        print ea_line
    f.write('what?')
    time_same_size(f, 3, 4)'''
