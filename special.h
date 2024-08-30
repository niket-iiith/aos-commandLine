#ifndef SPECIAL
#define SPECIAL

#include<vector>
#include<string>

using namespace std;

void handle_pipe(vector<string> commands);
vector<string> split_string(const string &str);
void split_and_execute(vector<string> commands);

#endif