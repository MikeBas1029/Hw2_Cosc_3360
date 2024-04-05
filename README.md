# 3360 OS Assignment

## Description:
This OS assignment implements the Banker's deadlock avoidance algorithm, leveraging Unix/Linux processes manipulation. It manages processes by requesting
and releasing resources using semaphores to ensure concurrency. Resource requests are handled with an Earliest Deadline First (EDF) scheduler, with Least Laxity First (LLF)
used as a tiebreaker. Input data is read from two text files.

I am using the text files that have the variable names for the information for Banker's Algorithim 

## How to run:
1. Complie the code my entering inside the terminal: 'g++ -o test main.cpp -pthread'
2. Execting the compiled program in the terminal with the input files. Ex: './test sample.txt sample_words.txt'
