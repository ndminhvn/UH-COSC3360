# Homework 1 - Minh Nguyen - 2069407

This project has two input files, one contains the adjacency matrix, and the other one contains words to be processed.

## First, the program reads the matrix from the first input file.
- From the matrix, we can determine how many input and output vertices (processes), then we can keep track of them by assigning their IDs (which is each of them row/column index).
- Then, it declares the number of pipes need to be made (equals to the number of 1s in the matrix).
- The number of forks should be equal to sizeOfMatrix + 1 (since there are two outputs in the matrix, then we need to combine these two results to get the final result by creating one more process)
