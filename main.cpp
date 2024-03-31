#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

//compile to test g++ -o test main.cpp
using namespace std;

//------------------------------------------------------------------------------------------------------------
//Structs:

struct resource{
    map<string, vector<string>> instance;   //key will be the type and the value is the
    int amountRes = 0;
};

struct process{
    int amount = 0;
    int processNum = 0;
    int deadline = 0;
    int compTime = 0;
    vector<string> actions;
};

struct Semaphore{   //binary Semaphore for wait/signal
    private:
        mutex mtx;
        condition_variable cv;
        size_t avail;
    public:
        explicit Semaphore(int avail_ = 1) : avail(avail_){}

        void wait(){
            unique_lock<mutex> lock(mtx);
            cv.wait(lock,[this] {return avail > 0;});
            avail--;
        }
        void signal(){
            unique_lock<mutex> lock(mtx);
            avail++;
            cv.notify_one();
        }
        size_t available() const{
            return avail;
        }
};

//------------------------------------------------------------------------------------------------------------
//Global functions:
    Semaphore accessSema(1);

//------------------------------------------------------------------------------------------------------------
//Functions:

bool compareTasks(const process& a, const process& b) {
    return a.deadline < b.deadline;
}

vector<process> scheduler(vector<process>& p){       //EDF Algorithm
    vector<process> scheduledProcess;
    int currTime = 0;

    sort(p.begin(), p.end(), compareTasks);

    for (const process& task : p) {
        // Check for schedulability
        int totalExecutionTime = 0;
        for (const process& t : scheduledProcess) {
            if (t.deadline >= task.deadline) {
                totalExecutionTime += t.compTime;
            }
        }
        if (totalExecutionTime + task.compTime > currTime + task.deadline) {
            cerr << "Error: Task " << task.processNum << " cannot be scheduled! EDF is not scheduled." << endl;
            return scheduledProcess; // Indicate an unscheduled scenario
        }

        // Schedule the task (start time is current ti
        scheduledProcess.push_back({task.amount, task.processNum, task.deadline, task.compTime, task.actions});
        currTime += task.compTime;
    }

    return scheduledProcess;
}

void calcNeed(){

}

bool isSafe(){

}

bool request(int val){  //semaphore
    cout<< "request has been called\n";
    return true;
}

void release(int val){  //semaphore

}

void destroy(){

}

void calculate(int val){

}

void useResource(){

}

void processAction(process task){
    cout<< "Process #"<< task.processNum<< endl;
    for(auto & action : task.actions){
        accessSema.wait();

        if(action.find("request") != string::npos){
            //cout<< action.substr(action.find('(')+1, action.find(')')-1 - action.find('('))<< endl;
            request(stoi(action.substr(action.find('(')+1, action.find(')')-1 - action.find('('))));
            //cout<< "calling requesting\n";
        }else if(action.find("release") != string::npos){

            //cout<< "calling release\n";
        }else if(action.find("calculate") != string::npos){

            //cout<< "calling calculate\n";
        }else if(action.find("use_resources") != string::npos){

            //cout<< "calling resources\n";
        }else if(action.find("print_resources_used") != string::npos){

            //cout<< "calling print resources\n";
        }else if(action.find("end.") != string::npos){

            //cout<< "calling end\n";
        }

        accessSema.signal();
    }
}

//------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {

    vector<resource> resources;
    vector<process> p;
    vector<thread> workingThreads;
    string masterString;

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
                process proc = {.amount = numVal};
                p.push_back(proc);
                //cout<< "Processes: "<<  processes.at(0).amount<< endl;
            }
        }
    }

    int max[resources.at(0).amountRes][p[0].amount];
    int allocation[resources[0].amountRes][p[0].amount];
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
        for(int c = 0; c < p[0].amount; c++){
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
            if(p.size() == processNum){
                p[0].processNum = processNum;
            }else if(processNum > p.size()){
                process proc = {.amount = p[0].amount};
                proc.processNum = processNum;
                p.push_back(proc);
            }
            count = 0;
            //cout<< "This process is: "<< processNum<< endl;
        }else {
            if (count == 0) {     //deadline
                d = stoi(inputLine.substr(0));
                p[processNum-1].deadline = d;
            } else if (count == 1) {       //computation time
                c = stoi(inputLine.substr(0));
                p[processNum-1].compTime = c;
            }else {
                p[processNum-1].actions.push_back(inputLine);
            }
            count++;
        }
    }

    /*for(int i = 0; i < p.size(); i++){
        cout<<"Process: "<< p[i].processNum<< endl;
        cout<< "Deadline: "<< p[i].deadline<< endl;
        cout<< "Computation Time: "<< p[i].compTime<< endl;

        cout<< "Actions:"<< endl;
        for(int x = 0; x < p[i].actions.size(); x++){
            cout<< p[i].actions[x]<< endl;
        }
        cout<< endl;
    }
     */

//------------------------------------------------------------------------------------------------------------
//Scheduling
    vector<process> scheduledTasks = scheduler(p);

    /*if (!scheduledTasks.empty()) {
        cout << "Scheduled Tasks (ID, Computation Time, Deadline):" << endl;
        for (const process& task : scheduledTasks) {
            cout << task.processNum << "  " << task.compTime << "  " << task.deadline << endl;
        }
    }
     */

//------------------------------------------------------------------------------------------------------------
//Process Management(Semaphores)

    for(auto &scheduledTask : scheduledTasks){          //creating concurrency w/threads
        thread workThread(&processAction, scheduledTask);
        workingThreads.push_back(std::move(workThread));
    }

    for (auto &workerThread : workingThreads) {
        workerThread.join();
    }

    return 0;
}
