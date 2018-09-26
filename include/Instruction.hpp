#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>

class Instruction {
    public:
        Instruction(int, int, int);
        Instruction();
        ~Instruction();
        
        int getOperand() {
            return operand;
        }

        int getOpcode() {
            return opcode;
        }

        int getLenght() {
            return lenght;
        }

    private:        
        int operand, opcode, lenght;  

};

#endif