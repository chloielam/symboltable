#include "SymbolTable.h"

string * tokenize(string s) {
    int order = 0;
    string * cmd = new string[4];
    int start = 0;
    for(unsigned i = 0; i < s.size(); i++) {
        if(order <= 3) {
            if(order == 1 && cmd[0] == "CALL") {
                cmd[1] = s.substr(start, s.size() - start);
                i = s.size();
            } else if(order == 2 && cmd[0] == "ASSIGN") {
                cmd[2] = s.substr(start, s.size() - start);
                i = s.size();
            } else if(s[i] == ' ') {
                cmd[order] = s.substr(start, i-start);
                order += 1;
                start = i + 1;
            } else if(i == s.size() - 1) {
                cmd[order] = s.substr(start, i-start+1);
            }
        } 
    }
    return cmd;
}

string * tokenize1(string s, int size) {
    int order = 1;
    int start = 0;
    string * cmd = new string[size];
    for(unsigned i = 0; i < s.size(); i++) {
        if(order <= size - 1) {
            if(s[i] == ',') {
                cmd[order] = s.substr(start, i-start);
                order += 1;
                start = i + 1;
            } else if(i == s.size() - 1) {
                cmd[order] = s.substr(start, i-start+1);
            }
        } 
    }
    return cmd;
}

int paraN(string s) {
    if(s != "") {
        return count(s.begin(), s.end(), ',') + 2;
    } else {
        return 1;
    }
}

string id(string s) {
    int pos = s.find("(");
    return s.substr(0, pos);
}

string pa(string s) {
    int pos = s.find("(");
    return s.substr(pos+1, s.size()-pos-2);
}

