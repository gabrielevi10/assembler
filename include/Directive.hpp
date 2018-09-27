#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

class Directive {
    public:
        Directive(int, int);
        Directive();
        ~Directive();

        int getOperands() {
            return operands;
        }

        int getLenght() {
            return lenght;
        }

    private:
       int operands, lenght;

};

#endif