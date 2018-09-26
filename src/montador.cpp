#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

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
    line = line.substr(0,line.find(';'));

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

int passage_zero(string program_name) {
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
                pre_processed << clear_line << endl;
            }
            // Se for do tipo 'if', veja o valor do equate 
            else if (clear_line.find("if") != string::npos) {
                if(!get_correspondant_equ(clear_line,equ_map)){
                    // Quando equ = 0 jogue fora a próxima linha
                    getline (myfile,line);
                }
            }
            else {
                // Para as outras instruções só copiar a linha formatada
                pre_processed << clear_line << endl;
            }
        }
        myfile.close();
        pre_processed.close();

    }

    else {
        cout << "Arquivo \'" << program_name + ".asm\'" << " não existe em memória\n"; 
    }
}

int main(int argc, char const *argv[]) {    
    if(argc != 2) {
        cout << "Não foi passado o argumento do nome do arquivo\n";
        cout << "Encerrando execução\n";
        return -1;
    }
    else {
        passage_zero(argv[1]);
    }    

    return 0;
}
