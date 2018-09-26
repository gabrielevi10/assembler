#include "Instruction.hpp"

using namespace std;

Instruction::Instruction(int operand, int opcode, int lenght) {
    this->operand = operand;
    this->opcode = opcode;
    this->lenght = lenght;
}
Instruction::Instruction() {}
Instruction::~Instruction() {}