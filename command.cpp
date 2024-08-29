#include"command.h"
#include"token.h"
#include<string.h>
#include<cstring>
#include<iostream>
#include<unistd.h>
#include<dirent.h>
#include<vector>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/wait.h>
#include<sys/types.h>

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
        //cout<<"prevDir OLD = "<<prevDir<<"\n";
        strcpy(prevDir, cwd);
        //cout<<"prevDir NEW = "<<prevDir<<"\n";
    }
    else{
        perror("getcwd");
    }
    

}

void handle_pwd(vector<string> tokens){
    char cwd[PATH_MAX];

    if(tokens.size() > 1){
        cout<<"error: too many arguments";
        exit(EXIT_FAILURE);
    }

    if(getcwd(cwd, sizeof(cwd)) != NULL){
        cout<<cwd<<"\n";
    }
    else{
        perror("getcwd");
    }
}

void print_permissions(struct stat *file_info)
{
    printf( (S_ISDIR(file_info->st_mode)) ? "d" : "-");
    printf( (file_info->st_mode & S_IRUSR) ? "r" : "-");
    printf( (file_info->st_mode & S_IWUSR) ? "w" : "-");
    printf( (file_info->st_mode & S_IXUSR) ? "x" : "-");
    printf( (file_info->st_mode & S_IRGRP) ? "r" : "-");
    printf( (file_info->st_mode & S_IWGRP) ? "w" : "-");
    printf( (file_info->st_mode & S_IXGRP) ? "x" : "-");
    printf( (file_info->st_mode & S_IROTH) ? "r" : "-");
    printf( (file_info->st_mode & S_IWOTH) ? "w" : "-");
    printf( (file_info->st_mode & S_IXOTH) ? "x" : "-");
}

void handle_ls(vector<string> tokens){
    bool showHidden = false;
    bool longFormat = false;
    char *dir = ".";
    DIR *dp;
    struct stat file_info;
    struct dirent *entry;
    
    //check flags
    for(int i=1; i<tokens.size(); i++){
        if(tokens[i][0] == '-'){
            if(tokens[i].find('a') != -1) showHidden = true;
            if(tokens[i].find('l') != -1) longFormat = true;
        }
        else{
            dir = tokens[i].data();
        }
    }

    dp = opendir(dir);
    if(dp == NULL){
        perror("ls");
        return;
    }

    // read directory enteries
    while((entry = readdir(dp)) != NULL)
    {
        // skip hidden files
        if(!showHidden && entry->d_name[0] == '.') continue;

        // long format
        if(longFormat){
            if(stat(entry->d_name, &file_info) == -1){
                perror("stat");
                continue;
            }

            print_permissions(&file_info);
            cout<<file_info.st_nlink<<" ";
            cout<<getpwuid(file_info.st_uid)->pw_name<<" ";
            cout<<getgrgid(file_info.st_gid)->gr_name<<" ";
            cout<<file_info.st_size<<" ";

            //print last modified time
            char time[100];
            struct tm *timeinfo = localtime(&file_info.st_mtime);
            strftime(time, sizeof(time), "%b %d %H:%M ", timeinfo);
            cout<<time;

            //print file name
            cout<<entry->d_name<<endl;
        }
        else{
            cout<<entry->d_name<<"\t";
        }
    }
    if(!longFormat) cout<<endl;

    closedir(dp);
}

void handle_system_commands(vector<string> tokens){
    int pid;
    int status;

    cout << tokens[0].c_str() << " " << *(char* const*)tokens.data() << endl;
    pid = fork();
    char* argv[] = { "/usr/bin/ls", ".", NULL };
    
    if(pid < 0){
        perror("fork()");
        return;
    }
    else if(pid == 0){
        // child process executes the command
        // if(execvp(tokens[0].c_str(), *(char* const**)tokens.data()) < 0){
        if(execvp(argv[0], argv) < 0){
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
        else{
            // parent process waits for the child process to complete
            do{
                waitpid(pid, &status, WUNTRACED);
            } while(!WIFEXITED(status) && !WIFSIGNALED(status));
        }
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
        handle_pwd(tokens);     
    else if(tokens[0] == "ls")                    //implement pwd command
        handle_ls(tokens);
    else    
        handle_system_commands(tokens);           //to execute non-built-in system commands

}
