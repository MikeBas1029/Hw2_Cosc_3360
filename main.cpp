#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//compile to test g++ -o test main.cpp
using namespace std;
//------------------------------------------------------------------------------------------------------------
//Functions:

void calcNeed(){

}

bool isSafe(){

}

bool request(){

}

void release(){

}

//------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    vector<string> resources;
    int processes;
    int resourcesNum;

//------------------------------------------------------------------------------------------------------------
//File reading:

    if(argc != 3){              //error if input not provided
        cout<< "No input argument is found.\n";
        return 1;
    }
    ifstream file(argv[1]);

    if(!file.is_open()){        //If it can't find file
        cout<< "No file can be found.\n";
        return 1;
    }

    string inputLine;
    int numVal;
    for(int i = 0; i < 2; i++){     //to get the m and n from file
        getline(file, inputLine);
        stringstream str(inputLine);
        while (str >> numVal){
            if(i == 0){
                resourcesNum = numVal;
                //cout<< "Resources: "<< resourcesNum<< endl;
            } else if(i == 1){
                processes = numVal;
                //cout<< "Processes: "<<  processes<< endl;
            }
        }
    }

//------------------------------------------------------------------------------------------------------------
//Resource Management:

    //int sem_id = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);

//------------------------------------------------------------------------------------------------------------
//Process Management:


//------------------------------------------------------------------------------------------------------------
//Deadlock Avoidance(Banker's Algorithm)

//------------------------------------------------------------------------------------------------------------
//Completion


//------------------------------------------------------------------------------------------------------------
//Output


    return 0;
}
