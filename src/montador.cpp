#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "Instruction.hpp"
#include "Directive.hpp"

using namespace std;

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

// Deixa a linha o mais simples possível para leitura
string format_line(string line) {
    // Retira os comentários
    line = line.substr(0, line.find(';'));

    // Troca os tabs por espaços
    replace(line.begin(), line.end(),'\t',' ');

    // Remove os espaços iniciais
    while(line[0] == ' ') {
        line.erase(0,1);        
    }

    // Remove os espaços finais
    while(line[line.length()-1] == ' ') {        
        line.pop_back();
    }   

    // Remove os espaços desnecessários para facilitar a leitura posterior
    for(int i=0; i < line.length(); i++){
        if(line[i] != ' '){
            continue;
        }
        else if(line[i] == ' ' and line[i+1] == ' '){
            line.erase(i,1);
        }
    }
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
    // Relação das linhas do fonte original com o pre processado
    map<int,int> lines_relations;

    if (myfile.is_open()) {
        // Abre o arquivo de pre-processamento
        pre_processed.open(program_name + ".pre");
        while (getline (myfile,line)) {            
            // Formata a linha
            string clear_line = format_line(line);

            // Se ela for do tipo equate adiciona ao mapa
            if (clear_line.find("equ") != string::npos) {
                add_to_equ_map(clear_line,equ_map);
                
                // Copia a linha formatada para o arquivo novo
                pre_processed << pre_line_counter << ' ' << original_line_counter << ' ' << clear_line << endl;
                pre_line_counter++;
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
                pre_processed << pre_line_counter << ' ' << original_line_counter << ' ' << clear_line << endl;
                pre_line_counter++;     
                lines_relations[pre_line_counter] = original_line_counter;
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
void load_instructions(map<string, Instruction> &map) {
    // São passados o número de operandos, 
    // os opcodes e o tamnho da instrução
    map["add"]    = Instruction(1, 1, 2);
    map["sub"]    = Instruction(1, 2, 2);
    map["mult"]   = Instruction(1, 3, 2);
    map["div"]    = Instruction(1, 4, 2);
    map["jmp"]    = Instruction(1, 5, 2);
    map["jmpn"]   = Instruction(1, 6, 2);
    map["jmpp"]   = Instruction(1, 7, 2);
    map["jmpz"]   = Instruction(1, 8, 2);
    map["copy"]   = Instruction(2, 9, 3);
    map["load"]   = Instruction(1, 10, 2);
    map["store"]  = Instruction(1, 11, 2);
    map["input"]  = Instruction(1, 12, 2);
    map["output"] = Instruction(1, 13, 2);
    map["stop"]   = Instruction(0, 14, 1);
}

// Carrega as diretivas de montagem para memória
void load_directives(map<string, Directive> &map) {
    // São passados o número de operandos e o tamanho da diretiva na memória
    map["section"]  = Directive(1, 0);
    map["space"]    = Directive(1, 0);
    map["const"]    = Directive(1, 0);
    map["public"]   = Directive(1, 0);
    map["equ"]      = Directive(1, 0);
    map["if"]       = Directive(1, 0);
    map["extern"]   = Directive(1, 0);
    map["begin"]    = Directive(1, 0);
    map["end"]      = Directive(1, 0);
}


int main(int argc, char const *argv[]) {
    // Contém as instruções do assembly
    map<string, Instruction> instructions_map;

    // Contém as diretivas de montagem
    map<string, Directive> directives_map;

    if(argc != 2) {
        cout << "Não foi passado o argumento do nome do arquivo\n";
        cout << "Encerrando execução\n";
        return -1;
    }
    else {
        passage_zero(argv[1]);
        //load_instructions(instructions_map);
        //load_directives(directives_map);       
    }    

    return 0;
}
