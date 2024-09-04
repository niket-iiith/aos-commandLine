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
#include<fstream>

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

    pid = fork();    
    vector<char*> c_args;

    // Convert vector<string> to vector<char*> for execvp
    for (const auto& token : tokens) {
        c_args.push_back(const_cast<char*>(token.c_str()));
    }
    c_args.push_back(nullptr); // Null-terminate the array
    
    if(pid < 0){
        perror("fork()");
        return;
    }
    else if(pid == 0){
        // child process executes the command
        if(execvp(c_args[0], c_args.data()) < 0){
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
    }
    else{
        // parent process waits for the child process to complete
        do{
            waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

}







void handle_pinfo(vector<string> tokens){
    string pid;

    if(tokens.size() > 1) pid = tokens[1];
    else pid = to_string(getpid());

    string status = "/proc/" + pid + "/status";
    string exe = "/proc/" + pid + "/exe";

    //open status file to read process information
    ifstream file(status);
    
    string data, state, vm;
    
    while(getline(file, data)){
        if(data.find("State") == 0) state = data.substr(7);
        else if(data.find("VmSize:") == 0) vm = data.substr(8);
    }
    file.close();

    char exePath[1024];
    int pathLen = readlink(exe.c_str(), exePath, sizeof(exePath)-1);
    exePath[pathLen] = '\0';

    cout<<"pid -- "<<pid<<endl;
    cout<<"status -- "<<state<<endl;
    cout<<"memory -- "<<vm<<endl;
    cout<<"path -- "<<exePath<<endl;

}





bool fileSearch(string path, string sFile)
{
    DIR* dir = opendir(path.c_str());
    if(dir == nullptr){
        perror("directory");
        return false;
    }

    struct dirent* entry;
    while((entry = readdir(dir)) != nullptr){
        string name = entry->d_name;

        if(name == "." || name == "..")
            continue;

        if(name == sFile){
            closedir(dir);
            return true;
        }        

        if (entry->d_type == DT_DIR) {
            string new_path = path + "/" + name;
            if (fileSearch(new_path, sFile)) {
                closedir(dir);
                return true;
            }
        }
    }
}



void handle_search(vector<string> tokens)
{
    //if filename not provided or extra arguments are passed, return to prompt
    if(tokens.size() != 2){
        cout<<"error: syntax -- search <filename> " << endl;
        return;
    }

    string sFile = tokens[1];
    string _current_directory_ = ".";

    if(fileSearch(_current_directory_, sFile)) cout << "True\n";
    else cout << "False\n";

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
    else if(tokens[0] == "pinfo")                 //implement pinfo
        handle_pinfo(tokens);
    else if(tokens[0] == "search")
        handle_search(tokens);
    else    
        handle_system_commands(tokens);           //to execute non-built-in system commands
}
