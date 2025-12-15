#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

// Structure to hold information about each object module
struct Module {
    string name;
    int startAddress;
    int length;
    vector<string> code;           // actual code lines
    map<string, int> defTable;     // symbols defined in this module
    vector<pair<string, int>> useTable;  // symbols used (external references)
};

// Global symbol table - stores all defined symbols across modules
map<string, int> globalSymbolTable;

// Function to remove extra spaces from a line
string clean(const string& str) {
    string result = "";
    bool lastWasSpace = false;
    
    for(int i = 0; i < str.length(); i++) {
        if(str[i] == ' ' || str[i] == '\t') {
            if(!lastWasSpace && result.length() > 0) {
                result += ' ';
                lastWasSpace = true;
            }
        } else {
            result += str[i];
            lastWasSpace = false;
        }
    }
    return result;
}

// Function to split a string by space
vector<string> split(const string& line) {
    vector<string> tokens;
    string word = "";
    
    for(int i = 0; i < line.length(); i++) {
        if(line[i] == ' ') {
            if(word.length() > 0) {
                tokens.push_back(word);
                word = "";
            }
        } else {
            word += line[i];
        }
    }
    
    if(word.length() > 0) {
        tokens.push_back(word);
    }
    
    return tokens;
}

// Convert string to integer
int toInt(const string& str) {
    int num = 0;
    for(int i = 0; i < str.length(); i++) {
        num = num * 10 + (str[i] - '0');
    }
    return num;
}

// Pass 1: Build global symbol table and assign addresses
void passOne(vector<Module>& modules) {
    int currentAddress = 0;
    
    cout << "\n===== PASS 1: Building Global Symbol Table =====\n";
    
    // Process each module
    for(int i = 0; i < modules.size(); i++) {
        modules[i].startAddress = currentAddress;
        
        cout << "\nModule: " << modules[i].name << endl;
        cout << "Start Address: " << currentAddress << endl;
        cout << "Length: " << modules[i].length << endl;
        
        // Add defined symbols to global symbol table
        for(auto it = modules[i].defTable.begin(); it != modules[i].defTable.end(); it++) {
            string symbol = it->first;
            int offset = it->second;
            int actualAddress = currentAddress + offset;
            
            // Check if symbol already exists
            if(globalSymbolTable.find(symbol) != globalSymbolTable.end()) {
                cout << "Warning: Symbol " << symbol << " already defined. Ignoring redefinition.\n";
            } else {
                globalSymbolTable[symbol] = actualAddress;
                cout << "Defined symbol: " << symbol << " at address " << actualAddress << endl;
            }
        }
        
        // Update current address for next module
        currentAddress += modules[i].length;
    }
    
    cout << "\n--- Global Symbol Table ---\n";
    for(auto it = globalSymbolTable.begin(); it != globalSymbolTable.end(); it++) {
        cout << it->first << " -> " << it->second << endl;
    }
}

// Pass 2: Link and relocate code
void passTwo(vector<Module>& modules) {
    cout << "\n\n===== PASS 2: Linking and Relocation =====\n";
    
    vector<string> linkedCode;
    
    // Process each module
    for(int i = 0; i < modules.size(); i++) {
        cout << "\nProcessing Module: " << modules[i].name << endl;
        
        // Process each code line in the module
        for(int j = 0; j < modules[i].code.size(); j++) {
            string line = modules[i].code[j];
            vector<string> parts = split(line);
            
            if(parts.size() < 2) {
                linkedCode.push_back(line);
                continue;
            }
            
            string instruction = parts[0];
            string operand = parts[1];
            
            // Check if operand is a symbol that needs to be resolved
            bool isSymbol = false;
            for(int k = 0; k < operand.length(); k++) {
                if((operand[k] >= 'A' && operand[k] <= 'Z') || 
                   (operand[k] >= 'a' && operand[k] <= 'z')) {
                    isSymbol = true;
                    break;
                }
            }
            
            string resolvedLine = instruction + " ";
            
            if(isSymbol) {
                // Look up symbol in global symbol table
                if(globalSymbolTable.find(operand) != globalSymbolTable.end()) {
                    int address = globalSymbolTable[operand];
                    cout << "Resolved " << operand << " to address " << address << endl;
                    
                    // Convert address to string
                    string addrStr = "";
                    if(address == 0) {
                        addrStr = "0";
                    } else {
                        int temp = address;
                        while(temp > 0) {
                            addrStr = (char)('0' + temp % 10) + addrStr;
                            temp /= 10;
                        }
                    }
                    resolvedLine += addrStr;
                } else {
                    cout << "Error: Undefined symbol " << operand << endl;
                    resolvedLine += operand + "(UNDEFINED)";
                }
            } else {
                // Check if it's a relocatable address (marked with +R or similar)
                if(parts.size() > 2 && parts[2] == "R") {
                    int offset = toInt(operand);
                    int relocatedAddr = modules[i].startAddress + offset;
                    
                    cout << "Relocated address " << offset << " to " << relocatedAddr << endl;
                    
                    // Convert to string
                    string addrStr = "";
                    if(relocatedAddr == 0) {
                        addrStr = "0";
                    } else {
                        int temp = relocatedAddr;
                        while(temp > 0) {
                            addrStr = (char)('0' + temp % 10) + addrStr;
                            temp /= 10;
                        }
                    }
                    resolvedLine += addrStr;
                } else {
                    // Absolute address - no relocation needed
                    resolvedLine += operand;
                }
            }
            
            linkedCode.push_back(resolvedLine);
        }
    }
    
    // Display final linked code
    cout << "\n\n===== FINAL LINKED CODE =====\n";
    int addr = 0;
    for(int i = 0; i < linkedCode.size(); i++) {
        cout << addr << ":\t" << linkedCode[i] << endl;
        addr++;
    }
}

int main() {
    // Sample input: simulating two object modules
    
    vector<Module> modules;
    
    // Module 1
    Module m1;
    m1.name = "MODULE_A";
    m1.length = 4;
    
    // Definition table: symbols defined in this module
    m1.defTable["START"] = 0;
    m1.defTable["LOOP"] = 2;
    
    // Code with symbolic references
    m1.code.push_back("LOAD X");      // External reference to X
    m1.code.push_back("ADD Y");       // External reference to Y
    m1.code.push_back("STORE Z");     // External reference to Z
    m1.code.push_back("JMP LOOP");    // Internal reference
    
    modules.push_back(m1);
    
    // Module 2
    Module m2;
    m2.name = "MODULE_B";
    m2.length = 3;
    
    // Definition table
    m2.defTable["X"] = 0;
    m2.defTable["Y"] = 1;
    m2.defTable["Z"] = 2;
    
    // Code
    m2.code.push_back("DATA 100");    // X's value
    m2.code.push_back("DATA 200");    // Y's value
    m2.code.push_back("DATA 0");      // Z's value
    
    modules.push_back(m2);
    
    cout << "========== TWO-PASS LINKER ==========\n";
    
    // Execute Pass 1
    passOne(modules);
    
    // Execute Pass 2
    passTwo(modules);
    
    cout << "\n========== LINKING COMPLETE ==========\n";
    
    return 0;
}
