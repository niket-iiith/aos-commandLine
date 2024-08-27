#include"prompt.h"
#include"token.h"
#include<limits>
#include<unistd.h>

using namespace std;

string getPromptfromENV(string homeDir)
{
    // get _username_ from environment variable "USER"
    const char* _username_ = getenv("USER");
    if(_username_ == nullptr){
        perror("getenv");
        exit(EXIT_FAILURE);
    }

    // get _system_name_ from system
    char _system_name_[_POSIX_HOST_NAME_MAX+1];
    if(gethostname(_system_name_, _POSIX_HOST_NAME_MAX) < 0){
        perror("gethostname");
        exit(EXIT_FAILURE);
    }

    // get current working directory
    char _current_directory_[PATH_MAX];
    if(getcwd(_current_directory_, PATH_MAX) == nullptr){
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    string currDir(_current_directory_);
    if(currDir.find(homeDir) == 0){
        currDir.replace(0, homeDir.length(), "~");
    }


    string prompt = string(_username_) + "@" + _system_name_ + ":" + _current_directory_ + ">";

    return prompt;
}