#include"special.h"
#include"command.h"
#include"token.h"
#include<iostream>
#include<string>
#include<cstring>
#include<vector>
#include<unistd.h>
#include <sys/wait.h>
#include <cerrno>

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
            cout << "Process running in background with PID: " << pid << endl;
        } 
        else {
            fprintf(stderr, "Maximum background processes reached.\n");
        }
    }
}

void handle_pipe(vector<string> commands) 
{
    int num_pipes = commands.size() - 1;
    int pipefds[2 * num_pipes]; // Each pipe needs two file descriptors

    // Create all pipes needed
    for (int i = 0; i < num_pipes; ++i) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }
    }

    int command_index = 0;
    for (const auto& command : commands) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            // If not the first command, redirect input from the previous pipe
            if (command_index != 0) {
                if (dup2(pipefds[(command_index - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                }
            }

            // If not the last command, redirect output to the next pipe
            if (command_index != num_pipes) {
                if (dup2(pipefds[command_index * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe file descriptors
            for (int i = 0; i < 2 * num_pipes; ++i) {
                close(pipefds[i]);
            }

            // Prepare the command and arguments for execvp
            vector<string> args = split_string(command); // Use the helper function
            vector<char*> c_args;
            for (auto& arg : args) {
                c_args.push_back(const_cast<char*>(arg.c_str()));
            }
            c_args.push_back(nullptr); // Null-terminate the array

            // Execute the command
            if (execvp(c_args[0], c_args.data()) < 0) {
                perror("Exec failed");
                exit(EXIT_FAILURE);
            }
        }
        ++command_index;
    }

    // Close all pipe file descriptors in the parent
    for (int i = 0; i < 2 * num_pipes; ++i) {
        close(pipefds[i]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i <= num_pipes; ++i) {
        wait(NULL);
    }
}

vector<string> split_string(const string &str) {
    vector<string> tokens;
    string token;
    size_t start = 0, end = 0;

    while ((end = str.find(' ', start)) != string::npos) {
        if (end != start) {
            tokens.push_back(str.substr(start, end - start));
        }
        start = end + 1;
    }

    if (start < str.size()) {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}


void split_and_execute(vector<string> commands){
    int i = 0; 
    int n = commands.size();
    
    while(n--)
    {
        if(commands[i].find("&") != string::npos){
            handleBackground(commands[i]);
        }
        else if(commands[i].find("|") != string::npos)
        {
            vector<string> command;

            const char* token;
            token = strtok(commands[i].data(), "|");
            
            while(token != NULL){
                command.push_back(token);
                token = strtok(NULL, "|");
            }

            handle_pipe(command);
        }
        else{
            handleCommand(commands[i]);
        }
        i++;
    }

}