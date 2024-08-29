#include"special.h"
#include"command.h"
#include"token.h"
#include<iostream>
#include<string>
#include<cstring>
#include<vector>
#include<unistd.h>

using namespace std;

#define MAX_BG_PROCESSES 100

typedef struct {
    pid_t pid;
    char command[256];
} BackgroundProcess;

BackgroundProcess bg_processes[MAX_BG_PROCESSES];
int bg_count = 0; // Number of background processes


void handleBackground(string command)
{
    //tokenize the command
    vector<string> tokens;
    tokens = tokenize(command);

    //return if no command found
    if(tokens.size() == 0) return;
    tokens.pop_back();
    int pid = fork();
    
    if(pid < 0){
        perror("fork()");
        return;
    }
    else if(pid == 0){
        // child process executes the command
       
        if(execvp(tokens[0].c_str(), (char* const*)tokens.data() ) < 0){
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
    } 
    else{
        // add the background process to the list and print the PID
        if (bg_count < MAX_BG_PROCESSES) {
            bg_processes[bg_count].pid = pid;
            strcpy(bg_processes[bg_count].command, tokens[0].c_str());
            bg_count++;
            cout << "Process running in background with PID: " << pid;
        } 
        else {
            fprintf(stderr, "Maximum background processes reached.\n");
        }
    }
}

void handlePipe(){

}

void split_and_execute(vector<string> commands){
    int i = 0; 
    int n = commands.size();
    
    while(n--)
    {
        if(commands[i].find('&') < 0){
            handleBackground(commands[i]);
        }
        else if(commands[i].find('|') < 0){
            handlePipe();
        }
        else{
            handleCommand(commands[i]);
        }
        i++;
    }

}