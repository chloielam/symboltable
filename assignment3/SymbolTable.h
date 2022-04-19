#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class SymbolTable {
public:
    class Symbol; 
private: 
    int capac; 
    Symbol ** table; 
    int size; 
    int scope;
    string func; 
    int c1; int c2;
public: 
    class Symbol {
        private: 
            string iden; 
            string type;
            int level; 
            int paraNo;
            string * para; 
            friend class SymbolTable; 
        public:
            Symbol() {
                iden = "";
                type = "";
                level = 0;
                paraNo = 0;
                para = nullptr;
            }
            void updateSymbol(string iden, string type, int level, int paraNo) {
                this->iden = iden; 
                this->type = type; 
                this->level = level;
                this->paraNo = paraNo;
                if(this->paraNo > 0) {
                    this->para = new string[paraNo];
                }
            }
            string printS() {
                return iden + "//" + to_string(level); 
            }
            ~Symbol() {
                if(this->paraNo > 0) {
                    delete [] para;
                }
            }
    };
    
    SymbolTable() {
        table = nullptr; 
        size = 0;
        capac = 0;
        scope = 0; 
        func = "";
        c1 = 0; c2 = 0;
    }

    void updateSymbolTable(int capac, string func, int c1, int c2) {
        this->capac = capac;
        table = new Symbol * [this->capac];
        for(int i = 0; i < this->capac; i++) {
            table[i] = new Symbol();
        } 
        this->size = 0; 
        this->scope = 0; 
        this->func = func;
        this->c1 = c1; 
        this->c2 = c2;
    }

    ~SymbolTable(){
        for(int i = 0; i < capac; i++) {
            delete table[i];
        }
        delete [] table;
    }
    
    int mod(string num, int a) {
        int res = 0;
        for (unsigned i = 0; i < num.length(); i++)
            res = (res * 10 + (int)num[i] - '0') % a;
        return res;
    }

    int linearh(int& probe, string k) {
        if(probe > capac) {
            return -1;
        }
        int index = (mod(k, capac) + static_cast<long long>(c1)*static_cast<long long>(probe))%capac;
        if(table[index]->iden == "") {
            return index; 
        } else {
            probe += 1; 
            return linearh(probe, k);
        }
    }
    
    int quadratich(int& probe, string k) {
        if(probe > capac) {
            return -1;
        }
        int index = (mod(k, capac) + (static_cast<long long>(c1)*static_cast<long long>(probe)) + static_cast<long long>(c2)*static_cast<long long>(probe)*static_cast<long long>(probe))%capac;
        if(table[index]->iden == "") {
            return index; 
        } else {
            probe += 1; 
            return quadratich(probe, k);
        }
    }

    int doubleh(int & probe, string k) {
        if(probe > capac) {
            return -1;
        }
        int h1 = mod(k, capac);
        int h2 = 1 + mod(k, capac - 2);
        int index = (h1 + static_cast<long long>(c1)*static_cast<long long>(probe)*static_cast<long long>(h2)) % capac;
        if(table[index]->iden == "") {
            return index; 
        } else {
            probe += 1; 
            return doubleh(probe, k);
        }
    }
    
    string key(string iden, int level) {
        string sk = to_string(level);
        for(unsigned i = 0; i < iden.size(); i++) {
            int a = (int)iden[i] - 48;
            sk = sk + to_string(a);
        }
        return sk;
    }

    int find(string iden, int level, int& probe) {
        string k = key(iden, level);
        int i; 
        int h1 = mod(k, capac);
        do {
            if(func == "linear") {
                i = (h1 + static_cast<long long>(c1)*static_cast<long long>(probe))%capac;
            } else if(func == "quadratic") {
                i = (h1 + static_cast<long long>(c1)*static_cast<long long>(probe) + static_cast<long long>(c2)*static_cast<long long>(probe)*static_cast<long long>(probe))%capac;
            } else {
                int h2 = 1 + mod(k, capac - 2);
                i = (h1 + static_cast<long long>(c1)*static_cast<long long>(probe)*static_cast<long long>(h2)) % capac;
            }
            probe += 1;
        } while(probe <= capac && table[i]->iden != "" && (table[i]->iden != iden || ((table[i]->iden == iden) && (table[i]->level) != level)));
        probe -= 1;
        if(table[i]->iden == iden && table[i]->level == level) return i; 
        else return -1;
    }
    
    int look(string iden, int& probe) {
        int index = -1;
        for(int i = scope; i >= 0 && index == -1; i--) {
            probe = 0;
            index = find(iden, i, probe);
        }
        return index;
    }
    
    bool insert(string iden, string type, int level, int paraNo, int& probe) {
        if(size == capac) return false;
        string k = key(iden, level);
        int i;
        if(func == "linear") {
            i = linearh(probe, k);
        } else if(func == "quadratic") {
            i = quadratich(probe, k);
        } else {
            i = doubleh(probe, k);
        }
        if(i == -1) return false;
        table[i]->updateSymbol(iden, type, level, paraNo);
        size += 1;
        return true;
    }

    string print() {
        string p = "";
        for(int i = 0; i < capac; i++) {
            if(table[i]->iden != "") {
                p = p + to_string(i) + " " + table[i]->printS() + ";";
            }   
        }
        if(p == "") return p;
        return p.substr(0, p.size() - 1);
    }

    void run(string filename);
};
#endif