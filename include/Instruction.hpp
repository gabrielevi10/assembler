#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>

class Instruction {
    public:
        Instruction(std::string, int, int, int);
        ~Instruction();

        std::string getMnemonic() {
            return mnemonic;
        }
        
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
        std::string mnemonic;
        int operand, opcode, lenght;  

};

#endif