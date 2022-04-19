#include "SymbolTable.h"

string * tokenize(string s) {
    int order = 0;
    string * cmd = new string[4];
    int start = 0;
    for(unsigned i = 0; i < s.size(); i++) {
        if(order <= 3) {
            if(order == 2 && cmd[0] == "ASSIGN") {
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
void tokenize1(string s, string * cmd, int size) {
    int order = 1;
    int start = 0;
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
}



void SymbolTable::run(string filename) { 
    ifstream file(filename);
    string tmp = ""; 
    regex rName = regex("^[a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*$");
    regex rNum = regex("^[0-9]+$");
    regex stat = regex("(^false$)|(^true$)");
    regex rString = regex("^[']{1}([a-z]|[A-Z]|[0-9]|[ ])*[']{1}$");
    regex type = regex("(^number$)|(^string$)|(^[(]{1}(number|string)?[)]{1}->(number|string){1}$)|(^[(]((number|string){1}[,]{1})+(number|string){1}[)]->(number|string){1}$)");
    regex func = regex("(^[(]{1}(number|string)?[)]{1}->(number|string){1}$)|(^[(]((number|string){1}[,]{1})+(number|string){1}[)]->(number|string){1}$)");
    regex funcall = regex("^[a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*[(]{1}(([']{1}([a-z]|[A-Z]|[0-9]|[ ])*[']{1})|([0-9]+)|([a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*))?[)]{1}$");
    regex funcall2 = regex("^[a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*[(]{1}((([']{1}([a-z]|[A-Z]|[0-9]|[ ])*[']{1})|([0-9]+)|([a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*){1})[,]{1})+(([']{1}([a-z]|[A-Z]|[0-9]|[ ])*[']{1})|([0-9]+)|([a-z]{1}([a-z]|[A-Z]|[0-9]|[_])*)){1}[)]{1}$");
    while(!file.eof()) {
        getline(file, tmp);
        string * cmd = tokenize(tmp);
        if(cmd[0] == "INSERT" && regex_search(cmd[1], rName) && regex_search(cmd[2], type) && regex_search(cmd[3], stat)) {
            int comp = 0; 
            int slas = 0;
            if(regex_search(cmd[2], func)) {
                if(root == nullptr) {
                    if(cmd[3] == "true" || scope == 0) {
                        if(cmd[2][1] != ')') {
                            int size = count(cmd[2].begin(), cmd[2].end(), ',') + 2;
                            Node * adder = new Node(cmd[1], 0, size, "function");
                            adder->para[0] = cmd[2].substr(cmd[2].size() - 6, 6); 
                            tokenize1(cmd[2].substr(1, cmd[2].size() - 10),adder->para, size);
                            root = adder;
                        } else {
                            Node * adder = new Node(cmd[1], 0, 1, "function");
                            adder->para[0] = cmd[2].substr(cmd[2].size() - 6, 6);
                            root = adder;
                        }
                    } else {
                        delete [] cmd;
                        throw InvalidDeclaration(tmp);
                    }
                } else if(cmd[3] == "true" || scope == 0) {
                    Node * temp = search(root, cmd[1], 0, comp);
                    if(temp != nullptr) {
                        delete [] cmd;
                        throw Redeclared(tmp);
                    } else {
                        comp = 0;
                        if(cmd[2][1] != ')') {
                            int size = count(cmd[2].begin(), cmd[2].end(), ',') + 2;
                            Node * adder = new Node(cmd[1], 0, size, "function");
                            adder->para[0] = cmd[2].substr(cmd[2].size() - 6, 6); 
                            tokenize1(cmd[2].substr(1, cmd[2].size() - 10),adder->para, size);
                            addBinary(root, adder, comp);
                            slay(adder, slas);
                        } else {
                            Node * adder = new Node(cmd[1], 0, 1, "function");
                            adder->para[0] = cmd[2].substr(cmd[2].size() - 6, 6);
                            addBinary(root, adder, comp);
                            slay(adder, slas);
                        }
                    }
                } else {
                    delete [] cmd;
                    throw InvalidDeclaration(tmp);
                }
            } else {
                if(root == nullptr) {
                    if(cmd[3] == "true" || scope == 0) {
                        Node * adder = new Node(cmd[1], 0, 0, cmd[2]);
                        root = adder;
                    } else {
                        Node * adder = new Node(cmd[1], scope, 0, cmd[2]);
                        root = adder;
                        add(cmd[1], scope);
                    }
                } else if(cmd[3] == "true" || scope == 0) {
                    Node * temp = search(root, cmd[1], 0, comp);
                    if(temp != nullptr) {
                        delete [] cmd;
                        throw Redeclared(tmp);
                    } else {
                        comp = 0;
                        Node * adder = new Node(cmd[1], 0, 0, cmd[2]);
                        addBinary(root, adder, comp);
                        slay(adder, slas); 
                    }
                } else {
                    Node * temp = search(root, cmd[1], scope, comp);
                    if(temp != nullptr) {
                        delete [] cmd;
                        throw Redeclared(tmp);
                    } else {
                        comp = 0;
                        Node * adder = new Node(cmd[1], scope, 0, cmd[2]);
                        addBinary(root, adder, comp);
                        slay(adder, slas); 
                        add(cmd[1], scope);
                    }
                }
            }
            cout << comp << ' ' << slas << '\n';
            delete [] cmd;
        } else if(cmd[0] == "ASSIGN" && regex_search(cmd[1], rName) && cmd[3] == "") {
            int comp = 0; 
            int slas = 0;
            if(regex_search(cmd[2], rString)) {
                int state = scope;
                Node * temp = search(root, cmd[1], state, comp);
                while(temp == nullptr && state >=0) {
                    comp = 0;
                    state -= 1;
                    temp = search(root, cmd[1], state, comp);
                }
                if(temp == nullptr) {
                    delete [] cmd;
                    throw Undeclared(tmp);
                } else {
                    slay(temp, slas);
                    if(root->type != "string") {
                        delete [] cmd;
                        throw TypeMismatch(tmp);
                    }
                }
            } else if(regex_search(cmd[2], rNum)) {
                int state = scope;
                Node * temp = search(root, cmd[1], state, comp);
                while(temp == nullptr && state >=0) {
                    comp = 0;
                    state -= 1;
                    temp = search(root, cmd[1], state, comp);
                }
                if(temp == nullptr) {
                    delete [] cmd;
                    throw Undeclared(tmp);
                } else {
                    slay(temp, slas);
                    if(root->type != "number") {
                        delete [] cmd;
                        throw TypeMismatch(tmp);
                    }
                }
            } else if(regex_search(cmd[2], rName)) {
                int state = scope;
                int check = comp;
                Node * temp1 = search(root, cmd[2], state, comp);
                while(temp1 == nullptr && state >= 0) {
                    comp = check;
                    state -= 1;
                    temp1 = search(root, cmd[2], state, comp);
                }
                if(temp1 == nullptr) {
                    delete [] cmd;
                    throw Undeclared(tmp); 
                } else if(temp1->type == "function"){
                    delete [] cmd;
                    throw TypeMismatch(tmp);
                } else {
                    slay(temp1, slas);
                    state = scope;
                    check = comp;
                    Node * temp2 = search(root, cmd[1], state, comp);
                    while(temp2 == nullptr && state >=0) {
                        comp = check;
                        state -= 1;
                        temp2 = search(root, cmd[1], state, comp);
                    }
                    if(temp2 == nullptr) {
                        delete [] cmd;
                        throw Undeclared(tmp); 
                    } else {
                        if(temp2->type != temp1->type) {
                            delete [] cmd;
                            throw TypeMismatch(tmp);
                        } else {
                            slay(temp2, slas);
                        }
                    }
                }
            } else if(regex_search(cmd[2], funcall) || regex_search(cmd[2], funcall2)) {
                int pos = cmd[2].find("(");
                string * arr;
                int size;
                string iden = cmd[2].substr(0, pos);
                if(cmd[2][pos+1] == ')') {
                    size = 1;
                    arr = new string[size];
                } else {
                    size = count(cmd[2].begin(), cmd[2].end(), ',') + 2;
                    arr = new string[size];
                    tokenize1(cmd[2].substr(pos+1, cmd[2].size()-pos-2),arr, size);
                }
                Node * temp = search(root, iden, 0, comp); 
                if(temp == nullptr) {
                    delete [] cmd;
                    delete [] arr;
                    throw Undeclared(tmp);
                } else if(temp->type != "function") {
                    delete [] cmd;
                    delete [] arr;
                    throw TypeMismatch(tmp);
                } else {
                    slay(temp, slas);
                    arr[0] = temp->para[0];
                    if(size != temp->count) {
                        delete [] cmd;
                        delete [] arr;
                        throw TypeMismatch(tmp);
                    }
                    for(int i = 1; i < size; i++) {
                        if(regex_search(arr[i], rNum)) {
                            if(temp->para[i] != "number") {
                                delete [] cmd;
                                delete [] arr;
                                throw TypeMismatch(tmp);
                            }
                        } else if(regex_search(arr[i], rString)) {
                            if(temp->para[i] != "string") {
                                delete [] cmd;
                                delete [] arr;
                                throw TypeMismatch(tmp);
                            }
                        } else {
                            int state = scope;
                            int check = comp;
                            Node * temp1 = search(root, arr[i], state, comp);
                            while(temp1 == nullptr && state >= 0) {
                                comp = check;
                                state -= 1;
                                temp1 = search(root, arr[i], state, comp);
                            }
                            if(temp1 == nullptr) {
                                delete [] cmd;
                                delete [] arr;
                                throw Undeclared(tmp); 
                            } else {
                                slay(temp1, slas);
                                if(temp->para[i] != temp1->type) {
                                    delete [] cmd;
                                    delete [] arr;
                                    throw TypeMismatch(tmp);
                                }
                            }
                        }
                    }
                    int state = scope;
                    int check = comp;
                    Node * temp2 = search(root, cmd[1], state, comp);
                    while(temp2 == nullptr && state >=0) {
                        comp = check;
                        state -= 1;
                        temp2 = search(root, cmd[1], state, comp);
                    }
                    if(temp2 == nullptr) {
                        delete [] cmd;
                        delete [] arr;
                        throw Undeclared(tmp); 
                    } else {
                        if(temp2->type != arr[0]) {
                            delete [] cmd;
                            delete [] arr;
                            throw TypeMismatch(tmp);
                        } else {
                            slay(temp2, slas);
                        }
                    }
                }
                delete [] arr;
            } else {
                delete [] cmd;
                throw InvalidInstruction(tmp);
            }
            cout << comp << ' ' << slas << '\n';
            delete [] cmd;
        } else if(cmd[0] == "BEGIN" && cmd[1] == "" && cmd[2] == "" && cmd[3] == "") {
            scope += 1;
            delete [] cmd;
        } else if(cmd[0] == "END" && cmd[1] == "" && cmd[2] == "" && cmd[3] == "") {
            if(scope == 0) {
                delete [] cmd;
                throw UnknownBlock();
            } else {
                if(head != nullptr) {
                    if(head->level == scope) {
                        Node2 * temp = head;
                        while(temp != nullptr) {
                            delSlay(temp->name, temp->level);
                            temp = temp->next;
                        }
                        while(head != nullptr) {
                            Node2 * temp1 = head;
                            head = head->next;
                            delete temp1;
                        }
                        head = nullptr;
                        tail = nullptr;
                    } else {
                        Node2 * temp = head;
                        while(temp->next != nullptr && temp->next->level != scope) {
                            temp = temp->next;
                        }
                        if(temp->next != nullptr) {
                            Node2 * temp1 = temp->next;
                            while(temp1 != nullptr) {
                                delSlay(temp1->name, temp1->level);
                                temp1 = temp1->next;
                            }
                            tail = temp;
                            temp = temp->next;
                            tail->next = nullptr;
                            while(temp != nullptr) {
                                Node2 * temp2 = temp;
                                temp = temp->next;
                                delete temp2;
                            }
                        }
                    }
                } 
            }
            scope -= 1;
            delete [] cmd;
        } else if(cmd[0] == "LOOKUP" && regex_search(cmd[1], rName) && cmd[2] == "" && cmd[3] == "") {
            int comp = 0; 
            int state = scope;
            Node * temp = search(root, cmd[1], state, comp);
            while(temp == nullptr && state >=0) {
                state -= 1;
                temp = search(root, cmd[1], state, comp);
            }
            if(temp == nullptr) {
                delete [] cmd; 
                throw Undeclared(tmp);
            } else {
                slay(temp, comp); 
                cout << temp->level << '\n';
            }
            delete [] cmd;
        } else if(cmd[0] == "PRINT" && cmd[1] == "" && cmd[2] == "" && cmd[3] == "") {
            if(root != nullptr) {
                string result = "";
                prePrint(root, result);
                result = result.substr(0, result.size() - 1);
                cout << result << '\n';
            }
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