void SymbolTable::run(string filename) { 
    ifstream file(filename);
    string tmp = ""; 
    regex lin = regex("^LINEAR \\d{1,6} \\d{1,6}$");
    regex quad = regex("^QUADRATIC \\d{1,6} \\d{1,6} \\d{1,6}$");
    regex doub = regex("^DOUBLE \\d{1,6} \\d{1,6}$");
    regex inser = regex("^INSERT [a-z]\\w*( \\d+)?$");
    regex assign = regex("^ASSIGN [a-z]([a-z]|[A-Z]|[0-9]|_)* ([0-9][0-9]*|'([a-z]|[A-Z]|[0-9]| )*'|[a-z]([a-z]|[A-Z]|[0-9]|_)*|[a-z]([a-z]|[A-Z]|[0-9]|_)*(\\(\\)|\\(([0-9][0-9]*|'([a-z]|[A-Z]|[0-9]| )*'|[a-z]([a-z]|[A-Z]|[0-9]|_)*)(,([0-9][0-9]*|'([a-z]|[A-Z]|[0-9]| )*'|[a-z]([a-z]|[A-Z]|[0-9]|_)*))*\\)))$");
    regex call = regex("^CALL [a-z]([a-z]|[A-Z]|[0-9]|_)*(\\(\\)|\\(([0-9][0-9]*|'([a-z]|[A-Z]|[0-9]| )*'|[a-z]([a-z]|[A-Z]|[0-9]|_)*)(,([0-9][0-9]*|'([a-z]|[A-Z]|[0-9]| )*'|[a-z]([a-z]|[A-Z]|[0-9]|_)*))*\\))$");
    regex begin = regex("(^BEGIN$)");;
    regex end = regex("(^END$)");
    regex lookup = regex("(^LOOKUP ([a-z][\\w]*)$)");
    regex prin = regex("(^PRINT$)");
    regex rName = regex("^[a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*$");
    regex rNum = regex("^[0-9]+$");
    regex rString = regex("^[']{1}([a-z]|[A-Z]|[0-9]|[ ])*[']{1}$");
    regex in = regex("(\\bnumber\\b|\\bstring\\b|\\bvoid\\b)");

    while(!file.eof()) {
        getline(file, tmp);
        string * cmd = tokenize(tmp);
        if(regex_search(tmp, in)) {
            delete [] cmd; 
            throw InvalidInstruction(tmp);
        } else if(regex_search(tmp, lin)) {
            updateSymbolTable(stoi(cmd[1]), "linear", stoi(cmd[2]), 0);
            delete [] cmd;
        } else if(regex_search(tmp, quad)) {
            updateSymbolTable(stoi(cmd[1]), "quadratic", stoi(cmd[2]), stoi(cmd[3]));
            delete [] cmd;
        } else if(regex_search(tmp, doub)) {
            updateSymbolTable(stoi(cmd[1]), "double", stoi(cmd[2]), 0);
            delete [] cmd;
        } else if(regex_search(tmp, inser)) {
            int probe = 0;
            if(find(cmd[1], scope, probe) != -1) {
                string e = cmd[1];
                delete [] cmd;
                throw Redeclared(e);
            }
            bool success;
            probe = 0;
            if(cmd[2] == "") {
                success = insert(cmd[1], "", scope, 0, probe);
            } else {
                if(scope != 0) {
                    string e = cmd[1];
                    delete [] cmd;
                    throw InvalidDeclaration(e);
                }
                success = insert(cmd[1], "function", 0, stoi(cmd[2]) + 1, probe);
            }
            if (!success) {
                delete [] cmd;
                throw Overflow(tmp);
            }
            std::cout << probe << '\n';
            delete [] cmd;
        } else if(regex_search(tmp, assign)) {
            int probe = 0; int total = 0;
            if(regex_search(cmd[2], rString)) {
                int index = look(cmd[1], probe); 
                total += probe; probe = 0;
                if(index == -1) {
                    string e = cmd[1];
                    delete [] cmd; 
                    throw Undeclared(e);
                }
                if(table[index]->type == "") {
                    table[index]->type = "string";
                } else if(table[index]->type != "string") {
                    delete [] cmd; 
                    throw TypeMismatch(tmp);
                }
            } else if(regex_search(cmd[2], rNum)) {
                int index = look(cmd[1], probe);
                total += probe; probe = 0; 
                if(index == -1) {
                    string e = cmd[1];
                    delete [] cmd; 
                    throw Undeclared(e);
                }
                if(table[index]->type == "") {
                    table[index]->type = "number";
                } else if(table[index]->type != "number") {
                    delete [] cmd; 
                    throw TypeMismatch(tmp);
                }
            } else if(regex_search(cmd[2], rName)) {  
                int index1 = look(cmd[2], probe);
                total += probe; probe = 0;
                if(index1 == -1) {
                    string e = cmd[2];
                    delete [] cmd; 
                    throw Undeclared(e);
                }
                string type1 = table[index1]->type;
                if(type1 == "function") {
                    delete [] cmd; 
                    throw InvalidInstruction(tmp);
                }
                int index2 = look(cmd[1], probe);
                total += probe; probe = 0;
                if(index2 == -1) {
                    string e = cmd[1];
                    delete [] cmd; 
                    throw Undeclared(e);
                }
                string type2 = table[index2]->type;
                 if(type2 == "function" || (type1 != "" && type2 != "" && type1 != type2)) {
                    delete [] cmd; 
                    throw TypeMismatch(tmp);
                } else if(type1 == "" && type2 == "") {
                    delete [] cmd; 
                    throw TypeCannotBeInferred(tmp);
                } else {
                    if(type1 == "") {
                        table[index1]->type = type2;
                    } else if(type2 == "") {
                        table[index2]->type = type1;
                    }
                }
            } else {
                string f = id(cmd[2]);  
                string p = pa(cmd[2]);
                int n = paraN(p);
                string * temp = tokenize1(p, n);
                int index1 = look(f, probe);
                total += probe; probe = 0;
                if(index1 == -1) {
                    delete [] cmd; 
                    throw Undeclared(f);
                }
                if(table[index1]->paraNo != n) {
                    delete [] cmd; 
                    throw TypeMismatch(tmp);
                }
                string * temp1 = table[index1]->para;
                for(int i = 1; i < n; i++) {
                    if(regex_search(temp[i], rString)) {
                        if(temp1[i] == "") {
                            temp1[i] = "string";
                        } else if(temp1[i] != "string") {
                            delete [] temp;
                            delete [] cmd; 
                            throw TypeMismatch(tmp);
                        }
                    } else if(regex_search(temp[i], rNum)) {
                        if(temp1[i] == "") {
                            temp1[i] = "number";
                        } else if(temp1[i] != "number") {
                            delete [] temp;
                            delete [] cmd; 
                            throw TypeMismatch(tmp);
                        }
                    } else {
                        int idx = look(temp[i], probe);
                        total += probe; probe = 0;
                        if(idx == -1) {
                            string e = temp[i];
                            delete [] temp;
                            delete [] cmd; 
                            throw Undeclared(e);
                        }
                        string t1 = table[idx]->type;
                        string t2 = temp1[i];
                        if(t1 == "function" || (t1 != "" && t2 != "" && t1 != t2)) {
                            delete [] temp;
                            delete [] cmd; 
                            throw TypeMismatch(tmp);
                        } else if(t1 == "" && t2 == "") {
                            delete [] temp;
                            delete [] cmd; 
                            throw TypeCannotBeInferred(tmp);
                        } else {
                            if(t1 == "") {
                                table[idx]->type = t2;
                            } else if(t2 == "") {
                                temp1[i] = t1;
                            }
                        }
                    }
                } 
                string t2 = table[index1]->para[0];
                if(t2 == "void") {
                    delete [] temp;
                    delete [] cmd; 
                    throw TypeMismatch(tmp);
                }
                int index2 = look(cmd[1], probe);
                total += probe; probe = 0;
                if(index2 == -1) {
                    string e = cmd[1];
                    delete [] temp;
                    delete [] cmd; 
                    throw Undeclared(e);
                }
                string t1 = table[index2]->type; 
                if(t1 == "function" || (t1 != "" && t2 != "" && t1 != t2)) {
                    delete [] temp;
                    delete [] cmd; 
                    throw TypeMismatch(tmp);
                } if(t1 == "" && t2 == "") {
                    delete [] temp;
                    delete [] cmd; 
                    throw TypeCannotBeInferred(tmp);
                } else {
                    if(t1 == "") {
                        table[index2]->type = t2;
                    } else if(t2 == "") {
                        table[index1]->para[0] = t1;
                    }
                }
                delete [] temp;
            }
            std::cout << total << '\n';
            delete [] cmd;
        } else if(regex_search(tmp, call)) {
            int total = 0; int probe = 0;
            string f = id(cmd[1]);
            string p = pa(cmd[1]);
            int n = paraN(p);
            string * temp = tokenize1(p, n);
            int index1 = look(f, probe);
            total += probe; probe = 0;
            if(index1 == -1) {
                delete [] cmd; 
                throw Undeclared(f);
            } 
            if(table[index1]->paraNo != n) {
                delete [] temp;
                delete [] cmd; 
                throw TypeMismatch(tmp);
            }
            string * temp1 = table[index1]->para;
            for(int i = 1; i < n; i++) {
                if(regex_search(temp[i], rString)) {
                    if(temp1[i] == "") {
                        temp1[i] = "string";
                    } else if(temp1[i] != "string") {
                        delete [] temp;
                        delete [] cmd; 
                        throw TypeMismatch(tmp);
                    }
                } else if(regex_search(temp[i], rNum)) {
                    if(temp1[i] == "") {
                        temp1[i] = "number";
                    } else if(temp1[i] != "number") {
                        delete [] temp;
                        delete [] cmd; 
                        throw TypeMismatch(tmp);
                    }
                } else {
                    int idx = look(temp[i], probe);
                    total += probe; probe = 0;
                    if(idx == -1) {
                        string e = temp[i];
                        delete [] temp;
                        delete [] cmd; 
                        throw Undeclared(e);
                    }
                    string t1 = table[idx]->type;
                    string t2 = temp1[i];
                    if(t1 == "" && t2 == "") {
                        delete [] temp;
                        delete [] cmd; 
                        throw TypeCannotBeInferred(tmp);
                    } else if(t1 == "function" || (t1 != "" && t2 != "" && t1 != t2)) {
                        delete [] temp;
                        delete [] cmd; 
                        throw TypeMismatch(tmp);
                    } else {
                        if(t1 == "") {
                            table[idx]->type = t2;
                        } else if(t2 == "") {
                            temp1[i] = t1;
                        }
                    }
                }
            }
            if((table[index1]->para[0] != "" && table[index1]->para[0] != "void")) {
                delete [] temp;
                delete [] cmd; 
                throw TypeMismatch(tmp);
            }
            table[index1]->para[0] = "void";
            delete [] temp;
            std::cout << total << '\n';
            delete [] cmd;
        } else if(regex_search(tmp, begin)) {
            scope += 1;
            delete [] cmd;
        } else if(regex_search(tmp, end)) {
            if(scope == 0) {
                delete [] cmd;
                throw UnknownBlock();
            }
            for(int i = 0; i < capac && size > 0; i++) {
                if(table[i]->level == scope) {
                    delete table[i];
                    size -= 1;
                    table[i] = new Symbol();
                }
            }
            scope -= 1;
            delete [] cmd;
        } else if(regex_search(tmp, lookup)) {
            int probe;
            int index = look(cmd[1], probe);
            if(index == -1) {
                string e = cmd[1];
                delete [] cmd;
                throw Undeclared(e);
            }
            std::cout << index << '\n';
            delete [] cmd;
        } else if(regex_search(tmp, prin)) {
            string p = print();
            if(p != "") std::cout << print() << '\n';
            delete [] cmd;
        } else {
            delete [] cmd;
            throw InvalidInstruction(tmp);
        }
    }
    if(scope > 0) {
        throw UnclosedBlock(scope);
    }
}