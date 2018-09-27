#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

class Directive {
    public:
        Directives(int, int);
        Directives();
        ~Directives();

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