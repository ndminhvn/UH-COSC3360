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
    vector<int> input_pipes_ids;     // Inputs of the vertex (the pipeID)
    vector<int> output_pipes_ids;    // Outputs of the vertex (the pipeID)
    bool isInput;   // Is the vertex input of another vertex
    bool isOutput;  // Is the vertex output of another vertex
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
        // v->inputs.push_back(-1);    // This vertex will read input from string file
        // v->value = arr[i]; // Get the value from input string array
        vertices.push_back(v);
    }

    // Add all the vertices to the vertices vector
    for (int i = 0; i < size; i++) {
        if (i < vertices.size() && i == vertices[i]->id) {
            continue;
        }
        else {
            Vertex *v = new Vertex;
            v->id = i;
            vertices.push_back(v);
        }
    }

    // Number of pipes
    int num_1s = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] == 1)
                num_1s++;
        }
    }
    int num_pipes = num_1s + 1;


    // Making all the pipes
    for (int i = 0; i < num_pipes; i++) {
        int fd[i][2];
        if (pipe(fd[2]) == -1) {
            perror("Pipes failed");
            return 1;
        }
    }

    int limit = size;   // The number of time to fork equals to the size of matrix
    // The limit of fork times will be = size + 1 if there're more than 1 output in the matrix
    if (zero_row > 1) 
        limit += 1;

    int pnum = -1;
    for (int i = 0; i < limit; i++) {

        // Start forking
        pid_t p = fork();
        if (p == -1) {
            perror("Fork failed");
            return 1;
        }
        // Parent process
        // else if (p > 0) {
        //     cout << "pnum = " << pnum << endl;
        //     cout << p << endl;
        //     close(fd[i][0]); // Close the read end of the pipe
            
        //     write(fd[i][1], vertices[i], sizeof(vertices[i])); // Write the string to the next vertex
        //     close(fd[i][1]); // Close the write end of the pipe

        //     read(fd[i][0], vertices[i], sizeof(vertices[i]));   // Read the string from the input
        //     close(fd[i][0]); // Close the read end of the pipe
        // }
        // // Child process
        // else {
        //     pnum = i;
        //     cout << "pnum = " << pnum << endl;
        //     cout << p << endl;
        //     close(fd[i][1]);    // Close the write end of the pipe
            
        //     read(fd[i][0], vertices[i], sizeof(vertices[i])); // Read the string from the input
        //     close(fd[i][0]); // Close the read end of the pipe
            
        //     write(fd[i][1], vertices[i], sizeof(vertices[i])); // Write the string to the next vertex
        //     cout << vertices[i+1] << endl;
        //     close(fd[i][1]); // Close the write end of the pipe
        //     break;
        // }
    }

    return 0;
}