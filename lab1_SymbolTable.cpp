#include <bits/stdc++.h>
using namespace std;

class Symbol
{
public:
    string name;
    int size;
    int address;

    // Symbol(string s,int siz,int add){
    //     this->name=s;
    //     this->size=siz;
    //     this->address=add;
    // }
};

int main()
{
    vector<Symbol> symbol_table;

    ifstream file("lab1input.c");
    string s;
    int curr_address = 100;

    if (file.is_open())
    {
        while (getline(file, s))
        {
            if (s.find("int") != string::npos || s.find("float") != string::npos || s.find("char") != string::npos)
            {
                int i = 0;
                while (s[i] == ' ')
                    i++;

                if ("int" == s.substr(i, 3))
                {
                    i += 3;
                    while (s[i] == ' ')
                        i++;

                    string curr = "";
                    while (i<s.length() && s[i] != ' ' && s[i] != '=' && s[i] != ';'&& s[i] != ',')
                        curr += s[i++];
                    
                    if (curr != "main" && curr != "main()" && curr != "main(){")
                    {
                        symbol_table.push_back({curr, 4, curr_address});
                        curr_address += 4;
                    }
                }
                else if ("char" == s.substr(i, 4))
                {
                    i += 4;
                    while (s[i] == ' ')
                        i++;

                    string curr = "";
                    while (i<s.length() && s[i] != ' ' && s[i] != '=' && s[i] != ';'&& s[i] != ',')   
                        curr += s[i++];
                    
                    symbol_table.push_back({curr, 1, curr_address});
                    curr_address += 1;
                }
                else if ("float" == s.substr(i, 5))
                {
                    i += 5;
                    while (s[i] == ' ')
                        i++;

                    string curr = "";
                    while (i<s.length() && s[i] != ' ' && s[i] != '=' && s[i] != ';'&& s[i] != ',')
                        curr += s[i++];

                    symbol_table.push_back({curr, 4, curr_address});
                    curr_address += 4;
                }
            }
        }

        string col1 = "Symbol", col2 = "size", col3 = "address";
        printf("%20s %8s %12s\n", col1.c_str(), col2.c_str(), col3.c_str());

        for (int i = 0; i < symbol_table.size(); i++)
        {
            printf("%20s %8d %12d\n", symbol_table[i].name.c_str(), symbol_table[i].size, symbol_table[i].address);
        }
    }
    else
        cout << "file not open";
}
