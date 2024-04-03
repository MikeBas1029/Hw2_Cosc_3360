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
#include <cstdlib>
#include <ctime>

//compile to test g++ -o test main.cpp
//compile w/threads g++ -o test main.cpp -pthread
using namespace std;

//------------------------------------------------------------------------------------------------------------
//Structs:
struct BankerData{
    int max[50][50]; // large number to make sure it'll fix. but I can adjust it if it needs to resized
    int allocation[50][50];
    int available[50];
    int need[50][50];
    int row;        //reference to num of processes
    int column;     //reference to num of resources
};

struct resource{
    map<string, vector<string>> instance;   //key will be the type and the value is the
    int amountRes = 0;
};

struct process{     //will contain Laxity & arrival time for tiebreaker in the case inside EDF scheduler
    int amount = 0;
    int processNum = 0;
    int deadline = 0;
    int compTime = 0;
    int arrivalTime = 0;
    int laxity;
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
BankerData bankerData;
vector<process> p;
vector<resource> resources;
string masterString;
//------------------------------------------------------------------------------------------------------------
//Functions:

void sortLLF(vector<process>::iterator begin, vector<process>::iterator end, int currTime) {
    // Sort based on deadline - executionTime - currTime (descending laxity)
    sort(begin, end,[currTime](const process& a, const process& b) { return (a.deadline - a.compTime - currTime) > (b.deadline - b.compTime - currTime); });
}

bool compareTasks(const process& a, const process& b) {
    return a.deadline < b.deadline;
}

vector<process> scheduler(vector<process>& p){       //EDF Algorithm
    vector<process> scheduledProcess;
    int currTime = 0;

    sort(p.begin(), p.end(), compareTasks);

    for (process& task : p) {
        // Check for schedulability
        int totalExecutionTime = 0;
        for (const process& t : scheduledProcess) {
            if (t.deadline >= task.deadline) {
                totalExecutionTime += t.compTime;
            }
        }
        if (totalExecutionTime + task.compTime > currTime + task.deadline) {
            cerr << "Error: Task " << task.processNum << " cannot be scheduled. EDF is not scheduled." << endl;
            return scheduledProcess; // Indicate an unscheduled scenario
        }

        // Schedule the task & give Laxity
        task.laxity = task.deadline - task.compTime - currTime;
        scheduledProcess.push_back(task);

        if (any_of(scheduledProcess.begin(), scheduledProcess.end() - 1,
                   [&task](const process& t) { return t.deadline == task.deadline; })) {
            // Custom function for LLF sorting
            sortLLF(scheduledProcess.begin(), scheduledProcess.end(), currTime);
        }


        currTime += task.compTime;
    }

    return scheduledProcess;
}

void calcNeed(){
    for(int r = 0; r < bankerData.row; r++){
        for(int c = 0; c < bankerData.column; c++){
            bankerData.need[r][c] = bankerData.max[r][c] - bankerData.allocation[r][c];
        }
    }
}

bool isSafe(){
    calcNeed();
    int proc = bankerData.row;
    int res = bankerData.column;

    bool finish[proc] = {0};
    int safeSeq[proc];
    int work[res];

    for(int c = 0; c < res; c++){
        work[c] = bankerData.available[c];
    }

    int count = 0;
    while(count < proc){
        bool found = false;
        for(int p = 0; p < proc; p++){
            if(finish[p] == 0){
                int c;
                for(c = 0; c < res; c++){
                    if(bankerData.need[proc][c] > work[c])
                        break;
                }
                if(c == res){
                    for(int i = 0; i < res; i++){
                        work[i] += bankerData.allocation[proc][i];
                    }
                    safeSeq[count++] = proc;
                    finish[proc] = 1;
                    found = true;
                }
            }
        }
        if(!found){
            cout<< "System not in safe state\n";
            return false;
        }
    }
    /*cout << "System is in safe state.\nSafe"
            " sequence is: ";
    for (int i = 0; i < proc; i++) {
        cout << safeSeq[i] << " ";
    }
    cout<< endl;
     */
    return true;    //Safe state
}

void request(int rAndValRequest){
    cout<< "Requesting: "<< rAndValRequest<< endl;
    rAndValRequest--;
    bankerData.allocation[rAndValRequest][1] = rAndValRequest;
    isSafe();       //Banker's Algo check if it's an x by 1 matrix
}
void request(int r, int c, int valRequest){
    cout<< "Requesting: ("<< r<< ", "<< c<< ") = "<< valRequest<< endl;
    r--; c--;
    bankerData.allocation[r][c] = valRequest;
    isSafe();       //Banker's Algo check if it's an x by y matrix
}

void release(int r){
    cout<< "Releasing: "<< r<< endl;
    r--;
    bankerData.allocation[r][1] = 0;
    isSafe();       //Banker's Algo check if it's an x by 1 matrix
}
void release(int r, int c, int valRelease){
    cout<< "Releasing: ("<< r<< ", "<< c<< ") = "<< valRelease<< endl;
    r--; c--;
    bankerData.allocation[r][c] = valRelease;
    isSafe();       //Banker's Algo check if it's an x by y matrix
}

void calculate(int procNum, int val){
    cout<< "Calculating: "<< val<< endl; //<< " into process #"<< procNum<<
    p[procNum-1].compTime += val;
}

void useResource(int val1, int val2){
    cout<< "Using Resource: ("<< val1<< ", "<< val2<< ")"<< endl;

    resource res = resources[val1-1];
    auto it = res.instance.begin();
    string key = it->first;
    vector<string> strings = it->second;
    int randomStringIndex = rand() % strings.size();
    //cout<< "Random instance: "<< strings[randomStringIndex]<< endl;
    if(masterString.find(key) != string::npos){
        masterString+= ", " + strings[randomStringIndex];
    }else{
        masterString += key + ": " + strings[randomStringIndex];
    }

    p[val1-1].compTime -= val1;     //reducing computation time after execution
    //cout<< masterString<< endl;
}

string orderProperly(string masStr){
    int coloPos = masStr.find(':');
    string sectionToSort = masStr.substr(coloPos + 2);

    vector<string> instList;
    int start = 0;
    int commaPos = sectionToSort.find(',');
    while(commaPos != string::npos){
        instList.push_back(sectionToSort.substr(start, commaPos - start));
        start = commaPos + 2;
        commaPos = sectionToSort.find(',', start);
    }

    instList.push_back(sectionToSort.substr(start));     //last instance in masterString

    sort(instList.begin(), instList.end());

    for(int i = 0; i < instList.size() - 1; i++){
        if(instList[i] == instList[i+1]){
            instList.erase(instList.begin() + i + 1);
            instList[i] += "s";
        }
    }

    string sorted;
    for(const string& inst: instList){
        sorted += inst + ", ";
    }

    sorted.erase(sorted.length() - 2);

    sorted = masStr.substr(0, masStr.find(':')+2) + sorted;

    return sorted;
}

void printResource(int procNum){
    masterString = orderProperly(masterString);         //updating masterString to remove duplicates and be in order alphabetically
    cout<< "Printing out used resources ->"<< masterString<< endl;

    p[procNum-1].compTime--;        //reducing computation time after execution
}

vector<int> extractNumbers(const string& str) {
    int startPos = str.find('(') + 1;
    int endPos = str.find(')');
    string numbersStr = str.substr(startPos, endPos - startPos);

    istringstream ss(numbersStr);
    vector<int> numbers;
    int num;

    // Extract numbers using stringstream
    while (ss >> num) {
        numbers.push_back(num);
    }

    return numbers;
}

void processAction(process task){
    bool twoInts;
    accessSema.wait();
    for(auto & action : task.actions){
        //accessSema.wait();
        if(action.find(',') != string::npos) {
            twoInts = true;
        }else{
            twoInts = false;
        }
        if((action.find("request") != string::npos)){
            //cout<< action.substr(action.find('(')+1, action.find(')')-1 - action.find('('))<< endl;
            if(twoInts){
                vector<int> numbers = extractNumbers(action);
                request(numbers[0], numbers[1], numbers[2]);
            }else{
                int val = stoi(action.substr(action.find('(')+1, action.find(')')-1 - action.find('(')));
                request(val);
            }
        }else if(action.find("release") != string::npos){
            int val = stoi(action.substr(action.find('(')+1, action.find(')')-1 - action.find('(')));
            release(val);
        }else if(action.find("calculate") != string::npos){
            int val = stoi(action.substr(action.find('(')+1, action.find(')')-1 - action.find('(')));
            calculate(task.processNum, val);
        }else if(action.find("use_resources") != string::npos){
            int one = stoi(action.substr(action.find('(')+1, 1));
            int two = stoi(action.substr(action.find(',')+1, 1));
            useResource(one ,two);
        }else if(action.find("print_resources_used") != string::npos){
            printResource(task.processNum);
        }else if(action.find("end.") != string::npos){
            cout<< endl;
        }

        //accessSema.signal();
    }
    accessSema.signal();
}

//------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {

