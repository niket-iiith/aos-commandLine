#ifndef COMMAND
#define COMMAND

#include<string>
#include<vector>

using namespace std;

void handle_echo(vector<string> tokens);
void handle_cd(vector<string> tokens);
void handle_pwd(vector<string> tokens);
void handleCommand(string command);

#endif