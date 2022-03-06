#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
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
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (matrix[i][j] == 1) {
                num_pipes++;
            }
        }
    }

    // Start piping
    int* fd[num_pipes][2];
    for (int i = 0; i < num_pipes; i++) {
        pipe(fd[i][2]);
        if (pipe(fd[i][2]) == -1) {
            perror("Pipes failed");
            return 1;
        }
    }

    int size = sizeof(matrix)/sizeof(matrix[0]);   // Size of matrix
    int pnum = -1;
    // The limit of fork times will be = size + 1 (there're 2 outputs in the matrix)
    for (int h = 0; h < size + 1; h++) {
        pid_t p = fork();
        if (p == 0) {
            pnum = h;
            break;
        }
    }

    return 0;
}