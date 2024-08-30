#include"prompt.h"
#include"token.h"
#include"seperate.h"
#include"special.h"
#include"command.h"

#include<stdio.h>
#include<iostream>
#include<limits.h>
#include<unistd.h>

using namespace std;

int main(){

    //get home directory
    char homeDir[PATH_MAX];
    if(getcwd(homeDir, PATH_MAX) == nullptr){
        cout<<"Can't access Home Directory\n";
        exit(EXIT_FAILURE);
    }

    setenv("HOME", homeDir, 1);
    
    while(true)
    {
        cout<<getPrompt(homeDir);

        string command_str;
        getline(cin, command_str);

        if(command_str == "exit"){
            exit(EXIT_SUCCESS);
        }

        vector<string> commands;
        commands = getCommands(command_str);

        split_and_execute(commands);

        // vector<string> tokens;
        // tokens = tokenize(commands);

    }

    return 0;
}