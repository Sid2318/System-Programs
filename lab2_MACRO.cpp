#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

struct Macro {
    int index;
    string name;
};

struct MacroDef {
    int index;
    string instruction;
};

int main() {
    vector<Macro> mnt; // Macro Name Table
    vector<MacroDef> mdt; // Macro Definition Table
    vector<string> expanded; // Expanded code

    ifstream file("lab2input.c");
    string line;
    int mdtIndex = 1;
    bool inMacro = false;

    while (getline(file, line)) {
        // Remove leading/trailing spaces
        size_t start = line.find_first_not_of(" \t");
        size_t end = line.find_last_not_of(" \t");
        line = (start == string::npos) ? "" : line.substr(start, end - start + 1);
        if (line.empty()) continue;

        if (line.find("Macro") == 0 || line.find("MACRO") == 0) {
            string macroName = line.substr(5);
            macroName = macroName.substr(macroName.find_first_not_of(" \t"));
            mnt.push_back({mdtIndex, macroName});
            inMacro = true;
        }
        else if (inMacro) {
            if (line.find("MEND") != string::npos) {
                mdt.push_back({mdtIndex, "MEND"});
                mdtIndex++;
                inMacro = false;
            } else {
                mdt.push_back({mdtIndex, line});
                mdtIndex++;
            }
        }
        else if (line.find(".code") != string::npos) {
            while (getline(file, line)) {
                size_t start = line.find_first_not_of(" \t");
                size_t end = line.find_last_not_of(" \t");
                line = (start == string::npos) ? "" : line.substr(start, end - start + 1);
                if (line.find("END") != string::npos) break;
                if (line.empty()) continue;

                bool macroCalled = false;
                for (auto &macro : mnt) {
                    if (line.find(macro.name) != string::npos) {
                        macroCalled = true;
                        int i = macro.index - 1;
                        while (i < mdt.size() && mdt[i].instruction != "MEND") {
                            expanded.push_back(mdt[i].instruction);
                            i++;
                        }
                        break;
                    }
                }
                if (!macroCalled) expanded.push_back(line);
            }
        }
    }
    file.close();

    cout << "\nMacro Name Table (MNT):\n";
    cout << "Index\tName\n";
    for (auto &m : mnt)
        cout << m.index << "\t" << m.name << "\n";

    cout << "\nMacro Definition Table (MDT):\n";
    cout << "Index\tInstruction\n";
    for (auto &d : mdt)
        cout << d.index << "\t" << d.instruction << "\n";

    cout << "\nExpanded Code:\n";
    for (auto &e : expanded)
        cout << e << "\n";

    return 0;
}





/*
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

struct Macro {
    int index;
    string name;
};

struct MacroDef {
    int index;
    string line;
};

int main() {
    vector<Macro> MNT;
    vector<MacroDef> MDT;
    vector<string> expanded;

    ifstream file("lab2input.c");

    string line;
    int index = 1;
    bool insideMacro = false;

    while (getline(file, line)) {

        // Remove extra spaces
        while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
            line.erase(0, 1);

        if (line.empty()) continue;

        // Step 1: Detect MACRO
        if (line.find("MACRO") == 0) {
            string name = line.substr(5);
            while (!name.empty() && name[0] == ' ')
                name.erase(0, 1);

            MNT.push_back({index, name});
            insideMacro = true;
        }

        // Step 2: Store macro body
        else if (insideMacro) {
            if (line == "MEND") {
                MDT.push_back({index, "MEND"});
                index++;
                insideMacro = false;
            } else {
                MDT.push_back({index, line});
                index++;
            }
        }

        // Step 3: Expand macros
        else if (line == ".code") {
            while (getline(file, line)) {
                while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
                    line.erase(0, 1);

                if (line == "END") break;
                if (line.empty()) continue;

                bool isMacro = false;

                for (auto &m : MNT) {
                    if (line == m.name) {
                        int i = m.index - 1;
                        while (MDT[i].line != "MEND") {
                            expanded.push_back(MDT[i].line);
                            i++;
                        }
                        isMacro = true;
                        break;
                    }
                }

                if (!isMacro)
                    expanded.push_back(line);
            }
        }
    }

    // Print MNT
    cout << "\nMNT:\n";
    cout << "Index  Name\n";
    for (auto &m : MNT)
        cout << m.index << "   " << m.name << endl;

    // Print MDT
    cout << "\nMDT:\n";
    cout << "Index  Line\n";
    for (auto &d : MDT)
        cout << d.index << "   " << d.line << endl;

    // Print Expanded Code
    cout << "\nExpanded Code:\n";
    for (auto &e : expanded)
        cout << e << endl;

    return 0;
}
*/