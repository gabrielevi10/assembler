#ifndef LINE_HPP
#define LINE_HPP

#include <string>
#include <vector>

using namespace std;

// Define o que uma linha precisa ter.
// Basicamente a opção de ter rótulos, opcode e operando(s)
class Line {
    public:
        Line(string, string, vector<string> );
        Line();
        // Debug apenas
        string to_print();

        string get_label();
        string get_opcode();
        vector<string> get_operands();

    private:
        string label, opcode;
        vector<string> operands;
        

};

#endif