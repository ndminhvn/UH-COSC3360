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
    int id;                          // Id of the vertex
    string value;                    // Value of the vertex
    vector<int> input_pipes_ids;     // Inputs of the vertex (the pipeID)
    vector<int> output_pipes_ids;    // Outputs of the vertex (the pipeID)
    bool isInput;                    // Is the input vertex
    bool isOutput;                   // Is the output vertex
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

    // // Vector holds the output vertices in the matrix
    // vector<int> output_ids;
    // Find the number of 0s rows, which is the number of outputs in the matrix
    int zero_rows = 0;
    for (int i = 0; i < size; i++) {
        int j = 0;
        while (j < size) {
            if (!(matrix[i][j] == 0)) {
                break;
            }
            j++;
            if (j == 6)     // If j reaches to the end of the row, that means this is a zero row
                // output_ids.push_back(i);
                zero_rows++;
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

    // // Add the input pipes ids of each vertex
    // int count_in = 0; // Count the number of 1 in the matrix
    // for (int j = 0; j < size; j++) {
    //     for (int i = 0; i < size; i++) {
    //         if (matrix[i][j] == 1) {
    //             count_in++;
    //             vertices[j]->input_pipes_ids.push_back(count_in);     // Add the input pipe ids
    //         }
    //     }
    // }

    // Add the output pipes ids of each vertex
    int count_out = 0; // Count the number of 1 in the matrix
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] == 1) {
                count_out++;
                vertices[i]->output_pipes_ids.push_back(count_out);     // Add the output pipe ids
            }
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
    int fd[num_pipes][2];
    for (int i = 1; i <= num_pipes; i++) {
        // int fd[i][2];
        if (pipe(fd[i]) == -1) {
            perror("Pipes failed");
            return 1;
        }
    }

    int limit = size;   // The number of time to fork equals to the size of matrix
    // The limit of fork times will be = size + 1 if there're more than 1 output in the matrix
    if (zero_rows > 1) 
        limit += 1;

    // Start forking
    int pipe_id = 0;
    for (int i = 1; i <= limit; i++) {
        pid_t p = fork();
        if (p == -1) {
            perror("Fork failed");
            return 1;
        }
        // Parent process
        else if (p > 0) {
            char buffer[100];

            read(fd[i][0], buffer, 100);  // Read from string input
            close(fd[i][0]);              // Close the read end of the pipe

            for (int j = 0; j < vertices.size(); j++) {
                if (vertices[j]->isInput == true) {
                    string s = vertices[j]->value;
                    char char_buffer[s.length() + 1];
                    write(fd[i][1], strcpy(char_buffer, s.c_str()), s.length() + 1);
                }
            }
        }
        // Child process
        else {
            pipe_id = i;   // Pipe ID
            char buffer[100];

            read(fd[pipe_id][0], buffer, 100); // Read from pipe
            close(fd[pipe_id][0]); // Close the read end of the pipe
            
            // Send the string to the next process  
            // need to process the string
            string s = vertices[i-1]->value;
            char char_buffer[s.length() + 1];

            for (int k = 0; k < vertices.size(); k++) {
                for (int h = 0; h < vertices[k]->output_pipes_ids.size(); h++) {
                    if (pipe_id == vertices[k]->output_pipes_ids[h]) {
                        write(fd[pipe_id][1], strcpy(char_buffer, s.c_str()), s.length() + 1);
                    }
                }
            }
            close(fd[pipe_id][1]); // Close the write end of the pipe
            break;
        }
    }

    return 0;
}