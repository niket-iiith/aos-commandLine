#include "token.h"
#include<string>
#include<string.h>
#include<iostream>

using namespace std;


vector<string> tokenize(string command_str)
{
    vector<string> tokens;

    char* command = new char[command_str.size() + 1];
    strcpy(command, command_str.c_str());
    const char* delimeters = " \t\n";

    char* token = strtok(command, delimeters);
    while(token != nullptr){
        tokens.push_back(string(token));
        token = strtok(nullptr, delimeters);
    }

    return tokens;
}