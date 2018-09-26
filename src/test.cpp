#include "Instruction.hpp"
#include <iostream>

using namespace std;

int main() {
    Instruction add("add", 1, 1, 2);
    Instruction sub("sub", 1, 2, 2);
    Instruction mult("mult", 1, 3, 2);
    Instruction div("div", 1, 4, 2);
    Instruction jmp("jmp", 1, 5, 2);
    Instruction jmpn("jmpn", 1, 6, 2);
    Instruction jmpp("jmpp", 1, 7, 2);
    Instruction jmpz("jmpz", 1, 8, 2);
    Instruction copy("copy", 2, 9, 3);
    Instruction load("load", 1, 10, 2);
    Instruction store("store", 1, 11, 2);
    Instruction input("input", 1, 12, 2);
    Instruction output("output", 1, 13, 2);
    Instruction stop("stop", 0, 14, 1);

    cout << add.getMnemonic();
    return 0;
}