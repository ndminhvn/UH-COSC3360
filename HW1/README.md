# Minh Nguyen - 2069407

This project has two input files, one contains the adjancency matrix, and the other one contains words to be processed.

First, the program reads the matrix from the first input file.
- Then, it declares the number of pipes need to be made (equals to the number of 1s in the matrix).
- The number of forks should be equal to sizeOfMatrix + 1 (because there are two rows of 0s in the matrix, which means that there are two outputs, and we need to combine these two results to get the final result by creating one more process)
