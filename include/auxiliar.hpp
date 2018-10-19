#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

const vector<string> split(const string &s, const char &c);

string remove_initial_spaces(string in);

string remove_final_spaces(string in);

string remove_unecessary_spaces(string in);

string format_line(string line);