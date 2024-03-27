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

void release(int val){

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

    ifstream file2(argv[2]);        //to gather the instances and contain them in the resources vector
    while(getline(file2, inputLine)){
        if(inputLine.find("available") != string::npos){

        }
        if(inputLine.find("max") != string::npos){

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
