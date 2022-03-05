#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdio>
#include <string>

using namespace std;

int main () {
    int fd1[2];     // Store two ends of the first pipe
    int fd2[2];     // Store two ends of the second pipe

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