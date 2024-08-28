#include"command.h"
#include"token.h"
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<vector>

#define PATH_MAX 1024

using namespace std;

void handle_echo(vector<string> tokens)
{
    int n = tokens.size()-1;
    int i = 1;

    while(n--){
        cout<<tokens[i];
        if(n != 0)
            cout<<" ";
        i++;
    }
    cout<<"\n";
}

void handle_cd(vector<string> tokens){
    char* homeDir = getenv("HOME");
    static char prevDir[PATH_MAX] = "";
    char cwd[PATH_MAX];


    if(tokens.size() == 1 || tokens[1].compare("~") == 0){
        //No argument or '~': Change to home directory
        if(chdir(homeDir) != 0)
            perror("cd");
    }
    else if(tokens[1] == "-"){
        //change to previous directory
        if(prevDir[0] == '\0')
            cout<<"error: OLDPWD not set\n";
        else{
            cout<<prevDir<<"\n";
            if(chdir(prevDir) != 0)
                perror("cd");
        }
    }
    else if(tokens[1] == ".."){
        //change to parent directory
        if(chdir("..") != 0)
            perror("cd");
    }
    else if(tokens[1] == "."){
        //do nothing
    }
    else{
        if(chdir(tokens[1].data()) != 0)
            perror("cd");
    }

    //Update previous directory to the current directory
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        cout<<"prevDir OLD = "<<prevDir<<"\n";
        strcpy(prevDir, cwd);
        cout<<"prevDir NEW = "<<prevDir<<"\n";
    }
    else{
        perror("getcwd");
    }
    

}

void handle_pwd(vector<string> tokens){
    char cwd[PATH_MAX];

    if(getcwd(cwd, sizeof(cwd)) != NULL){
        cout<<cwd<<"\n";
    }
    else{
        perror("getcwd");
    }
}


//called in special.cpp if there is no pipe or a background command
void handleCommand(string command)
{
    //tokenize the command
    vector<string> tokens;
    tokens = tokenize(command);

    //return if no command found
    if(tokens.size() == 0) return;

    if(tokens[0] == "echo")
        handle_echo(tokens);                      //implement echo command
    else if(tokens[0] == "cd")
        handle_cd(tokens);                        //implement cd command
    else if(tokens[0] == "pwd")
        handle_pwd(tokens);                       //implement pwd command
    // else    
        //handle_system_commands(tokens);         //to execute non-built-in system commands


}
