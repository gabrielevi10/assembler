#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <stdlib.h>

#include "Instruction.hpp"
#include "Directive.hpp"
#include "Line.hpp"

using namespace std;

// Contém as instruções do assembly
map<string, Instruction> instructions_map;

// Contém as diretivas de montagem
map<string, Directive> directives_map;

// Relação das linhas do fonte original com o pre processado
map<int,int> lines_relations;

// Tabela de símbolos
map<string,int> symbol_table;

// Tabela de definições
map<string, int> definition_table;

// Marcam o inicio das seções bss e data
// usando o contador de tamanho de instruções
int beginning_section_data;
int beginning_section_bss;

// Indica se houve algum erro detectado por alguma das passagens,
// se sim, o montador não irá gerar arquivo objeto
bool error = false;

// Split de string a partir de um caractere
const vector<string> split(const string& s, const char& c) {    
	string buff{""};
	vector<string> v;
	
	for(auto n:s)
	{
		if(n != c) buff+=n; else
		if(n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);
	
	return v;
}

// Remove os espaços iniciais de uma string
string remove_initial_spaces(string in){
    while(in[0] == ' ') {
        in.erase(0,1);        
    }
    return in;
}

// Remove os espaços finais
string remove_final_spaces(string in){
    while(in[in.length()-1] == ' ') {        
        in.pop_back();
    }

    return in;
}
    
// Remove os espaços duplicados
string remove_unecessary_spaces(string in) {
    for(int i = 0; i < in.length(); i++) {
        while(in[i] == ' ' && in[i+1] == ' '){
            in.erase(i,1);        
        }
    }    
    return in;
}

// Deixa a linha o mais simples possível para leitura
string format_line(string line) {
    // Retira os comentários
    line = line.substr(0, line.find(';'));

    // Troca os tabs por espaços
    replace(line.begin(), line.end(),'\t',' ');

    // Remove os espaços iniciais
    line = remove_initial_spaces(line);

    // Remove os espaços finais
    line = remove_final_spaces(line);
    
    // Remove os espaços duplicados
    line = remove_unecessary_spaces(line);

    // Deixa em caixa baixa
    transform(line.begin(), line.end(), line.begin(), ::tolower);

    return line;
}

// Adiciona uma linha contendo o 'equ' ao respectivo mapa
void add_to_equ_map(string line, map<string, int> &map) {
    string label;
    int equ_value;

    // Pega o rótulo
    label = line.substr(0,line.find(':'));
    // Pega o valor do equate
    equ_value = stoi(line.substr(line.find("equ ")+4,line.length()));
    
    // Adiciona ao mapa
    map[label] = equ_value;
}

// Pega o valor do equ correspondente ao 'if'
int get_correspondant_equ(string line, map<string, int> &map) {
    // Pega o rótulo
    string label = line.substr(line.find("if ")+ 3, line.length());
    return map[label];
}

// Passagem zero do montador
void passage_zero(string program_name) {
    // Conta as linhas originais
    int original_line_counter = 1;
    // Conta as linhas do arquivo pre-processado
    int pre_line_counter = 1;
    string line;
    ifstream myfile (program_name + ".asm");    
    ofstream pre_processed;
    // Map contendo os rótulos dos equs e seus respectivos valores
    map<string, int> equ_map;    

    if (myfile.is_open()) {
        // Abre o arquivo de pre-processamento
        pre_processed.open(program_name + ".pre");
        while (getline (myfile,line)) {            
            // Formata a linha
            string clear_line = format_line(line);

            // Se ela for do tipo equate adiciona ao mapa
            if (clear_line.find("equ") != string::npos) {
                add_to_equ_map(clear_line,equ_map);                        
                lines_relations[pre_line_counter] = original_line_counter;
            }
            // Se for do tipo 'if', veja o valor do equate 
            else if (clear_line.find("if") != string::npos) {
                if (!get_correspondant_equ(clear_line,equ_map)) {
                    // Quando equ = 0 jogue fora a próxima linha
                    getline (myfile,line);
                    original_line_counter++;
                }
            }
            else if (clear_line != "") {
                // Para as outras instruções só copiar a linha formatada para o arquivo novo
                lines_relations[pre_line_counter] = original_line_counter;
                // pre_processed << original_line_counter << ' ' << pre_line_counter << ' ' << clear_line << endl;
                pre_processed << clear_line << endl;
                pre_line_counter++;     
            }
            original_line_counter++;
        }
        myfile.close();
        pre_processed.close();        
    }

    else {
        cout << "Arquivo \'" << program_name + ".asm\'" << " não existe em memória\n"; 
        exit(0);
    }
}

// Separa uma string em classe Line(contem label opcode e operandos)
Line token_separator(string input, int original_line) {
    string label, opcode;
    vector<string> operands;

    label  = "";
    opcode = "";

    // Se possuir dois ':' existem dois rótulos(erro)
    if(count(input.begin(), input.end(),':') == 2){
        cout << "Erro sintático: ";
        cout << "dois rótulos declarados na linha " << original_line;
        cout << endl;
        error = true;
    }

    // Pega o rótulo caso exista
    if(input.find(':') != -1) {        
        label = input.substr(0, input.find(':'));
        input = input.substr(input.find(':')+1, input.length());
    }

    // Pega o opcode
    input = remove_initial_spaces(input);
    if(input.find(' ') == -1) {
        // Para o caso dos que não possuem argumentos
        // Não existe espaço após a diretiva
        opcode = input;
        input = "";
    }
    else {
        // Para o caso das que possuem argumentos
        // Existe um espaço que separa o opcode deles
        opcode = input.substr(0, input.find(' '));
        input = input.substr(input.find(' ')+1,input.length());
    }    

    // Se achar uma vírgula possui dois argumentos
    if(input.find(',') != -1) {
        auto args = split(input,',');

        // Caso tenha mandado mais de dois argumentos avise o user o erro
        if(args.size() > 2) {
            cout << "Erro sintático: ";
            cout << "muitos argumentos dados(" << args.size() << ")";
            cout << " na linha " << original_line << endl;
            error = true;
        }
        // Add cada argumento ao vetor dedicado(operands)
        for(string s : args){
            s = remove_initial_spaces(s);
            if(!s.empty()) {
                operands.push_back(s);
            }
        }
    }
    // Se não achar virgula só possui um argumento
    else {
        // Remove espaços iniciais e finais desnecessários
        input = remove_initial_spaces(input);
        input = remove_final_spaces(input);
        
        if(!input.empty()) {
            operands.push_back(input);
        }
    }

    Line l(label,opcode,operands);
    return l;
}

// Valida se um token é válido
void validate_token(string token, int line_counter) {
    char first = token[0];

    if(isdigit(first)){
        cout << "Erro léxico, token começando com número na linha ";
        cout << line_counter << endl;
        error = true;
    }
    

    for(char i : token){
        // Se possuir um caractere que não seja alfanumerico nem underscore
        if(!isalpha(i) && !isdigit(i) && i != '_' ){
            // O token é inválido
            cout << "Erro léxico, token " << token;
            cout << " possui o caractere \'" << i;
            cout << "\' inválido na linha " << line_counter << endl;
            error = true;
        }
    }

}

// Retorna true se for uma instrução
bool is_a_instruction(string opcode) {
    return(instructions_map.find(opcode) == instructions_map.end()) ? false : true;        
}

// Retorna true se for uma diretiva
bool is_a_directive(string opcode) {
    return(directives_map.find(opcode) == directives_map.end()) ? false : true;
}

bool symbol_table_contains(string label) {
    return(symbol_table.find(label) == symbol_table.end()) ? false : true;
}

// Valida uma linha olhando seus tokens 
// e se a quantidade de instruções bate 
// com a exigida pela instrução
void validate_line(Line instruction, int line_counter) {
    // Valida quantidade de operandos dados
    int operands_expected;
    int operands_given = instruction.get_operands().size();
    string opcode = instruction.get_opcode();

    // Se for uma instrução buscar no mapa de instruções
    if(is_a_instruction(opcode)) {        
        operands_expected = instructions_map[instruction.get_opcode()].getOperand();                
    }

    else if(is_a_directive(opcode)) {
        operands_expected = directives_map[instruction.get_opcode()].getOperands();        
    }
    
    // Space pode receber 0 ou 1 operandos
    if((opcode == "space" && operands_given != 0 && operands_given != 1) ||
       (opcode != "space" && operands_given != operands_expected)) {

        cout << "Erro sintático, dado(s) " << operands_given;
        cout << " operandos e esperado(s) " << operands_expected;
        cout << " na linha " << line_counter << endl;
        error = true;        
    }

    // Valida o token do rótulo
    validate_token(instruction.get_label(),line_counter);
    // Valida o token dos operandos das instruções
    if(is_a_instruction(opcode)) {
        for(string operand : instruction.get_operands()){
            // Se possuir soma só avalie o token do operando em si
            if(operand.find(" + ") != -1) {            
                operand = operand.substr(0, operand.find(" + "));            
            }
            validate_token(operand,line_counter);
        }
    }
}

void passage_one(string file_name) {
    // Variavel que possui o tamanho das linas
    int size_counter = 0;
    // Conta em qual linha do arquivo pre-processado está
    int line_counter = 1;
    // Linha lida do arquivo pre-processado
    string line;
    // Seção atual(text,bss,data)
    string current_section;
    // Marca se existe seção text
    bool section_text = false;
    bool is_public;
    bool is_extern;
    ifstream myfile (file_name + ".pre");
    vector<string> splitted_label;

    while (getline (myfile,line)) {
        // Linha original relativa ao arquivo '.pre'
        int original_line = lines_relations[line_counter];

        // Quebra a linha em vários tokens
        Line instruction = token_separator(line, original_line);

        is_public = false;
        is_extern = false;

        if(instruction.get_opcode() == "section") {
            current_section = instruction.get_operands()[0];

            if(current_section == "text") {
                section_text = true;                  
            }
            // Se a seção for bss ou data e a seção de texto 
            // ainda não tiver sido declarada declare erro
            else if((current_section == "data" || 
                current_section == "bss") && !section_text) {

                cout << "Seção " <<  line.substr(line.find(' ')+1,line.length());
                cout << " decladarada antes da de texto na linha " << original_line << endl;
                error = true;
            }
            else if(current_section == "data") {
                // Marca o inicio da seção
                beginning_section_data = size_counter;
            }
            else if(current_section == "bss") {
                // Marca o inicio da seção
                beginning_section_bss = size_counter;
            }
            else {
                cout << "Erro sintático, seção \'" << current_section;
                cout << "\' indefinida" << endl;
                error = true;
            }

            line_counter++;
            continue;
        }      
        // Adiciona rótulo na tabela de símbolos(se existir)
        string this_label = instruction.get_label();
        if( !this_label.empty() ) {
            // verifica se o simbolo é público ou externo
            splitted_label = split(this_label, ' ');
            // se for público, vai pra tabela de definições
            if (splitted_label[0] == "public") {
                this_label = splitted_label[1];
                definition_table[this_label] = -1;
            }
            if (instruction.get_opcode() == "extern")
                is_extern = true; 
            // No caso de já conter o símbolo
            if(symbol_table_contains(this_label)) {                
                cout << "Erro semântico. Símbolo \'";
                cout << this_label << "\' redeclarado";
                cout << " na linha " << original_line << endl;
                error = true;
            }
            // se for externo, vai pra TS com o valor -1, indicando que é externo 
            else if (is_extern){
                symbol_table[this_label] = -1;
            }
            else {
                symbol_table[this_label] = size_counter;
            }
        }

        string opcode = instruction.get_opcode();
        if( is_a_instruction( opcode ) ) {
            // Para instruções fora da seção devida
            if(current_section != "text") {
                cout << "Erro sintático, instrução " << opcode;
                cout << " fora da seção devida";
                cout << " na linha " << original_line << endl;
                error = true;
            }

            // Valida instrução
            validate_line(instruction, original_line);
            // Incrementa o contador do tamanho das instruções
            size_counter += instructions_map[opcode].getLenght();
        }

        else if( is_a_directive( opcode ) ) {            
            // Para as diretivas space e const fora de suas seções devidas
            if(opcode == "space" && current_section != "bss" || 
                opcode == "const" && current_section != "data") {                   

                cout << "Erro sintático, diretiva " << opcode;
                cout << " fora da seção devida";
                cout << " na linha " << original_line << endl;
                error = true;
            }
            // Valida diretiva 
            validate_line(instruction, original_line);

            // Para o caso de space com argumento
            if(opcode == "space" && instruction.get_operands().size() == 1) {            
                int argument = stoi(instruction.get_operands()[0]);
                // Acrescenta n espaços de memoria
                size_counter += (argument - 1);
            }
            size_counter += directives_map[opcode].getLenght();
        }
        // Erro dos comandos que não são nem diretivas nem instruções
        else if ( !opcode.empty() ){
            cout << "Erro sintático, a instrução/diretiva \'";
            cout << opcode << "\' da linha " << original_line;
            cout << " não existe."  << endl;
            error = true;
        }
        
        line_counter++;
    }

    // Para o caso se seção texto faltante
    if(!section_text) {
        cout << "Seção de texto faltante";
        error = true;

    }
    myfile.close();
    for(auto i : definition_table) {
        definition_table[i.first] = symbol_table[i.first];
    }
}

void passage_two(string file_name) {
    int position_counter = 0;
    int line_counter = 1;
    int original_line;
    int opvalue;
    string line;
    ifstream my_file(file_name + ".pre");
    fstream auxiliar_file(file_name + ".aux", fstream::out);
    ofstream result_file(file_name + ".obj");
    bool isnt_in_st;
    bool is_module = false;
    bool exists_end = false;
    bool exists_public = false;
    bool exists_extern = false;
    vector<string> splitted_op;
    vector<int> values_v;
    map<string, vector<int>> use_table;
    vector<int> relative;

    while (getline(my_file, line)) {
        if (line == "section text" or line == "section bss" or line == "section data")
            continue;

        original_line = lines_relations[line_counter];

        opvalue = 0;

        Line actual_line = token_separator(line, original_line);
        // flag que informa se o operando não está na tabela de simbolo
        isnt_in_st = false;
        // verifica se os operandos estão na tabela de simbolos
        // para cada operando, se não for um número e não estiver na tabela, erro
        for (string operand : actual_line.get_operands()) {
            splitted_op = split(operand, ' ');
            if (!isdigit(operand[0]) and operand[0] != '-' and !symbol_table_contains(splitted_op[0])) {
                cout << "Erro sintático, o operando ";
                cout << operand << " na linha " << actual_line.get_label();
                cout << " não foi definido." << endl;
                error = true;
                isnt_in_st = true;
            }
        }

        if (is_a_instruction(actual_line.get_opcode())) {
            // salva a posição anterior do contador de posições para que, se preciso,
            // seja possível o acesso a um rotulo como operando no meio da expressão
            // como por exemplo copy n1 n2, para ter a posição de n1
            int back_pos = position_counter;
            position_counter += instructions_map[actual_line.get_opcode()].getLenght();
            if (!isnt_in_st and actual_line.get_operands().size() == instructions_map[actual_line.get_opcode()].getOperand()) {
                int aux = position_counter - back_pos;
                for (string operand : actual_line.get_operands()){
                    splitted_op = split(operand, ' ');
                    opvalue = symbol_table[splitted_op[0]];
                    if (opvalue == -1) {
                        use_table[splitted_op[0]].push_back(position_counter - aux + 1);
                    }
                    relative.push_back(position_counter - aux + 1);
                    aux--;
                    if (splitted_op.size() > 1) {
                        // verifica e faz a operação conforme passado para a instrução, como input n1 + 2
                        if (splitted_op[1] == "-") {
                            opvalue = opvalue - stoi(splitted_op[2]);
                        }
                        else if (splitted_op[1] == "+") {
                            opvalue = opvalue + stoi(splitted_op[2]);
                        }
                        else if (splitted_op[1] == "*") {
                            opvalue = opvalue * stoi(splitted_op[2]);
                        }
                        else if (splitted_op[1] == "/") {
                            opvalue = opvalue / stoi(splitted_op[2]);
                        }
                    }
                    values_v.push_back(opvalue);
                }
                auxiliar_file << instructions_map[actual_line.get_opcode()].getOpcode();
                auxiliar_file << " ";
                if (instructions_map[actual_line.get_opcode()].getOperand() != 0)
                    for(int i : values_v)
                        auxiliar_file << i << " ";
                values_v.clear();
            }
            else {
                cout << "Erro sintático, operando inválido na linha " << original_line << endl;
                error = true;
            }
        }
        else if (is_a_directive(actual_line.get_opcode())) {
            if (actual_line.get_opcode() == "space") {
                opvalue = 0;
                auxiliar_file << "00" << " ";
                if (actual_line.get_operands().size() > 0) {
                    for (string str : actual_line.get_operands())
                        opvalue += stoi(str);
                }
                for(int i = 1; i < opvalue; i++)
                    auxiliar_file << "00" << " ";
            }
            else if (actual_line.get_opcode() == "const") {
                auxiliar_file << stoi(actual_line.get_operands()[0]) << " ";
            }
            else if (actual_line.get_opcode() == "begin") {
                is_module = true;
                auxiliar_file << "TABLE USE\n" << endl;
                auxiliar_file << "TABLE DEFINITION\n" << endl;
                auxiliar_file << "RELATIVE\n" << endl;;
                auxiliar_file << "CODE\n";
            }
            else if (actual_line.get_opcode() == "end") {
                exists_end = true;
            }
        }
        else if (actual_line.get_opcode().size() > 0) {
            cout << "Erro sintático, operação não identificada na linha " << original_line << endl;
            error = true;
        }

        line_counter++;
    }

    auxiliar_file.close();
    auxiliar_file.open(file_name + ".aux");
    
    while(getline(auxiliar_file, line)) {
        result_file << line << endl;
        if (line == "TABLE USE") {
            for (auto i : use_table) {
                for (int x : i.second) {
                    result_file << i.first << " " << x << endl;
                }
            }
        }
        if (line == "RELATIVE") {
            for (int i : relative) {
                result_file << i << " ";
            }
            result_file << endl;
        }
    }

    if (is_module and !exists_end) {
        cout << "Erro semântico, uso de begin sem o uso de end" << endl;
        error = true;
    } 

    auxiliar_file.close();
    result_file.close();

    remove((file_name + ".aux").c_str());

    if (error) {
        remove((file_name + ".obj").c_str());
    }
}

// Carrega as instruções do assembly para memória(mapa)
void load_instructions() {
    // São passados o número de operandos, 
    // os opcodes e o tamnho da instrução
    instructions_map["add"]    = Instruction(1, 1, 2);
    instructions_map["sub"]    = Instruction(1, 2, 2);
    instructions_map["mult"]   = Instruction(1, 3, 2);
    instructions_map["div"]    = Instruction(1, 4, 2);
    instructions_map["jmp"]    = Instruction(1, 5, 2);
    instructions_map["jmpn"]   = Instruction(1, 6, 2);
    instructions_map["jmpp"]   = Instruction(1, 7, 2);
    instructions_map["jmpz"]   = Instruction(1, 8, 2);
    instructions_map["copy"]   = Instruction(2, 9, 3);
    instructions_map["load"]   = Instruction(1, 10, 2);
    instructions_map["store"]  = Instruction(1, 11, 2);
    instructions_map["input"]  = Instruction(1, 12, 2);
    instructions_map["output"] = Instruction(1, 13, 2);
    instructions_map["stop"]   = Instruction(0, 14, 1);
}

// Carrega as diretivas de montagem para memória
void load_directives() {
    // São passados o número de operandos e o tamanho da diretiva na memória
    directives_map["section"]  = Directive(1, 0);
    directives_map["space"]    = Directive(1, 1);
    directives_map["const"]    = Directive(1, 1);
    directives_map["public"]   = Directive(0, 0);
    directives_map["equ"]      = Directive(1, 0);
    directives_map["if"]       = Directive(1, 0);
    directives_map["extern"]   = Directive(0, 0);
    directives_map["begin"]    = Directive(0, 0);
    directives_map["end"]      = Directive(0, 0);
}

int main(int argc, char const *argv[]) {
    // Se não for passado o argumento não execute
    if(argc != 2) {
        cout << "Não foi passado o argumento do nome do arquivo\n";
        cout << "Encerrando execução\n";
        return -1;
    }    
    passage_zero(argv[1]);
    load_instructions();
    load_directives();
    passage_one(argv[1]);
    passage_two(argv[1]);

    cout << "Tabela de símbolos:" << endl;
    for(auto i : symbol_table) {
        cout << i.first << '-' << i.second << endl;
    }

    cout << "Tabela de definições:" << endl;
    for(auto i : definition_table) {
        cout << i.first << '-' << i.second << endl;
    }

    return 0;
}
