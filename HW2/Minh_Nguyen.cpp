#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

// Variables
int numResources = 0;					//	The total number of resource nodes
int numProcesses = 0;					//	The total number of processes
struct Resource	{						//	Structure of the resource node
	int ID;								//  ID of the resource
	int available;						//  Number of resource instances
	string resourceType;				//	Resource type
	vector<string> resourceInstances;	//	Vector of resource instances	
};

Resource *resources;					//	Array that contains all the resource nodes

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
int numProcessesRemaining;				//	The number of processes remaining to process

int getValueFromLine(string inputString) {
	//	Find the first '=' sign which indicates that the desired value is after it
	int pos = inputString.find("=");

	//	Create a sub string for everything after the '=' sign
	string sub = inputString.substr(pos + 1);

	return stoi(sub);
}

void readFromFile(string fileName, string fileWordName) {
	fstream inputFile(fileName);
	fstream inputFile1(fileWordName);	

	if (inputFile.is_open()) {
		// cout << "\n" << "Opened file: " << fileName << "\n\n";

		// Get the first line
		string currentLine;

		// Get the number of resources
		getline(inputFile, currentLine);
		numResources = stoi(currentLine);
		
		//	Initialize size of resources array
		resources = new Resource[numResources];
		// cout << "Total Resources: " << numResources << endl;	//DEBUG

		// Get the amount of processes
		getline(inputFile, currentLine);
		numProcesses = stoi(currentLine);
		
		// Initialize size of processes array
		processes = new Process[numProcesses];
		// cout << "Total Processes: " << numProcesses << "\n\n";	//DEBUG

		// Get the ID and amount of resources each resource has
		for (int i = 0; i < numResources; i++) {
			getline(inputFile, currentLine);

			//	Create new resource struct and add it to array of resources
			Resource resource;
			resources[i] = resource;
			resources[i].ID = i + 1;	// Resource ID starts from 1
			resources[i].available = getValueFromLine(currentLine);
			// cout << "Resource " << resources[i].ID << " has " << resources[i].available << " instances of resources." << endl;	// DEBUG
		}

		// Get resource instances from fileWordName file
		if (inputFile1.is_open()) {
			string line;
			string id, type, instance;

			// Get the resource instances from the input file
			for (int i = 0; i < numResources; i++) {
				// Get the resource type
				inputFile1 >> id >> resources[i].resourceType;
				// cout << "Resource type: " << resources[i].resourceType << endl;
				// Loop through the resource instances vector and push_back to the vector
				for (int j = 0; j < resources[i].available; j++) {
					inputFile1 >> instance;
					resources[i].resourceInstances.push_back(instance);
					// cout << resources[i].resourceInstances[j] << " ";
				}
				// cout << endl;
			}
			inputFile1.close();
		}
		else if (!inputFile1.is_open()) {
			perror("File not found.\n");
			exit(0);
		}

		// cout << endl;	//	Skip a line - Used for DEBUG 

		// Get the size of resource related parameters for the process
		for (int i = 0; i < numProcesses; i++) {
			processes[i].allocatedResources = new int[numResources];
			processes[i].maxResources = new int[numResources];
			processes[i].neededResources = new int[numResources];
		}
		
		// Get the value of the max value processor can demand from each resource
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

		// Loop through each process
		for (int i = 0; i < numProcesses; i++) {
			// Skip all lines until next process
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

			// Get the instruction amount for this process
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

			// Loop through instructions
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
		perror("File not found.\n");
		exit(0);
	}
}

void processesSortingByDeadline(Process arr[], int left, int right) {
	// Quick Sort algorithm
	int i = left, j = right;
	Process temp;
	Process pivot = arr[(left + right) / 2];

	//	Partioning
	while (i <= j) {
		while (arr[i].deadline < pivot.deadline)
			i++;
		while (arr[j].deadline > pivot.deadline)
			j--;
		while (arr[j].deadline == pivot.deadline && arr[j].computeTime < pivot.computeTime)
			j--;

		if (i <= j) {
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			i++;
			j--;
		}
	}

	//	Recursion
	if (left < j)
		processesSortingByDeadline(arr, left, j);
	if (i < right)
		processesSortingByDeadline(arr, i, right);
}

void createPipes() {
	// cout << "Creating pipes for Processes...\n";
	for (int i = 0; i < numProcesses; i++) {
		// If process failed to create a pipe then exit
		if (pipe(processes[i].pipe_ChildSendToParent) == -1) {
			perror("ERROR: Pipe from Child to Parent failed.");
			exit(0);
		}
		if (pipe(processes[i].pipe_ParentSendToChild) == -1) {
			perror("ERROR: Pipe from Parent to Child failed.");
			exit(0);
		}
		// cout << " Pipe created for Process " << processes[i].ID << endl;	// DEBUG
	}

	// Initialize buffer size of char array for pipe
	bufferLength = sizeof(buffer) / sizeof(buffer[0]);
	// cout << "Character buffer length for pipes: " << bufferLength << endl;	// DEBUG
}

bool isSafe() {
	return true;
}

void request(string message, Process process, int requestInts[])
{
	// Create string for request ints
	string requestValues = "(";
	for (int i = 0; i < numResources; i++) {
		requestValues += to_string(requestInts[i]);
		if (i < numResources - 1)
			requestValues += ",";
	}
	requestValues += ")";

	cout << "Requesting " << requestValues << " resources for Process: " << process.ID << "..." << endl;

	// Cache original resource incase of failure
	int tempResourceArray[numResources];
	// Copy the array to the temp array
	memcpy(tempResourceArray, process.allocatedResources, numResources);
	
	// Banker's Algorithm for deadlock avoidance for each resource request
	for (int i = 0; i < numResources; i++) {
		// If the requested resources is higher then the need
		if (requestInts[i] > process.neededResources[i]) {
			// Reset the allocatedResources to their original values
			process.allocatedResources = tempResourceArray;
			
			cout << "Process " << process.ID << " is requesting more resources than it needs from resource " << i + 1 << ". Process is terminated.\n";
			
			// Send termination message
			write(process.pipe_ChildSendToParent[1], "TERMINATE", bufferLength);
			return;
		}
		// If the request is more then the available...
		else if (requestInts[i] > resources[i].available) {
			// Process waits
			cout << "Process " << process.ID << " is requesting more resources than available. Process waits." << endl;
			
			// Send wait message
			//write(process.pipe_ParentSendToChild[1], "WAIT", bufferLength);
			break;
		}
	}

	// No errors in allocation, begin the actual allocation.
	for (int i = 0; i < numResources; i++) {
		resources[i].available -= requestInts[i];
		process.allocatedResources[i] += requestInts[i];
		process.neededResources -= requestInts[i];
	}
	
	// Create a string of the array amount of allocated resources in process
	string allocatedValues = "(";
	for (int i = 0; i < numResources; i++)
	{
		allocatedValues += to_string(process.allocatedResources[i]);
		if (i < numResources - 1)
			allocatedValues += ",";
	}
	allocatedValues += ")";

	cout << "Process " << process.ID << " now has " << allocatedValues << " allocated resources." << endl;

	// Display amount of availiable resources in Resource
	for (int i = 0; i < numResources; i++)
		cout << "Resource " << resources[i].ID << " now has " << resources[i].available << " availiable resources." << endl;

	// Safe check
	if (isSafe()) {
		// Complete transaction
		cout << "Process " << process.ID << " is safe. Instruction completed." << endl;
	}
	else {
		// Process must wait
		cout << "Process " << process.ID << " is not safe. Process is waiting." << endl;
		return;
	}
	
	cout << message << " instruction complete message written to Process " << process.ID << endl;
	
	message += "=SUCCESS";
	
	//close(process.pipe_ParentWriteToChild[0]);
	write(process.pipe_ParentSendToChild[1], message.c_str(), bufferLength);
}

void release(string message, Process process, int releaseInts[]) {
	// Create string for release ints and display it
	string releaseValues = "(";
	for (int i = 0; i < numResources; i++) {
		releaseValues += to_string(releaseInts[i]);
		if (i < numResources - 1)
			releaseValues += ",";
	}
	releaseValues += ")";
	
	cout << "Releasing " << releaseValues << " resources for Process: " << process.ID << "..." << endl;
	
	// Loop through each release amount
	for (int i = 0; i < numResources; i++) {
		resources[i].available += releaseInts[i];
		process.allocatedResources[i] -= releaseInts[i];
		process.neededResources += releaseInts[i];
	}

	// Create a string of the array amount of allocated resources in process
	string allocatedValues = "(";
	for (int i = 0; i < numResources; i++) {
		allocatedValues += to_string(process.allocatedResources[i]);
		if (i < numResources - 1)
			allocatedValues += ",";
	}
	allocatedValues += ")";

	cout << "Process " << process.ID << " now has " << allocatedValues << " allocated resources." << endl;

	//	Display amount of availiable resources in Resource
	for (int i = 0; i < numResources; i++)
		cout << "Resource " << resources[i].ID << " now has " << resources[i].available << " availiable resources." << endl;

	cout << message << " instruction complete message written to Process " << process.ID << endl;
	message += "=SUCCESS";

	write(process.pipe_ParentSendToChild[1], message.c_str(), bufferLength);
}

void use_resources(string message, Process process, int amount) {
	cout << "Using resources for Process: " << process.ID << "..." << endl;
	
	cout << "Process " << process.ID << " used (" << amount << ") allocated resources." << endl;
	
	cout << message << " instruction complete message written to Process " << process.ID << endl;
	message += "=SUCCESS";

	write(process.pipe_ParentSendToChild[1], message.c_str(), bufferLength);
}

void print_resources_used(Process process, string message) {
	if (message.find("request") != string::npos) {
		cout << "Main Process received instruction: " << message << " from Process " << process.ID << endl;

		//	Find all integers in the message and store it as an array
		int intsInMessage[numResources];
		int intIndexInString = 0;
		string number;
		for (int i = 0; i < message.length(); i++) {
			if (isdigit(message[i])) {	
				//found a digit, get the int
				for (int j = i; ; j++) {
					if (isdigit(message[j]))		//consecutive digits
						number += message[j];
					else {
						i = j - 1;		//set i to the index of the last digit
						break;
					}
				}
				intsInMessage[intIndexInString] = stoi(number.c_str());
				number = "";
				intIndexInString++;
			}
		}
		//	Perform request function and pass the parameter
		request(message, process, intsInMessage);
	}
	else if (message.find("release") != string::npos) {
		cout << "Main Process received instruction: " << message << " from Process " << process.ID << endl;
		
		//	Find all integers in the message and store it as an array
		int intsInMessage[numResources];
		int intIndexInString = 0;
		string number;
		for (int i = 0; i < message.length(); i++) {
			if (isdigit(message[i])) {	//found a digit, get the int
				for (int j = i; ; j++) {
					if (isdigit(message[j]))		//consecutive digits
						number += message[j];
					else {
						i = j - 1;		//set i to the index of the last digit
						break;
					}
				}
				intsInMessage[intIndexInString] = stoi(number.c_str());
				number = "";
				intIndexInString++;
			}
		}

		// Perform release function and pass the parameter
		release(message, process, intsInMessage);
	}
	else if (message.find("use_resources") != string::npos) {
		cout << "Main Process received instruction: " << message << " from Process " << process.ID << endl;
		
		use_resources(message, process, 1);
	}
	// If process sent termination message... tell main process that it has been terminated
	else if (message.find("TERMINATED") != string::npos) {
		numProcessesRemaining--;
	}
	// Else, message is invalid... Terminate the child process
	else {
		write(process.pipe_ChildSendToParent[1], "TERMINATE", bufferLength);

		// cout << "ERROR: invalid instruction message." << endl;
		exit(0);
	}
}

int main() {
    readFromFile("many.txt","many_words.txt");
	cout << "Done reading file\n";

	processesSortingByDeadline(processes, 0, numProcesses - 1);
	// cout << "Sorting done.\n";

	createPipes();
	// cout << "Pipes created.\n";

	int pid;				// Process ID
	int mainID = getpid();	// Main Process ID

	Process currentProcess;
	for (int i = 0; i < numProcesses; i++) {
		currentProcess = processes[i];
		pid = fork();

		if (mainID != getpid()) {
			break;
		}
	}

	if (pid == -1) {
		perror("ERROR: Fork failed.");
		exit(0);
	}

	// Child process
	else if (pid == 0) {
		cout << "Forked Child Process: " << currentProcess.ID << endl << endl;

		//	Loop through each instruction and process it to Main Process
		for (int i = 0; i < sizeof(currentProcess.instructions); i++) {
			close(currentProcess.pipe_ChildSendToParent[0]);
			write(currentProcess.pipe_ChildSendToParent[1], currentProcess.instructions[i].c_str(), bufferLength);

			cout << "Process " << currentProcess.ID << " sent instruction: " << currentProcess.instructions[i] << endl;		
			
			close(currentProcess.pipe_ParentSendToChild[1]);

			while (true) {
				read(currentProcess.pipe_ParentSendToChild[0], buffer, bufferLength);

				string instructionMessage = buffer;
				
				if (instructionMessage.find("SUCCESS") != string::npos) {
					cout << "Process " << currentProcess.ID << " completed instruction: " << instructionMessage << endl << endl;
					break;
				}
				else if (instructionMessage.find("TERMINATE") != string::npos) {
					cout << "Process " << currentProcess.ID << " terminated." << endl;
					write(currentProcess.pipe_ChildSendToParent[1], "TERMINATED", bufferLength);
					exit(0);
				}
				else if (instructionMessage.find("WAIT") != string::npos) {
					cout << "Process " << currentProcess.ID << " is trying to send instruction: " << currentProcess.instructions[i] << " again." << endl;
				}
				else
					cout << "Process " << currentProcess.ID << " is listening..." << endl;
			}
		}

		//	Send termination message
		write(currentProcess.pipe_ChildSendToParent[1], "TERMINATED", bufferLength);
		
		cout << "Process " << currentProcess.ID << " has no more instructions. Process terminated." << endl;

		exit(0);
	}
	//	Process Parent Process
	else {
		numProcessesRemaining = numProcesses;
		while(numProcessesRemaining > 0) {			
			for (int i = 0; i < numProcesses; i++) {	
				read(processes[i].pipe_ChildSendToParent[0], buffer, bufferLength);

				string instructionMessage = buffer;
				//	if the read buffer is not empty... evaluate message 
				if (sizeof(instructionMessage) > 0)
					print_resources_used(processes[i], instructionMessage);
			}
		}
	}

	cout << "\nNo more instructions left to process. Main Process terminating..." << endl;

    return 0;
}