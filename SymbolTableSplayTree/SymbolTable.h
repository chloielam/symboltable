#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class SymbolTable {

public:
    class Node;
    class Node2;
private: 
    Node * root;
    int scope;
    Node2 * head; 
    Node2 * tail;
public:
    class Node2 {
        private: 
            string name; 
            int level;
            Node2 * next; 
            friend class SymbolTable;
        public:
        Node2(string _name, int _level) {
            name = _name;
            level = _level;
            next = nullptr;
        }
        ~Node2() {
        }
    };
    class Node {
        private: 
            string name; 
            int level; 
            int count; 
            string * para;
            string type;
            Node * left; 
            Node * right; 
            Node * parent;
            friend class SymbolTable;
        public:
            Node(string _name, int _level, int _count, string _type) {
                name = _name; 
                level = _level; 
                count = _count; 
                type = _type;
                left = nullptr; 
                right = nullptr; 
                parent = nullptr;
                if(count != 0) {
                    para = new string[count];
                }
            }
            ~Node(){
                if(count != 0) delete [] para;
            }

            string printNode() {
                return name + "//" + to_string(level); 
            }
    };
    SymbolTable() {
        root = nullptr;
        scope = 0; 
        head = nullptr;
        tail = nullptr;
    }

    void add(string _name, int _level) {
        if(head == nullptr) {
            head = new Node2(_name, _level);
            tail = head;
        } else {
            tail->next = new Node2(_name, _level);
            tail = tail->next;
        }
    }

    void slay(Node * slayer, int& count) {
        if(slayer->parent != nullptr) count += 1;
        while(slayer->parent != nullptr) {
            Node * p = slayer->parent;
            Node * g = p->parent;
            if(g == nullptr) {
                if(slayer == p->left) {
                    turnRight(p);
                } else {
                    turnLeft(p);
                }
            } else {  
                if(p == g->left && slayer == p->left) {
                    turnRight(g);
                    turnRight(p);
                } else if(p == g->right && slayer == p->right) {
                    turnLeft(g); 
                    turnLeft(p);
                } else if(p == g->left && slayer == p->right) {
                    turnLeft(p);
                    turnRight(g);
                } else {
                    turnRight(p);
                    turnLeft(g);
                }
            }
        }
        root = slayer;
    }

    void turnLeft(Node * splayer) {
        Node * temp2 = splayer->right;
        Node * temp3 = splayer->parent;
        if(temp3 != nullptr) {
            if(temp3->right == splayer) {
                temp3->right = temp2;
            } else {
                temp3->left = temp2;
            }
        }
        splayer->right = temp2->left;
        if(temp2->left != nullptr) {
            temp2->left->parent = splayer;
        }
        temp2->left = splayer;
        splayer->parent = temp2;
        temp2->parent =temp3;
    }

    void turnRight(Node * splayer) { 
        Node * temp2 = splayer->left;
        Node * temp3 = splayer->parent;
        if(temp3 != nullptr) {
            if(temp3->right == splayer) {
                temp3->right = temp2;
            } else {
                temp3->left = temp2;
            }
        }
        splayer->left = temp2->right;
        if(temp2->right != nullptr) {
            temp2->right->parent = splayer;
        }
        temp2->right = splayer;
        splayer->parent = temp2;
        temp2->parent =temp3;
    }

    void addBinary(Node *& _root, Node * adder, int& count) {
        if(_root == nullptr) {
            _root = adder; 
            return;
        }
        if(adder->level > _root->level) {
            count += 1;
            if(_root->right == nullptr) {
                _root->right = adder;
                adder->parent = _root;
            } else {
                addBinary(_root->right, adder, count);
            }
        } else if(adder->level < _root->level) {
            count += 1;
            if(_root->left == nullptr) {
                _root->left = adder;
                adder->parent = _root;
            } else {
                addBinary(_root->left, adder, count);
            }
        } else if(adder->name.compare(_root->name) > 0) {
            count += 1;
            if(_root->right == nullptr) {
                _root->right = adder;
                adder->parent = _root;
            } else {
                addBinary(_root->right, adder, count);
            }
        } else if(adder->name.compare(_root->name) < 0) {
            count += 1;
            if(_root->left == nullptr) {
                _root->left = adder;
                adder->parent = _root;
            } else {
                addBinary(_root->left, adder, count);
            }
        }
    } 
    Node * search(Node * _root, string _name, int _level, int& count) {
        if((_root == nullptr)||(_root->name == _name && _root->level == _level)) {
            count += 1;
            return _root;
        } else if(_root->level < _level) {
            count += 1;
            return search(_root->right, _name, _level, count);
        } else if(_root->level > _level) {
            count += 1;
            return search(_root->left, _name, _level, count);
        } else if(_root->name.compare(_name) > 0) {
            count += 1;
            return search(_root->left, _name, _level, count);
        } else {
            count += 1;
            return search(_root->right, _name, _level, count);
        }
    }

    Node * max(Node * _root) {
        if(_root->right == nullptr) {
            return _root;
        } else {
            return max(_root->right);
        }
    }

    void delSlay(string _name, int _level) {
        if(root != nullptr) {
            int count = 0;
            Node * temp = search(root, _name, _level, count);
            if(temp!= nullptr) {
                slay(temp, count);
                if(root->left == nullptr && root->right == nullptr) {
                    delete root; 
                    root = nullptr;
                } else if(root->left != nullptr && root->right != nullptr) {
                    Node * left = root->left;
                    Node * right = root->right;
                    delete root;
                    root = left;
                    root->parent = nullptr;
                    Node * maxLeft = max(root);
                    slay(maxLeft, count);
                    root->right = right;
                    right->parent = root;
                } else if(root->left == nullptr) {
                    Node * right = root->right; 
                    delete root; 
                    root = right; 
                    root->parent = nullptr;
                } else {
                    Node * left = root->left; 
                    delete root; 
                    root = left;
                    root->parent = nullptr;
                }
            }
        }
    }

    void prePrint(Node * _root, string& print) {
        if(_root != nullptr) {
        print = print + _root->printNode() + " ";
        prePrint(_root->left, print); 
        prePrint(_root->right, print);
        }
    }

    void delBinary(Node *& _root) {
        if(_root != nullptr) {
            delBinary(_root->right);
            delBinary(_root->left);
            delete _root;
            _root = nullptr;
        }
    }

    ~SymbolTable() {
        delBinary(root);
        while(head != nullptr) {
            Node2 * temp = head;
            head = head->next;
            delete temp;
        }
    }

    void run(string filename);
};
#endif