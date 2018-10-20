#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <stdlib.h>

#include "auxiliar.hpp"

using namespace std;

#define dbg false

map<string, int> global_definition_table;

vector<map<string, int>> all_files_def_tables;

vector<vector<int>> all_files_relatives;

vector<map<string, vector<int>>> use_tables;

vector<vector<int>> all_files_code;

bool is_module = false;

// Retorna true se estiver na tabela de uso do indice passado
bool is_in_table_use(string label, int index) {
    return(use_tables[index].find(label) == use_tables[index].end()) ? false : true;        
}

// Preenche as estruturas de dados para que a linkagem seja possível
void fill_data_structures(string file_name) {
    ifstream file(file_name + ".obj");
    string line;
    vector<string> splitted_line, aux;
    map<string, vector<int>> actual_use_table;
    map<string, int> auxiliar_table;

    while (getline(file, line)) {
        // Preenche a ED responsável pelas tabelas de uso de todos os arquivos passados
        if (line == "TABLE USE") {
            is_module = true;
            while (getline(file, line) && !line.empty()) {
                splitted_line = split(line, ' ');
                actual_use_table[splitted_line[0]].push_back(stoi(splitted_line[1]));
            }
            use_tables.push_back(actual_use_table);
        }
        // Preenche a tabela global de definições
        else if (line == "TABLE DEFINITION") {
            while(getline(file, line) && !line.empty()) {
                splitted_line = split(line, ' ');
                auxiliar_table[splitted_line[0]] = stoi(splitted_line[1]);
            }
            all_files_def_tables.push_back(auxiliar_table);
        }
        // Preenche a ED responsável pelos relativos de todos os arquivos passados
        else if (line == "RELATIVE") {
            getline(file, line);
            splitted_line = split(line, ' ');
            vector<int> aux;
            for (string x : splitted_line)
                aux.push_back(stoi(x));
            all_files_relatives.push_back(aux);
        }
        // Preenche a ED responsável pelos códigos de todos os arquivos passados
        else if (line == "CODE"){
            getline(file, line);
            splitted_line = split(line, ' ');
            vector<int> aux;
            for (string x : splitted_line)
                aux.push_back(stoi(x));
            all_files_code.push_back(aux);
        }
        if (!is_module) {
            splitted_line = split(line, ' ');
            vector<int> aux;
            for (string x : splitted_line)
                aux.push_back(stoi(x));
            all_files_code.push_back(aux);
        }
    }
}

// Monta a tabela global de definições
void make_global_def_table() {
    int correction_factor = 0;
    int index = 0;
    vector<int> code;

    for(auto j : all_files_def_tables) {
        for (auto k : j) {      
            global_definition_table[k.first] = k.second + correction_factor;   
        }
        code = all_files_code[index];
        correction_factor = correction_factor + code.size();
        index++;
    }
}

// Gera o executavel, com o nome do primeiro .obj passado
void generate_executable(int number_of_objs, string name) {
    ofstream executable_file(name + ".e");
    string line;
    vector<int> code;
    vector<int> relatives;
    vector<int> aux_v;
    int correction_factor = 0;
    int aux, index = 0;

    if (!is_module) {
        for (int i : all_files_code[0]) {
            executable_file << i << " ";
        }
        return;
    }

    for (auto c : all_files_code) {
        code = c;
        for (auto t : global_definition_table) {
            if(is_in_table_use(t.first, index)){
                for (int i : use_tables[index][t.first]) {
                    code[i] = t.second;
                    aux_v.push_back(i);
                }
            }
        }
        if (!all_files_relatives[index].empty()) {
            for (int k : all_files_relatives[index]) {
                if (!(find(aux_v.begin(), aux_v.end(), k) != aux_v.end())) {
                    relatives.push_back(k);
                }      
            }
        }
        aux_v.clear();
        relatives.erase(unique(relatives.begin(), relatives.end()), relatives.end());

        for (int s : relatives) {
            code[s] = code[s] + correction_factor;
        }

        for (int i : code) {
            executable_file << i << " ";
        }

        correction_factor = code.size();
        index++;
        relatives.clear();
    } 

    executable_file.close(); 
}

int main(int argc, const char *argv[]) {
    for (int i = 0; i < argc; i++) {
        fill_data_structures(argv[i]);
    }

    make_global_def_table();
    
    generate_executable(argc - 1, argv[1]);

    #if dbg
        cout << "Tabelas de uso" << endl;
        for (auto i : use_tables) {
            for (auto x : i) {
                cout << x.first;
                    for (auto z : x.second)
                        cout << " " << z << " ";
                cout << endl;
            }
        }

        cout << "Tabela Global de definição" << endl;
        for (auto i : global_definition_table) {
            cout << i.first << " " << i.second << endl;
        }
    #endif

    return 0;
}