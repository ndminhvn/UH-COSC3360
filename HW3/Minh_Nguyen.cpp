#include <iostream>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;

// Variables
int tp = 0;         // Total number of page frames in main memory
int ps = 0;         // Page size (in number of bytes)

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
                numPageFaults++;            // Increase the numPageFaults
            } 
            // The set is full, use FIFO
            else {
                int temp = pQueue.front();
                pQueue.pop();
                set.erase(temp);
                set.insert(pages[i]);
                pQueue.push(pages[i]);
                numPageFaults++;
            }
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

int main() {
    int pages[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int n = sizeof(pages)/sizeof(pages[0]);
    int capacity = 4;
    cout << FIFO(pages, n, capacity);
    return 0;
}