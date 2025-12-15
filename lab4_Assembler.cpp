#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;

map<string, string> opcodeTable = {
    {"MOV", "01"},
    {"ADD", "02"},
    {"SUB", "03"},
    {"MUL", "04"},
    {"DIV", "05"},
    {"LOAD", "06"},
    {"STORE", "07"},
    {"JMP", "08"},
    {"JZ", "09"},
    {"HALT", "0A"}
};

map<string, int> symbolTable;

map<string, int> literalTable;

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

// Helper function to check if string is a number
bool isNumber(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// Parse a line into label, opcode, and operands
struct Instruction {
    string label;
    string opcode;
    string operand1;
    string operand2;
    int address;
};

Instruction parseLine(const string& line, int address) {
    Instruction inst;
    inst.address = address;
    
    string trimmedLine = trim(line);
    if (trimmedLine.empty() || trimmedLine[0] == ';') {
        return inst; // Empty or comment line
    }
    
    stringstream ss(trimmedLine);
    string first, second, third;
    ss >> first;
    
    // Check if first token is a label (ends with ':')
    if (first.back() == ':') {
        inst.label = first.substr(0, first.length() - 1);
        ss >> first; // Next token is the opcode
    }
    
    inst.opcode = first;
    
    // Get operands
    if (ss >> second) {
        // Remove comma if present
        if (second.back() == ',') {
            second = second.substr(0, second.length() - 1);
        }
        inst.operand1 = second;
        
        if (ss >> third) {
            inst.operand2 = third;
        }
    }
    
    return inst;
}

// Pass 1: Build symbol table and assign addresses
vector<Instruction> pass1(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        exit(1);
    }
    
    vector<Instruction> instructions;
    string line;
    int locationCounter = 0;
    
    cout << "\n========== PASS 1 ==========\n";
    cout << "LC\tLabel\tOpcode\tOperand1\tOperand2\n";
    cout << "---------------------------------------------------\n";
    
    while (getline(file, line)) {
        Instruction inst = parseLine(line, locationCounter);
        
        if (inst.opcode.empty()) continue; // Skip empty lines
        
        // Add label to symbol table
        if (!inst.label.empty()) {
            symbolTable[inst.label] = locationCounter;
        }
        
        // Handle START directive
        if (inst.opcode == "START") {
            if (!inst.operand1.empty() && isNumber(inst.operand1)) {
                locationCounter = stoi(inst.operand1);
                inst.address = locationCounter;
            }
        }
        // Handle END directive
        else if (inst.opcode == "END") {
            instructions.push_back(inst);
            break;
        }
        // Handle DS (Define Storage)
        else if (inst.opcode == "DS") {
            int size = 1;
            if (!inst.operand1.empty() && isNumber(inst.operand1)) {
                size = stoi(inst.operand1);
            }
            instructions.push_back(inst);
            locationCounter += size;
            continue;
        }
        // Handle DC (Define Constant)
        else if (inst.opcode == "DC") {
            instructions.push_back(inst);
            locationCounter++;
            continue;
        }
        // Regular instruction
        else {
            instructions.push_back(inst);
            locationCounter++;
        }
        
        // Display pass 1 info
        cout << inst.address << "\t"
             << (inst.label.empty() ? "-" : inst.label) << "\t"
             << inst.opcode << "\t"
             << (inst.operand1.empty() ? "-" : inst.operand1) << "\t"
             << (inst.operand2.empty() ? "-" : inst.operand2) << "\n";
    }
    
    file.close();
    return instructions;
}

// Pass 2: Generate machine code
void pass2(const vector<Instruction>& instructions) {
    cout << "\n========== PASS 2 ==========\n";
    cout << "Address\tLabel\tOpcode\tOperand\tMachine Code\n";
    cout << "---------------------------------------------------\n";
    
    for (const auto& inst : instructions) {
        string machineCode = "";
        
        // Handle directives
        if (inst.opcode == "START" || inst.opcode == "END") {
            cout << inst.address << "\t"
                 << (inst.label.empty() ? "-" : inst.label) << "\t"
                 << inst.opcode << "\t"
                 << (inst.operand1.empty() ? "-" : inst.operand1) << "\t"
                 << "-" << "\n";
            continue;
        }
        else if (inst.opcode == "DS") {
            cout << inst.address << "\t"
                 << (inst.label.empty() ? "-" : inst.label) << "\t"
                 << inst.opcode << "\t"
                 << inst.operand1 << "\t"
                 << "--- (Reserved)" << "\n";
            continue;
        }
        else if (inst.opcode == "DC") {
            cout << inst.address << "\t"
                 << (inst.label.empty() ? "-" : inst.label) << "\t"
                 << inst.opcode << "\t"
                 << inst.operand1 << "\t"
                 << inst.operand1 << "\n";
            continue;
        }
        
        // Generate machine code for regular instructions
        if (opcodeTable.find(inst.opcode) != opcodeTable.end()) {
            machineCode = opcodeTable[inst.opcode];
            
            // Handle operands
            if (!inst.operand1.empty()) {
                // Check if operand is a symbol
                if (symbolTable.find(inst.operand1) != symbolTable.end()) {
                    int addr = symbolTable[inst.operand1];
                    machineCode += " " + to_string(addr);
                }
                // Check if operand is a number
                else if (isNumber(inst.operand1)) {
                    machineCode += " " + inst.operand1;
                }
                // Register or other operand
                else {
                    machineCode += " " + inst.operand1;
                }
                
                // Handle second operand if present
                if (!inst.operand2.empty()) {
                    if (symbolTable.find(inst.operand2) != symbolTable.end()) {
                        int addr = symbolTable[inst.operand2];
                        machineCode += " " + to_string(addr);
                    }
                    else {
                        machineCode += " " + inst.operand2;
                    }
                }
            }
            
            cout << inst.address << "\t"
                 << (inst.label.empty() ? "-" : inst.label) << "\t"
                 << inst.opcode << "\t"
                 << (inst.operand1.empty() ? "-" : inst.operand1) << "\t"
                 << machineCode << "\n";
        }
    }
}

// Display Symbol Table
void displaySymbolTable() {
    cout << "\n========== SYMBOL TABLE ==========\n";
    cout << "Symbol\t\tAddress\n";
    cout << "--------------------------------\n";
    for (const auto& entry : symbolTable) {
        cout << entry.first << "\t\t" << entry.second << "\n";
    }
}

int main() {
    string filename = "lab4input.asm";
    
    cout << "Two-Pass Assembler\n";
    cout << "==================\n";
    
    // Pass 1: Build symbol table
    vector<Instruction> instructions = pass1(filename);
    
    // Display symbol table
    displaySymbolTable();
    
    // Pass 2: Generate machine code
    pass2(instructions);
    
    return 0;
}
