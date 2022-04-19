#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Symbol {

    string name; 
    string type; 
    string value; 
    int level;

public: 
    Symbol() {
        type = "";
        name = "";
        value = "";
        level = 0;
    }
    Symbol(string _name, string _type, string _value, int _level) : name(_name), type(_type), value(_value), level(_level) {
    }
    void sName(string _name) {
        name = _name;
    }
    void sType(string _type) {
        type = _type;
    }
    string getType() {
        return type; 
    }
    string getName() {
        return name;
    }
    string getValue() {
        return value; 
    }
    int getLevel() {
        return level; 
    }
    void sLevel(int _level) {
        level = _level; 
    }
    void sValue(string _value) {
        value = _value;
    }
    string print() {
        return name + "//" + std::to_string(level) + ' ';
    }
};

class SymbolTable {

    Symbol ** arr;
    int currScope; 
    int size; 
    int capacity;

public:
    SymbolTable() {
        capacity = 100;
        arr = new Symbol * [capacity];
        for(int i = 0; i < capacity; i++) {
            arr[i] = new Symbol();
        }
        currScope = 0; 
        size = 0; 
    }

    void ensureCap(int oldCap) {
        int newCap = (oldCap * 3) / 2 + 1;
        Symbol ** arr1 = new Symbol * [newCap];
        for(int i = 0; i < size; i++) {
            arr1[i] = arr[i];
        }
        for(int i = size; i < newCap; i++) {
            arr1[i] = new Symbol();
        }
        delete [] arr;
        arr = arr1; 
        capacity = newCap;
    }

    bool lookUp(string _name) {
        if(size == 0) return 0; 
        for(int i = 0; i < size; i++) {
            if(arr[i]->getName() == _name && arr[i]->getLevel() == currScope) {
                return 1; 
            }
        }
        return 0;
    }

    Symbol * find(string _name) {
        if(size == 0) return NULL; 
        int index = -1;
        for(int i = 0; i < size; i++) {
            if(arr[i]->getName() == _name) index = i;
        }
        if(index < 0) return NULL;
        else return arr[index] ;
    }

    void insert(string _name, string _type) {
        if(size == capacity) {
            ensureCap(capacity);
        }
        arr[size]->sName(_name); arr[size]->sType(_type); arr[size]->sLevel(currScope);
        size += 1;
    }

    void assign(Symbol * s, string value) {
        s->sValue(value);
    }

    void ifBegin() {
        currScope += 1;
    }

    void ifEnd() {
        if(currScope > 0 && size > 0) {
            int start = -1;
            bool found = 0;
            for(int i = 0; i < size && !found; i++) {
                if(arr[i]->getLevel() == currScope) {
                    start = i;
                    found = 1;
                }
            }
            int temp = size;
            if(start > -1) {
                for(int i = start; i < temp; i++) {
                    arr[i]->sName(""); arr[i]->sType(""); arr[i]->sLevel(0); arr[i]->sValue("");
                    size -= 1;
                }
            }
        }
        currScope -= 1;
    }

    void print() {
        string output = ""; 
        for(int i = 0; i < size; i++) {
            bool isFound = 0;
            for(int j = i + 1; j < size && !isFound; j++) {
                if(arr[i]->getName() == arr[j]->getName() && arr[i]->getLevel() < arr[j]->getLevel()) {
                    isFound = 1;
                }
            }
            if(!isFound) output += arr[i]->print();
        }
        cout << output.substr(0, output.size() - 1) << '\n';
    }

    void rPrint() {
        string output = ""; 
        for(int i = 0; i < size; i++) {
            bool isFound = 0;
            for(int j = i + 1; j < size; j++) {
                if(arr[i]->getName() == arr[j]->getName() && arr[i]->getLevel() < arr[j]->getLevel()) {
                    isFound = 1;
                }
            }
            if(!isFound) output = arr[i]->print() + output;
        }
        cout << output.substr(0, output.size() - 1) << '\n';
    }

    ~SymbolTable() {
        for(int i = 0; i < capacity; i++) {
            delete arr[i];
        }
        delete [] arr;
    }

    void run(string filename);
};

#endif