# Homework 1 - Minh Nguyen - 2069407

This project has two input files, one contains the adjacency matrix, and the other one contains words to be processed.

### Vertex Structure 
```id, value, etc...```
## First, the program reads the matrix from the two input files.
- From the matrix, we can determine how many input and output vertices (processes), then we can keep track of them by assigning their IDs (which is each of them row/column index).
- Then, it declares the number of pipes need to be made (equals to the number of 1s in the matrix).
- The number of forks should be equal to sizeOfMatrix + 1 (since there are two outputs in the matrix, then we need to combine these two results to get the final result by creating one more process)

- Using the vector to store the information of the input vertices

## Second, the program makes the pipes and forks to read and write the data to different vertices