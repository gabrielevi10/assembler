#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>

#include "auxiliar.hpp"

using namespace std;

#define debug true

map<string, int> global_definition_table;

vector<map<string, int>> all_files_def_tables;

vector<string> all_files_relatives;

vector<map<string, vector<int>>> use_tables;

vector<string> all_files_code;

// Preenche as estruturas de dados para que a linkagem seja possível
void fill_data_structures(string file_name) {
    ifstream file(file_name + ".obj");
    string line;
    bool tb = false;
    bool tf = false;
    bool rel = false;
    vector<string> splitted_line, aux;
    map<string, vector<int>> actual_use_table;
    map<string, int> auxiliar_table;

    while (getline(file, line)) {
        // Preenche a ED responsável pelas tabelas de uso de todos os arquivos passados
        if (line == "TABLE USE") {
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
            all_files_relatives.push_back(line);
        }
        // Preenche a ED responsável pelos códigos de todos os arquivos passados
        else if (line == "CODE"){
            getline(file, line);
            all_files_code.push_back(line);
        }
    }
}

void make_global_def_table() {
    int correction_factor = 0;
    int index = 0;
    vector<string> code;

    for(auto j : all_files_def_tables) {
        for (auto k : j) {      
            global_definition_table[k.first] = k.second + correction_factor;   
        }
        code = split(all_files_code[index], ' ');
        correction_factor = correction_factor + code.size();
        index++;
    }
}

void generate_executable(int number_of_objs, string name) {
    ofstream executable_file(name + ".e");
    string line;
    vector<string> code;
    vector<string> relatives;
    int correction_factor = 0;
    int aux, index;

}

int main(int argc, const char *argv[]) {
    for (int i = 0; i < argc; i++) {
        fill_data_structures(argv[i]);
    }

    make_global_def_table();
    
    generate_executable(argc - 1, argv[0]);

    #ifdef debug
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

        cout << "Relativos" << endl;
        for (auto i : all_files_relatives){
            cout << i << endl;
        }
    #endif

    return 0;
}