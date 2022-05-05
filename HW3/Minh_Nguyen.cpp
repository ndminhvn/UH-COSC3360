#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <queue>
#include <map>
#include <climits>

using namespace std;

// Variables
int tp = 0;         // Total number of page frames in main memory
int ps = 0;         // Page size (in number of bytes)
int r = 0;          // Number of page frames per process for algorithms

// Classes
struct Process {
    int ID;         // Process ID
    bool deactivated;
    bool activated;
    int numProcesses; // Number of processes
    string address;
};

Process *processes; // Array of processes

void readFromFile(string fileName) {
    fstream inputFile(fileName);
    if (inputFile.is_open()) {
        string line;

        // Get the total number of page frames in main memory
        getline(inputFile, line);
        tp = stoi(line);

        // Get the number of page size in bytes
        getline(inputFile, line);
        ps = stoi(line);

        // Get the number of page frames per process for FIFO, LRU, LRU-K, LFU, and OPT
        getline(inputFile, line);
        r = stoi(line);

    }
    else {
        cout << "File " << fileName << " not found." << endl;
    }
}

int FIFO(int pages[], int size, int frames) {
    int numPageFaults = 0;

    // Set of pages
    unordered_set<int> set;

    // Pages stored in a queue
    queue<int> pQueue;

    for (int i = 0; i < size; i++) {
        // If the current page is not in the set (pageFault)
        if (set.find(pages[i]) == set.end()) {
            // If there is still some empty slots
            if (set.size() < frames) {
                set.insert(pages[i]);       // Insert the page into the set
                pQueue.push(pages[i]);      // Insert the page into the queue
            } 
            // The set is full, use FIFO
            else {
                int temp = pQueue.front();
                pQueue.pop();
                set.erase(temp);
                set.insert(pages[i]);
                pQueue.push(pages[i]);
            }
            numPageFaults++;            // Increase the numPageFaults
        }
    }
    return numPageFaults;
}

// Reference: https://www.geeksforgeeks.org/program-for-least-recently-used-lru-page-replacement-algorithm/
int LRU(int pages[], int size, int frames) {
    int numPageFaults = 0;

    // Set of pages
    unordered_set<int> set;

    // Store the least recently used pages
    map<int, int> pMap;

    for (int i = 0; i < size; i++) {
        // If the current page is not in the set (pageFault)
        if (set.find(pages[i]) == set.end()) {
            // If there is still some empty slots
            if (set.size() < frames) {
                set.insert(pages[i]);       // Insert the page into the set
            }
            // If the set is full, use LRU (remove the least recently used page and insert the current one)
            else {
                int lru = INT_MAX, temp;
                for (auto it : set) {
                    if (pMap[it] < lru) {
                        lru = pMap[it];
                        temp = it;
                    }
                }
 
                // Remove the least recently used page
                set.erase(temp);
                // Insert the current page
                set.insert(pages[i]);
            }

            numPageFaults++;                // Increase the numPageFaults
            pMap[pages[i]] = i;             // Store the recently used page
        }
    }
    return numPageFaults;
}

int LRU_K(int pages[], int size, int capacity) {
    int numPageFaults = 0;

    return numPageFaults;
}

int LFU(int pages[], int size, int capacity) {
    int numPageFaults = 0;

    set<int> entries;

    map<int, int> pMap;       // Frequency map

    for (int i = 0; i < size; i++) {
        // If the element is already present -> remove it -> add to the end
        if (entries.find(pages[i]) != entries.end()) {
            pMap[pages[i]]++;
            entries.erase(entries.find(pages[i]));
            entries.insert(pages[i]);
        }
        else {
            if (entries.size() == capacity) {
                // pMap[entries[0]]--;
                entries.erase(entries.begin());
            }
            entries.insert(pages[i]);   // Add it to the end
            pMap[pages[i]]++;           // Increase the frequency
            numPageFaults++;
        }

        // // Compare frequency with other pages starting from the 2nd last page
        // int j = entries.size() - 2;

        // while (pMap[entries[j]] > pMap[entries[j + 1]] && j > -1) {
        //     swap(entries[j + 1], entries[j]);
        //     j--;
        // }
    }
    
    return numPageFaults;
}

int OPT(int pages[], int size, int capacity) {
    int numPageFaults = 0;

    return numPageFaults;
}

int main() {

    /* TEST the FIFO and LRU page replacement algorithms */

    // int pages[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    // int n = sizeof(pages)/sizeof(pages[0]);
    // int capacity = 4;
    // cout << "FIFO TEST: " << FIFO(pages, n, capacity) << endl;
    // cout << "LRU TEST: " << LRU(pages, n, capacity) << endl;

    cout << "Sorry, I can't finish this assignment on time :((\n";
    return 0;
}