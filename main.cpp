#include <iostream>
#include <map>
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
    map<string, vector<string>> instance;   //key will be the type and the value is the
    int amountRes;
};

struct process{
    int amount;
    map<int, vector<string>> actions;      //Key will be the process number and the value will be functions called for the process
    map<int, int> deadAndCompTime;         //Key will be deadline and the pair would be the computation time. The amount of them will be in correlation to their process
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

void useResource(){

}

//------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {

    vector<resource> resources;
    process p;

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
                p = {.amount = numVal};
                //processes.push_back(p);
                //cout<< "Processes: "<<  processes.at(0).amount<< endl;
            }
        }
    }

    int max[resources.at(0).amountRes][p.amount];
    int allocation[resources[0].amountRes][p.amount];
    int available[resources[0].amountRes];

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

    for(int r = 0; r < resources[0].amountRes; r++){         //filling up allocation matrix to contain 0's
        for(int c = 0; c < p.amount; c++){
            allocation[r][c] = 0;
        }
    }

    ifstream file2(argv[2]);            //accessing word file
    if(!file2.is_open()){               //If it can't find file
        cout<< "No file can be found.\n";
        return 1;
    }

    int count = 0;
    while(getline(file2, inputLine)){   //to gather the instances and contain them in the resources vector
        if(count >= resources[0].amountRes) {    //so it does not exceed the amount of resources
            cout<< "it has exceed the amount of resources\n";
            break;
        }else{
            if(resources.size() > count+1){     //if it hasn't been created just yet
                resource r = {.amountRes = resources[0].amountRes};
                resources.push_back(r);
            }
            if (inputLine.find(':') != string::npos) {
                string instances = inputLine.substr(inputLine.find(':') + 1);

                stringstream ss(instances);
                string inst;
                string type;

                getline(ss, type, ':');   //taking the Food:
                resources[count].instance[type];//adding the type into the key of the instance map

                while (getline(ss, inst, ',')) {
                    size_t start = inst.find_first_not_of(" ");
                    size_t end = inst.find_last_not_of(" ");

                    if (start != string::npos && end != string::npos) {
                        inst = inst.substr(start, end - start + 1);
                    }

                    resources[count].instance[type].push_back(inst);

                }
            }
            count++;
        }
    }
    file2.close();

    /*
    for (const auto& pair : resources[0].instance) {
        cout << "Key: " << pair.first << ", Values:";
        for (const auto& value : pair.second) {
            cout << " " << value;
        }
        cout << std::endl;
    }
     */

    int processNum;
    int d, c;   //deadline and computation time
    while(getline(file1, inputLine)){      //reading each process and putting them into p struct
        inputLine.erase(remove_if(inputLine.begin(), inputLine.end(), [](unsigned char c) { return std::isspace(c); }), inputLine.end());   //to remove any possible spaces
        if(inputLine.find("process") != string::npos){
            processNum = stoi(inputLine.substr(inputLine.find("process") + 8, 1));
            count = 0;
            //cout<< "This process is: "<< processNum<< endl;
        }else {
            if (count == 0) {     //deadline
                d = stoi(inputLine.substr(0));
                p.deadAndCompTime[d];
            } else if (count == 1) {       //computation time
                c = stoi(inputLine.substr(0));
                p.deadAndCompTime[d] = c;
            }else {
                p.actions[processNum].push_back(inputLine);
            }
            count++;
        }
    }

    /*for (const auto& pair : p.actions) {
        cout << "Process Number: " << pair.first << endl;
        cout << "Actions:" << endl;
        for (const auto& action : pair.second) {
            cout << action << endl;
        }
        cout << endl;
    }

    cout << "Deadlines and Computation Times:" << endl;
    for (const auto& pair : p.deadAndCompTime) {
        cout << "Deadline: " << pair.first << ", Computation Time: " << pair.second << endl;
    }
     */


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
