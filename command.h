#ifndef COMMAND
#define COMMAND

#include<string>
#include<vector>

using namespace std;

void handle_echo(vector<string> tokens);
void handle_cd(vector<string> tokens);
void handle_pwd(vector<string> tokens);
void print_permissions(struct stat *file_info);
void handle_ls(vector<string> tokens);
void handle_system_commands(vector<string> tokens);
void handle_pinfo(vector<string> tokens);
void handleCommand(string command);

#endif