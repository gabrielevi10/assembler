#include "Instruction.hpp"

using namespace std;

Instruction::Instruction(string mnemonic, int operand, int opcode, int lenght) {
    this->mnemonic = mnemonic;
    this->operand = operand;
    this->opcode = opcode;
    this->lenght = lenght;
}

Instruction::~Instruction() {}