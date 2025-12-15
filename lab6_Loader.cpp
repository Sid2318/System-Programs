#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

// Simple memory: address -> instruction code
map<int, string> memory;

// Absolute Loader - loads at exact addresses from file
void absoluteLoader(string filename) {
    ifstream file(filename);
    
    cout << "\n=== ABSOLUTE LOADER ===\n";
    
    string line;
    string programName;
    int startAddr = 0;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        char recordType = line[0];
        
        // H record: Header (program name and start address)
        if (recordType == 'H') {
            programName = line.substr(2, 6);
            startAddr = stoi(line.substr(9, 4), nullptr, 16); // hex to int
            
            cout << "Program: " << programName << endl;
            cout << "Start: " << startAddr << endl;
        }
        // T record: Text (actual machine code)
        else if (recordType == 'T') {
            int loadAddr = stoi(line.substr(2, 4), nullptr, 16);
            int length = stoi(line.substr(7, 2), nullptr, 16);
            
            cout << "Loading at address " << loadAddr << ": ";
            
            // Read machine code bytes (starting from position 10)
            int pos = 10;
            for (int i = 0; i < length && pos < line.length(); i++) {
                string code = line.substr(pos, 2);
                memory[loadAddr + i] = code;
                cout << code << " ";
                pos += 3; // skip space
            }
            cout << endl;
        }
        // E record: End (execution start address)
        else if (recordType == 'E') {
            int execAddr = stoi(line.substr(2, 4), nullptr, 16);
            cout << "Execution starts at: " << execAddr << endl;
        }
    }
    
    file.close();
}

// Relocating Loader - loads at different address by adding offset
void relocatingLoader(string filename, int offset) {
    ifstream file(filename);
    
    cout << "\n=== RELOCATING LOADER ===\n";
    cout << "Relocation offset: " << offset << endl;
    
    string line;
    string programName;
    int startAddr = 0;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        char recordType = line[0];
        
        // H record: Header
        if (recordType == 'H') {
            programName = line.substr(2, 6);
            startAddr = stoi(line.substr(9, 4), nullptr, 16);
            
            cout << "Program: " << programName << endl;
            cout << "Original start: " << startAddr << endl;
            cout << "New start: " << (startAddr + offset) << endl;
        }
        // T record: Text - load at new address (original + offset)
        else if (recordType == 'T') {
            int loadAddr = stoi(line.substr(2, 4), nullptr, 16);
            int newAddr = loadAddr + offset; // relocate!
            int length = stoi(line.substr(7, 2), nullptr, 16);
            
            cout << "Loading at address " << newAddr << ": ";
            
            int pos = 10;
            for (int i = 0; i < length && pos < line.length(); i++) {
                string code = line.substr(pos, 2);
                memory[newAddr + i] = code;
                cout << code << " ";
                pos += 3;
            }
            cout << endl;
        }
        // E record: End
        else if (recordType == 'E') {
            int execAddr = stoi(line.substr(2, 4), nullptr, 16);
            cout << "Execution starts at: " << (execAddr + offset) << endl;
        }
    }
    
    file.close();
}

// Show what's loaded in memory
void showMemory() {
    cout << "\n=== MEMORY MAP ===\n";
    cout << "Address\tCode\n";
    
    for (auto& pair : memory) {
        cout << pair.first << "\t" << pair.second << endl;
    }
}

int main() {
    cout << "Simple Loader\n";
    cout << "=============\n";
    
    int choice;
    cout << "\n1. Absolute Loader (load at original address)\n";
    cout << "2. Relocating Loader (load at new address)\n";
    cout << "Choice: ";
    cin >> choice;
    
    string filename = "lab6input.obj";
    
    if (choice == 1) {
        absoluteLoader(filename);
    } 
    else if (choice == 2) {
        int offset;
        cout << "Enter offset (e.g., 100): ";
        cin >> offset;
        relocatingLoader(filename, offset);
    }
    
    showMemory();
    
    return 0;
}
