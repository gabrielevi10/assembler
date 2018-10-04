#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "Instruction.hpp"
#include "Directive.hpp"

using namespace std;

// Contém as instruções do assembly
map<string, Instruction> instructions_map;

// Contém as diretivas de montagem
map<string, Directive> directives_map;

// Relação das linhas do fonte original com o pre processado
map<int,int> lines_relations;

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
int passage_zero(string program_name) {
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
                //pre_processed << pre_line_counter << ' ' << original_line_counter << ' ' << clear_line << endl;
                lines_relations[pre_line_counter] = original_line_counter;
                pre_processed << clear_line << endl;
                pre_line_counter++;     
            }
            original_line_counter++;
        }
        myfile.close();
        pre_processed.close();

        return 0;
    }

    else {
        cout << "Arquivo \'" << program_name + ".asm\'" << " não existe em memória\n"; 
        return -1;
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

// Separa uma string em vários tokens
void token_separator(string input, int original_line) {
    string label, opcode;
    vector<string> operands;

    label  = "";
    opcode = "";

    // Se possuir dois ':' existem dois rótulos(erro)
    if(count(input.begin(), input.end(),':') == 2){
        cout << "Erro, dois rótulos declarados na linha " << original_line;
        cout << endl;
        exit(0);
    }

    // Pega o rótulo caso exista
    if(input.find(':') != -1) {        
        label = input.substr(0, input.find(':'));
        input = input.substr(input.find(':')+1, input.length());
    }

    // Pega o opcode
    input = remove_initial_spaces(input);
    opcode = input.substr(0, input.find(' '));
    input = input.substr(input.find(' ')+1,input.length());

    // Se achar uma vírgula possui dois argumentos
    if(input.find(',') != -1) {
        auto args = split(input,',');

        // Caso tenha mandado mais de dois argumentos avise o user o erro
        if(args.size() > 2) {
            cout << "Erro muitos argumentos dados(" << args.size() << ")";
            cout << " na linha " << original_line << endl;
        }
        // Add cada argumento ao vetor dedicado(operands)
        for(string s : args){
            s = remove_initial_spaces(s);
            if(!s.empty()) {
                operands.push_back(s);
            }
        }
    }
    // Se não achar, só possui um
    else {
        // Remove espaços iniciais e finais desnecessários
        input = remove_initial_spaces(input);
        input = remove_final_spaces(input);

        if(!input.empty()) {
            operands.push_back(input);
        }
    }

    // cout << label  << endl;
    // cout << opcode << endl;
    // cout << operands.size() << endl;
    // for(auto i : operands)
    //     cout << i << endl;
}

void passage_one(string file_name) {
    int line_counter = 1;
    string line;
    bool section_text = false;
    ifstream myfile (file_name + ".pre");

    while (getline (myfile,line)) {
        int original_line = lines_relations[line_counter];

        // Indica que a section text foi declarada
        if(line == "section text"){            
            section_text = true;
            line_counter++;
            continue;
        }
        // Se a seção for section data ou bss antes da seção texto alertar erro
        else if((line == "section data" || line == "section bss") and !section_text){
            cout << "Seção " <<  line.substr(line.find(' ')+1,line.length());
            cout << " decladarada antes da de texto na linha " << original_line << endl;
            exit(0);
        }
        
        token_separator(line, original_line);
        line_counter++;
    }

    // Para o caso se seção texto faltante
    if(!section_text) {
        cout << "Seção de texto faltante";
        exit(0);

    }
    myfile.close();
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
    //token_separator("a: rot: add par1, par2");    
    passage_one(argv[1]);


    //TODO: fazer os erros de token invalido (func que recebe um token)
    return 0;
}
