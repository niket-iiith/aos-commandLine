#include<stdio.h>
#include<iostream>
#include<limits.h>
#include<unistd.h>
#include"prompt.h"
#include"token.h"

using namespace std;

int main(){

    //get home directory
    char homeDir[PATH_MAX];
    if(getcwd(homeDir, PATH_MAX) == nullptr){
        cout<<"Can't access Home Directory\n";
        exit(EXIT_FAILURE);
    }
    
    while(true)
    {
        cout<<getPromptfromENV(homeDir);

        string command_str;
        getline(cin, command_str);

        if(command_str == "exit"){
            exit(EXIT_SUCCESS);
        }

        vector<string> tokens;
        tokens = tokenize(command_str);

    }

    return 0;
}