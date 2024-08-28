#include<string.h>
#include<vector>
#include<iostream>
#include<limits>
#include"seperate.h"

#define MAX_TOKENS 20

using namespace std;

vector<string> getCommands(string command)
{
    vector<string> commands;

    const char* token;
    token = strtok(command.data(), ";");
    
    int count = 0;
    while(token != NULL)
    {
        count++;

        //error handler
        if(count >= MAX_TOKENS){
            cout<<"error: too many commands";
        }

        commands.push_back(token);
        token = strtok(NULL, ";");
    }

    return commands;
}