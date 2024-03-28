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
//Structs:

struct resource{
    string type;
    string instance;
    int amountRes;
};

struct process{
    int deadline;
    int compTime;
    int amount;
    vector<string> actions;
};

//------------------------------------------------------------------------------------------------------------
//Functions:

void scheduler(){       //EDF Algorithm

}

void calcNeed(){

}

bool isSafe(){

}

bool request(int val){  //semaphore

}

void release(int val){  //semaphore

}

void destroy(){

}

void calculate(int val){

}

//------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {

    vector<resource> resources;
    vector<process> processes;

//------------------------------------------------------------------------------------------------------------
//File reading:

    if(argc != 3){                      //error if input not provided
        cout<< "No input argument is found.\n";
        return 1;
    }

    ifstream file1(argv[1]);
    if(!file1.is_open()){               //If it can't find file
        cout<< "No file can be found.\n";
        return 1;
    }

    string inputLine;
    int numVal;
    for(int i = 0; i < 2; i++){         //to get the m and n from file
        getline(file1, inputLine);
        stringstream str(inputLine);
        while (str >> numVal){
            if(i == 0){
                resource r = {.amountRes = numVal};
                resources.push_back(r);
                //cout<< "Resources: "<< resources.at(0).amountRes<< endl;
            } else if(i == 1){
                process p = {.amount = numVal};
                processes.push_back(p);
                //cout<< "Processes: "<<  processes.at(0).amount<< endl;
            }
        }
    }

    int max[resources.at(0).amountRes][processes.at(0).amount];
    int allocation[resources.at(0).amountRes][processes.at(0).amount];
    int available[resources.at(0).amountRes];

    for(int i = 0; i < 3; i++){
        getline(file1, inputLine);
        if(inputLine.find("available") != string::npos){
            int index = stoi(inputLine.substr(inputLine.find("available") + 10, 1)) - 1;
            available[index] = stoi(inputLine.substr(inputLine.find('=')+1));
            //cout<< inputLine.substr(inputLine.find("available") + 10, 1)<< endl;
            //cout<< inputLine.substr(inputLine.find('=')+1)<< endl;
        }
        if(inputLine.find("max") != string::npos) {
            int row = stoi(inputLine.substr(inputLine.find("max") + 4, 1)) - 1;
            int col = stoi(inputLine.substr(inputLine.find("max") + 6, 1)) - 1;
            max[row][col] = stoi(inputLine.substr(inputLine.find('=')+1));
            //cout<< row<< endl;
            //cout<< col<< endl;
        }
    }

    for(int r = 0; r < resources.at(0).amountRes; r++){         //filling up allocation matrix to contain 0's
        for(int c = 0; c < processes.at(0).amount; c++){
            allocation[r][c] = 0;
        }
    }

    ifstream file2(argv[2]);        //to gather the instances and contain them in the resources vector
    if(!file2.is_open()){               //If it can't find file
        cout<< "No file can be found.\n";
        return 1;
    }
    while(getline(file2, inputLine)){
        cout<< "hello\n";
    }


//------------------------------------------------------------------------------------------------------------
//Resource Management:



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
