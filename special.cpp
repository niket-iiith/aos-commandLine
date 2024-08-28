#include"special.h"
#include"command.h"
#include<iostream>
#include<string>
#include<vector>

using namespace std;

void split_and_execute(vector<string> commands){
    int i = 0; 
    int n = commands.size();
    //cout<<"I am in split_and_execute() "<<n<<endl;
    while(n--)
    {
        // if(commands[i].find('&') != NULL){}
        //     //handleBackround();
        // else if(commands[i].find('|') != NULL){}
        //     //handlePipe();
        // else{
            handleCommand(commands[i]);
        // }
        i++;
    }

}