    vector<thread> workingThreads;

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
                bankerData.row = numVal;        //for knowing the range for when doing bankers
                resource r = {.amountRes = numVal};
                resources.push_back(r);
                //cout<< "Resources: "<< resources.at(0).amountRes<< endl;
            } else if(i == 1){
                bankerData.column = numVal;     //for knowing the range for when doing bankers
                process proc = {.amount = numVal};
                p.push_back(proc);
                //cout<< "Processes: "<<  processes.at(0).amount<< endl;
            }
        }
    }

    //Info for Banker's Avoidance
    /*int max[resources.at(0).amountRes][p[0].amount];
    int allocation[resources[0].amountRes][p[0].amount];
    int available[resources[0].amountRes];*/

    for(int i = 0; i < p[0].amount+1; i++){
        getline(file1, inputLine);
        if(inputLine.find("available") != string::npos){
            int index = stoi(inputLine.substr(inputLine.find("available") + 10, 1)) - 1;
            bankerData.available[index] = stoi(inputLine.substr(inputLine.find('=')+1));
            //cout<< inputLine.substr(inputLine.find("available") + 10, 1)<< endl;
            //cout<< inputLine.substr(inputLine.find('=')+1)<< endl;
        }
        if(inputLine.find("max") != string::npos) {
            int row = stoi(inputLine.substr(inputLine.find("max") + 4, 1)) - 1;
            int col = stoi(inputLine.substr(inputLine.find("max") + 6, 1)) - 1;
            bankerData.max[row][col] = stoi(inputLine.substr(inputLine.find('=')+1));
            //cout<< row<< endl;
            //cout<< col<< endl;
        }
    }

    for(int r = 0; r < resources[0].amountRes; r++){         //filling up allocation matrix to contain 0's
        for(int c = 0; c < p[0].amount; c++){
            bankerData.allocation[r][c] = 0;
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
                    size_t start = inst.find_first_not_of(' ');
                    size_t end = inst.find_last_not_of(' ');

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

    file1.close();
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
        cout<< "Process #"<< scheduledTask.processNum<< " -> (Deadline: "<< scheduledTask.deadline<< ", Computation Time: "<< scheduledTask.compTime<< ")"<< endl;
        thread workThread(&processAction, scheduledTask);
        workingThreads.push_back(std::move(workThread));
    }

    for (auto &workerThread : workingThreads) {
        workerThread.join();
    }

    return 0;
}
