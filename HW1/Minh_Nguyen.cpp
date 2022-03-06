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

struct Vertex {
    int id;         // Id of the vertex
    string value;   // Value of the vertex
    bool isInput;   // Is the vertex input of another vertex
    bool isOutput;  // Is the vertex output of another vertex
    int inputId;    // ID of the incoming vertex
    int outputId;   // ID of the connected output vertex
};

int main () {
    // Read adjacency matrix from input file
    int matrix[7][7];

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

    // Vector holds the input vertices in the matrix
    vector<int> input_ids;
    // Find the number of 0s column, which is the number of inputs in the matrix
    int zero_columns = 0;
    for (int j = 0; j < size; j++) {
        int i = 0;
        while (i < size) {
            if (!(matrix[i][j] == 0)) {
                break;
            }
            i++;
            if (i == 6)     // If j reaches to the end of the column, that means this is a zeros column
                input_ids.push_back(j);     // Add column index as input ID
                zero_columns++;
        }
    }

    // Vector holds the output vertices in the matrix
    vector<int> output_ids;
    // Find the number of 0s rows, which is the number of outputs in the matrix
    int zero_row = 0;
    for (int i = 0; i < size; i++) {
        int j = 0;
        while (j < size) {
            if (!(matrix[i][j] == 0)) {
                break;
            }
            j++;
            if (j == 6)     // If j reaches to the end of the row, that means this is a zero row
                output_ids.push_back(i);
                zero_row++;
        }
    }

    vector<Vertex*> vertices;   // Vector stores the vertices
    // Assign the values to the corresponding input vertices (will be the parent processes) in the matrix
    for (int i = 0; i < input_ids.size(); i++) {
        Vertex *v = new Vertex;
        v->id = input_ids[i];
        v->isInput = true;
        v->isOutput = false;
        v->value = arr[i]; // Get the value from input string array
        vertices.push_back(v);
    }

    // Number of pipes
    int num_pipes = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] == 1)
                num_pipes++;
        }
    }


    int limit = size;   // The number of time to fork equals to the size of matrix
    // The limit of fork times will be = size + 1 if there're more than 1 output in the matrix
    if (zero_row > 1) 
        limit += 1;

    for (int i = 0; i < limit; i++) {
        int pnum = -1;
        // Start piping
        int fd[i][2];
        
        if (pipe(fd[2]) == -1) {
            perror("Pipes failed");
            return 1;
        }

        // Start forking
        pid_t p = fork();
        if (p == -1) {
            perror("Fork failed");
            return 1;
        }
        // Child process
        else if (p == 0) {
            pnum = i;
            close(fd[i][1]);    // Close the write end of the pipe
            
            read(fd[i][0], vertices[i], sizeof(vertices[i])); // Read the string from the input
            close(fd[i][0]); // Close the read end of the pipe
            
            write(fd[i][1], vertices[i], sizeof(vertices[i])); // Write the string to the next vertex
            close(fd[i][1]); // Close the write end of the pipe
            break;
        }
        // Parent process
        else {
            close(fd[i][0]); // Close the read end of the pipe
            
            write(fd[i][1], vertices[i], sizeof(vertices[i])); // Write the string to the next vertex
            close(fd[i][1]); // Close the write end of the pipe
            
            wait(NULL); // Wait for child to send the string

            read(fd[i][0], vertices[i], sizeof(vertices[i]));   // Read the string from the input
            close(fd[i][0]); // Close the read end of the pipe
        }
    }
    // for (int i = 0; i < num_pipes; i++) {
        

    // }

    // // Check the connection between the vertices
    // for (int i = 0; i < size; i++) {
    //     for (int j = 0; j < size; j++) {
    //         if (matrix[i][j] == 1) {
    //             Vertex *v = new Vertex;
    //             v->id = j;
    //             v->isOutput = true;
    //             v->inputId = i;
    //             vertices.push_back(v);
    //             // Check the connected status of the vertex
    //             // cout << v->inputId << " - " << v->value << " is input of " << v->id << endl;
    //         }
    //     }
    // }
    
    // // for (int i = 0; i < vertices.size(); i++) {
    // //     if (vertices[i]->inputId != 0) {
    // //         cout << vertices[i]->id << " - " << vertices[i]->value << " is output of " << vertices[i]->inputId << endl;
    // //     }
    // //     else cout << vertices[i]->id << " - " << vertices[i]->value << " is input of " << vertices[i]->outputId << endl;
    // // }


    return 0;
}