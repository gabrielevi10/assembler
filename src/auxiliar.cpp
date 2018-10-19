#include "auxiliar.hpp"

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

    // Remove o carriage return(caso arquivo tenha sido gerado em Windows)
    line.erase( remove(line.begin(), line.end(), '\r'), line.end() );

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