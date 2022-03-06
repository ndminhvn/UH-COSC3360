#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main () {
    // Read adjacency matrix from input file
    int matrix[7][7];      // Two dimensions array to store input from files
    
    ifstream inputFile1 ("input1.txt");
    if (inputFile1.is_open()) {
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                inputFile1 >> matrix[i][j];
            }
        }
        inputFile1.close();
    }

    // Size of matrix
    int size = sizeof(matrix)/sizeof(matrix[0]);

    // Read the word content
    string arr[3];
    ifstream inputFile2 ("input2.txt");
    if (inputFile2.is_open()) {
        for (int i = 0; i < 3; i++) {
            inputFile2 >> arr[i];
        }
        inputFile2.close();
    }

    // Number of pipes
    int num_pipes = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] == 1)
                num_pipes++;
        }
    }

    // Start piping
    int fd[num_pipes][2];
    for (int i = 0; i < num_pipes; i++) {
        pipe(&fd[i][2]);
        if (pipe(&fd[i][2]) == -1) {
            perror("Pipes failed");
            return 1;
        }
    }

    // Vector holds the input vertices in the matrix
    vector<int> input_ids;
    // Find the number of 0s column, which is the number of inputs in the matrix
    int zero_column = 0;
    for (int j = 0; j < size; j++) {
        int i = 0;
        while (i < size) {
            if (!matrix[i][j] == 0) {
                break;
            }
            i++;
            if (i == 6)     // If j reaches to the end of the column, that means this is a zero column
                input_ids.push_back(j);     // Add column index as input ID
                zero_column++;
        }
    }
    

    // Vector holds the output vertices in the matrix
    vector<int> output_ids;
    // Find the number of 0s rows, which is the number of outputs in the matrix
    int zero_row = 0;
    for (int i = 0; i < size; i++) {
        int j = 0;
        while (j < size) {
            if (!matrix[i][j] == 0) {
                break;
            }
            j++;
            if (j == 6)     // If j reaches to the end of the row, that means this is a zero row
                output_ids.push_back(i);
                zero_row++;
        }
    }

    
    int limit = size;   // The number of time to fork equals to the size of matrix
    // The limit of fork times will be = size + 1 if there're more than 1 output in the matrix
    if (zero_row > 1) 
        limit += 1;

    int pnum = -1;
    for (int h = 0; h < limit; h++) {
        pid_t p = fork();
        if (p == -1) {
            perror("Fork failed");
            return 1;
        }
        // Child process
        else if (p == 0) {
            pnum = h;
            break;
        }
    }

    return 0;
}