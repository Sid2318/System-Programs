#include<bits/stdc++.h>
using namespace std;

class MNT{
public:
    unordered_map<string,int> mp;

    void insert(string name,int index){
        mp[name]=index;
    }

    void print(){
        cout<<"MNT table: "<<endl<<endl;
        string col1 = "Name", col2 = "index";
        printf("%5s %8s\n", col1.c_str(), col2.c_str());

        for(auto it:mp)
        printf("%5s %8d\n", it.first.c_str(), it.second);
    }
};

class MDT{
public: 
    string card;
};

class ALA{
    public:
    unordered_map<string,string> formal_actual;

    void print(){
        cout<<"ALA : "<<endl<<endl;
        string col1 = "Formal", col2 = "Actual";
        printf("%5s %8s\n", col1.c_str(), col2.c_str());

        for(auto it:formal_actual)
        printf("%5s %8s\n", it.first.c_str(), it.second.c_str());
    }
};

int main(){
    MNT mnt_table;
    vector<MDT> mdt_table;
    ALA ala_table;

    ifstream file("lab3input.c");
    string s;

    if (file.is_open())
    {
        while (getline(file, s) && s!=".code")
        {
            if(s.find("Macro ")!=string::npos){
                int i=0;
                while(s[i]==' ') i++;
                i+=5;//for going forward of word 'Macro'
                while(s[i]==' ') i++;

                string name="";
                while(s[i]!=' ' && s[i]!=',') name+=s[i++];
                mnt_table.insert(name,mdt_table.size());

                while(i<s.length()){
                    i++;
                    string formal="";
                    while(s[i]!=' ' && s[i]!=',' && i<s.length()) formal+=s[i++];
                    ala_table.formal_actual[formal]="";
                }

                mdt_table.push_back({s});
                while(getline(file,s)){
                    mdt_table.push_back({s});
                    if(s=="Mend") break;
                }
            }
        }

        mnt_table.print();

        cout<<endl<<endl;
        cout<<"MDT table:"<<endl<<endl;
        cout<<"index   card"<<endl;
        for(int i=0;i<mdt_table.size();i++){
            cout<<i<<"       "<<mdt_table[i].card<<endl;
        }

        cout<<endl<<endl;
        ala_table.print();

        cout<<endl<<endl<<"Expanded code:"<<endl;

        while (getline(file, s) && s!="end"){
            istringstream line(s);
            string word;
            bool end=true;
            while(line>>word){
                if(mnt_table.mp.find(word)==mnt_table.mp.end()){ 
                    cout<<word<<" ";
                    end=true;
                }
                else{
                    int i=mnt_table.mp[word];
                    istringstream start(mdt_table[i].card);
                    string formal,actual;
                    start>>formal;//removing "Macro:" string
                    start>>formal;//removing macro name
                    // cout<<formal<<endl;
                    while(line>>actual && start>>formal){ 
                        if(actual.back()==',') actual.pop_back();
                        if(formal.back()==',') formal.pop_back();
// cout<<actual<<" "<<formal<<endl;
                        ala_table.formal_actual[formal]=actual;
                    }
                    i++;

                    while(mdt_table[i].card!="Mend"){

                        istringstream start(mdt_table[i].card);{
                        
                        string word;
                        while(start>>word){
                            if(word.back()==','){
                                word.pop_back();
                                if(ala_table.formal_actual.find(word)==ala_table.formal_actual.end()) cout<<word<<", ";
                                else cout<<ala_table.formal_actual[word]<<", ";
                            }
                            else{
                                if(ala_table.formal_actual.find(word)==ala_table.formal_actual.end()) cout<<word<<" ";
                                else cout<<ala_table.formal_actual[word]<<" ";
                            }
                        }
                        cout<<endl;
                    }
                        i++;
                    }
                    end=false;
                }
            }
            if(end)cout<<endl;

            // ala_table.print();
        }

    }
    else
        cout << "file not open";

return 0;
}