#include "Line.hpp"

Line::Line(string l, string opc, vector<string> ops) {
    this->label    = l;
    this->opcode   = opc;
    this->operands = ops;
}

string Line::to_print(){
    string ret;    
    ret = "rotulo (" + label + ") opcode (" + opcode + ") -> ";
    for(auto i : operands) {
        ret += '(' + i + ')';
    }    
    return ret;
}

string Line::get_label(){
    return this->label;
}

string Line::get_opcode(){
    return this->opcode;    
}

vector<string> Line::get_operands(){
    return this->operands;
}
