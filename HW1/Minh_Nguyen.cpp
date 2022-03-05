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
    int fd1[2];     // Store two ends of the first pipe
    int fd2[2];     // Store two ends of the second pipe

    // Read adjacency matrix from input file
    int input_arr[7][7];      // Two dimensions array to store input from files
    
    ifstream inputFile ("input.txt");
    string line;
    if (inputFile.is_open()) {
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                inputFile >> input_arr[i][j];
            }
        }
        inputFile.close();
    }

    pid_t p;
    
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe failed");
        return 1;
    }

    p = fork();

    if (p < 0) {
        perror("Fork failed");
        return 1;
    }

    // Parent process
    else if (p > 0) {

    }
    
    // Child process
    else {

    }

    return 0;
}