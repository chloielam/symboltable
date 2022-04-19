#include "SymbolTable.h"

string * tokenize(string s) {
    int order = 0;
    string * cmd = new string[3];
    int start = 0;
    for(unsigned i = 0; i < s.size(); i++) {
        if(order < 2) {
            if(s[i] == ' ') {
                cmd[order] = s.substr(start, i-start);
                order += 1;
                start = i + 1;
            } else if(i == s.size() - 1) {
                cmd[order] = s.substr(start, i-start+1);
            }
        } else {
            cmd[2] = s.substr(i,s.size() - i);
            i = s.size();
        }
    }
    return cmd;
}

void SymbolTable::run(string filename) { 
    ifstream file(filename);
    string tmp = ""; 
    regex rName = regex("^[a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*$");
    regex rNum = regex("^[0-9]+$");
    regex rString = regex("^[']{1}([a-z]|[A-Z]|[0-9]|[ ])*[']{1}$");
    while(!file.eof()) {
        getline(file, tmp);
        string * cmd = tokenize(tmp);
        if(cmd[0] == "INSERT" && regex_search(cmd[1], rName) && (cmd[2] == "number" || cmd[2] == "string")) {
            if(!lookUp(cmd[1])) {
                insert(cmd[1], cmd[2]);
                cout << "success\n";
            } else {
                delete [] cmd;
                throw Redeclared(tmp);
            }
            delete [] cmd;
        } else if(cmd[0] == "ASSIGN" && regex_search(cmd[1], rName)) {
            Symbol * s = find(cmd[1]);
            if(regex_search(cmd[2], rNum)) {
                if(s == NULL) {
                    delete [] cmd;
                    throw Undeclared(tmp);
                }
                if(s->getType() == "number") {
                    assign(s, cmd[2]);
                    cout << "success\n";
                } else {
                    delete [] cmd;
                    throw TypeMismatch(tmp);
                }
            } else if(regex_search(cmd[2], rString)) {
                if(s == NULL) {
                    delete [] cmd; 
                    throw Undeclared(tmp);
                }
                if(s->getType() == "string") {
                    assign(s, cmd[2]);
                    cout << "success\n";
                } else {
                    delete [] cmd; 
                    throw TypeMismatch(tmp);
                }
            } else if(regex_search(cmd[2], rName)) {
                if(s == NULL) {
                    delete [] cmd; 
                    throw Undeclared(tmp);
                }
                Symbol * s2 = find(cmd[2]);
                if(s2 == NULL) {
                    delete [] cmd;
                    throw Undeclared(tmp);
                } else if(s2->getType() == s->getType()) {
                    assign(s, s2->getValue());
                    cout << "success\n";
                } else if(s2->getType() != s->getType()) {
                    delete [] cmd;
                    throw TypeMismatch(tmp);
                } else {
                    delete [] cmd;
                    throw Undeclared(tmp);
                }
            } else {
                delete [] cmd;
                throw InvalidInstruction(tmp);
            }
            delete [] cmd;
        } else if(cmd[0] == "BEGIN" && cmd[1] == "" && cmd[2] == "") {
            ifBegin();
            delete [] cmd;
        } else if(cmd[0] == "END" && cmd[1] == "" && cmd[2] == "") {
            ifEnd();
            if(currScope < 0) {
                delete [] cmd;
                throw UnknownBlock();
            }
            delete [] cmd;
        } else if(cmd[0] == "LOOKUP" && regex_search(cmd[1], rName) && cmd[2] == "") {
            Symbol * s = find(cmd[1]);
            if(s != NULL) {
                cout << s->getLevel() << '\n';
            } else {
                delete [] cmd;
                throw Undeclared(tmp);
            }
            delete [] cmd;
        } else if(cmd[0] == "PRINT" && cmd[1] == "" && cmd[2] == "") {
            if(size != 0) print();
            delete [] cmd;
        } else if(cmd[0] == "RPRINT" && cmd[1] == "" && cmd[2] == "") {
            if(size != 0) rPrint();
            delete [] cmd;
        } else {
            delete [] cmd;
            throw InvalidInstruction(tmp);
        }
    }
    if(currScope > 0) {
        throw UnclosedBlock(currScope);
    }
}