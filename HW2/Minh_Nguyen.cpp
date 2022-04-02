#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;

// Variables
int numResources = 0;						//	The total number of resource nodes
int numProcesses = 0;						//	The total number of processes
struct Resource	{						//	Structure of the resource node
	int ID;
	int available;
};

Resource *resources;					//	Array that contains all the structures of the resrouce nodes

struct Process {
	int ID;
	int deadline;
	int computeTime;					//	Integer equal to number of requests and releases plus the parenthesized values in the calculate and useresources instructions.
	int *allocatedResources;			//	The amount of resources that is currently allocated to the process per resource
	int *maxResources;					//	Array representing the max amount of resources a process need to complete process from each resource
	int *neededResources;				//	Array of how much resources needed left to complete execution per resource
	string *instructions;				//	Array of instructions for the processor
	int pipe_ParentSendToChild[2];			
	int pipe_ChildSendToParent[2];			
};

Process *processes;						//	Array that contains all the structures of the processes
char buffer[1000];						//	Character buffer length of write message
int bufferLength;
int instructionsToProcess = 0;			//  Total number of instructions
int numProcessesRemaining;			//	The number of processes remaining to process.

int getValueFromLine(string inputString) {
	//	Find the first '=' sign which indicates that the desired value is after it
	int pos = inputString.find("=");

	//	Create a sub string for everything after the '=' sign
	string sub = inputString.substr(pos + 1);

	return stoi(sub);
}

void readFromFile(string fileName) {
	fstream inputFile(fileName);	

	if (inputFile.is_open()) {
		// cout << "\n" << "Opened file: " << fileName << "\n\n";

		//	Get the first line
		string currentLine;

		//	Get the number of resources
		getline(inputFile, currentLine);
		numResources = stoi(currentLine);
		
		//	Initialize size of resources array
		resources = new Resource[numResources];
		// cout << "Total Resources: " << numResources << endl;	//DEBUG

		//	Get the amount of processes
		getline(inputFile, currentLine);
		numProcesses = stoi(currentLine);
		
		//	Initialize size of processes array
		processes = new Process[numProcesses];
		// cout << "Total Processes: " << numProcesses << "\n\n";	//DEBUG

		//	Get the ID and amount of resources each resource has
		for (int i = 0; i < numResources; i++) {
			getline(inputFile, currentLine);

			//	Create new resource struct and add it to array of resources
			Resource resource;
			resources[i] = resource;
			resources[i].ID = i;
			resources[i].available = getValueFromLine(currentLine);
			// cout << "Resource " << resources[i].ID + 1 << " has " << resources[i].available << " instances of resources." << endl;	// DEBUG
		}

		// cout << endl;	//	Skip a line - Used for DEBUG 

		//	Get the size of resource related parameters for the process
		for (int i = 0; i < numProcesses; i++) {
			processes[i].allocatedResources = new int[numResources];
			processes[i].maxResources = new int[numResources];
			processes[i].neededResources = new int[numResources];
		}
		
		//	Get the value of the max value processor can demand from each resource
		for (int i = 0; i < numProcesses; i++) {
			// cout << "Max resources Process " << i + 1 << " that can demand from:" << endl;
			
			for (int j = 0; j < numResources; j++) {
				getline(inputFile, currentLine);
				processes[i].maxResources[j] = getValueFromLine(currentLine);
				processes[i].neededResources[j] = processes[i].maxResources[j];
				
				// cout << " Resource " << j + 1 << ": " << processes[i].maxResources[j] << endl;	//DEBUG
			}
		}

		// cout << endl;

		//	Loop through each process
		for (int i = 0; i < numProcesses; i++) {
			//	Skip all lines until next process
			while (true) {
				getline(inputFile, currentLine);
				if (currentLine.find("process_") != string::npos)
					break;
			}
			
			// cout << "Fetching parameters for " << currentLine << "..." << endl;	//DEBUG

			// ID
			processes[i].ID = i + 1;

			// Deadline
			getline(inputFile, currentLine);
			processes[i].deadline = stoi(currentLine);
			// cout << "Process " << i + 1 << " deadline: " << processes[i].deadline << endl;
			
			// Compute time
			getline(inputFile, currentLine);
			processes[i].computeTime = stoi(currentLine);
			// cout << "Process " << i + 1 << " compute time: " << processes[i].computeTime << endl;

			//	Get the instruction amount for this process
			int instructionAmount = 0;			
			streampos originalPosition = inputFile.tellg();		//	Cache line position
			
			while (true) {
				getline(inputFile, currentLine);

				//	Break loop if found "end" and assign the length of instructions array
				if (currentLine.find("end") != string::npos) {
					processes[i].instructions = new string[instructionAmount];
					inputFile.seekg(originalPosition, ios::beg);			//	Set the getline back to the original position
					break;
				}
				instructionAmount++;
			}

			// cout << "Process " << i + 1 << " instructions:" << endl;

			//	Loop through instructions
			for (int j = 0; j < instructionAmount; j++) {
				getline(inputFile, currentLine);
				processes[i].instructions[j] = currentLine;

				//	increment the total number of instructions
				instructionsToProcess++;

				// cout << " " << j+1 << ") " << processes[i].instructions[j] << endl;
			}
			// cout << endl;
		}
		inputFile.close();		// Close the input file
	}

	else {
		cout << "File not found.\n";
		exit(0);
	}
}

int main() {
    readFromFile("many.txt");
	cout << "Done reading file\n";
    return 0;
}