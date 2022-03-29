#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
//#include <io.h>
#include <string>
#include <math.h>
#include <vector>
#include "task.cpp"

using namespace std;

void sort(vector<Task*> &list)// will sort the array by priority (initial period) puts the highest priority/shortest period into the 0 position
{    
    int longestPeriodIndex=0;
    int shortestPeriodIndex=0;
    Task *temp = new Task();
    for(int i =0; i<list.size(); i++){  
        longestPeriodIndex = i;
        shortestPeriodIndex = i;
        for(int j= i; j<list.size(); j++){
            //if(list[j]->period < list[shortestPeriodIndex]->currentDeadline){ //list[j] has a higher priority than list[shortestPeriodIndex] and has the shortest period so far.
            if(list[j]->currentDeadline < list[shortestPeriodIndex]->currentDeadline){   
                shortestPeriodIndex = j;
            }
            //else if(list[j]->period > list[longestPeriodIndex]->currentDeadline){//list[j] has a lower priority than list[longestPeriodIndex] and has the longest period so far
            else if(list[j]->currentDeadline > list[longestPeriodIndex]->currentDeadline){   
                longestPeriodIndex = j;
            }
        }
        temp = list[shortestPeriodIndex];
        list[shortestPeriodIndex] = list[i];
        list[i] = temp; 
    }
}

void RMA(vector<Task*> &taskList, vector<string> &timing)
{// not sure where to put the aperiotic (whether they need to be top or low priority) {I prefer top, so before everything else}
    for(int currTimingIndex = 0; currTimingIndex<timing.size(); currTimingIndex++) // start looking at each timing segment then decide on what goes into that timing segment
    {
        cout << "NEW TIME **************************************************************************************** " << currTimingIndex << endl;
        string& currTiming = timing.at(currTimingIndex);
        for(int currTaskIndex = 0; currTaskIndex < taskList.size(); currTaskIndex++){ // look at each task and decide if it should go into that timing segment.
            Task* currTask = taskList.at(currTaskIndex);
            cout << currTask->id << " - released: " << currTask->released << " - currentExecutionCounter: " << currTask->currentExecutionCounter << " - current deadline:" << currTask->currentDeadline <<endl;
            //TODO check each task if it's passed it's deadline to see if it got missed or if it needs to be reset.
            if(currTimingIndex > currTask->currentDeadline && ((currTask->currentExecutionCounter != 0 && currTask->released == false) || (currTask->released == true))){ // it missed the deadline
                currTask->numMissedDeadlines += 1;
                cout << currTask->id << " - Missed it's deadline" << endl;
                currTask->released = false; // resets the task when it misses a deadline.
                currTask->currentExecutionCounter = 0;
            }
            
            if (currTimingIndex > currTask->currentDeadline-1 && currTask->currentExecutionCounter == 0 && currTask->released == false){ // we've passed its deadline and it doesn't need to finish.
                currTask->released = true;
                currTask->numExecutions += 1; 
                currTask->currentDeadline += currTask->period; // set the next deadline
                cout << currTask->id << " - is released and is reset" << endl;
            }

            if(currTask->released == false && currTask->currentExecutionCounter == 0){ //if it is not ready to execute and hasn't been preempted
                //do nothing - break?
                cout << currTask->id << " - is not ready to execute and hasn't been preempted " << endl;
                continue;
            }

            if(currTiming != ""){ //if something has been scheduled, it was put there by a higher priority task and go on to the next timing segment.
                //do nothing - break?
                cout << currTask->id << " - Wants to be scheduled but " << currTiming << " is scheduled" << endl;
                if(currTask->currentExecutionCounter != 0){
                    cout << currTask->id << " - has been preempted by " << currTiming<< endl; 
                    currTask->numPreemptions += 1;
                }
                continue;
            }
            
            if(currTask->released == false && currTask->currentExecutionCounter != 0){ // if it needs to finnish
                currTask->released = false;
                currTiming = currTask->id; // schedule it
                currTask->currentExecutionCounter -= 1; // decrement the number of time segments it needs to finish
                cout << currTask->id << " needs to finish with:" << currTask->currentExecutionCounter << " executions left." <<  endl;
            }
            else if(currTask->released == true){
                cout << currTask->id << " - is scheduled" << endl;
                currTiming = currTask->id; // schedule it
                currTask->released = false; // lock it
                currTask->currentExecutionCounter= currTask->executionTime-1; //set the number of times it needs to execute to finish
            }
            
        }
    }
    for(int currTaskIndex = 0; currTaskIndex < taskList.size(); currTaskIndex++){
        Task* currTask = taskList.at(currTaskIndex);
        cout << currTask->id << " - executed: " << currTask->numExecutions << "times, - preempted: " << currTask->numPreemptions << " times, - missed its deadline: " << currTask->numMissedDeadlines << endl;
    }
}

void update(vector<Task*> &tasks, int time)
{
    for (int i = 0; i < tasks.size(); i++)
    {
        if (time >= tasks[i]->currentDeadline)
        {
            tasks[i]->released = true;
        }
        if (time >= tasks[i]->currentDeadline + tasks[i]->period)
        {
            cout << tasks[i]->id << " missed its Deadline!" << endl;
            //list[i].currentPeriod += list[i].period; // if we decide to skip missed deadlines
        }
    }
    sort(tasks);
}

void EDF(vector<Task*> &tasks, vector<string> &timing2)
{
    cout << "made it to EDF" << endl;
    int consecutive = 0;
    for (int time = 0; time < timing2.size(); time++)// We go through each point 
    {
        Task* currTask = tasks.at(0); //functionally a cast from string to char*. Gets the ID from the 0 position in list
        timing2.at(time) = currTask->id;// If sort works, this should always be top priority
        consecutive++;
        if (consecutive > currTask->executionTime)// runs with expectation of FIFO then updates accordingly
        {
            currTask->released = false;
            currTask->currentDeadline += currTask->period;
            update(tasks, time);
            consecutive = 0;
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 3){
        fprintf(stdout, "There aren't enough files please have the execution be: ./scheduler.exe input.txt output.txt\n");
        return -1;
    }
    
    ifstream inputFile;
    inputFile.open(argv[1], ios::in);
    if(!inputFile.is_open()){
        fprintf(stdout, "The program couldn't open the input file.");
        return -1;
    }

    ofstream outputFile;
    outputFile.open(argv[2], ios::out);
    if(!outputFile.is_open()){
        fprintf(stdout, "The program couldn't open and write to the output file.");
        return -1;
    }


    int whileCount = 0;
    int totDuration = 0;
    int numTasks = 0;
    string line;
    vector<Task*> tasks; // implementing the tasks as a dynamic vector.

    while(getline(inputFile, line)){
        if (whileCount == 0){
            numTasks= stoi(line);
        }
        else if(whileCount == 1){
            totDuration = stoi(line);
        }
        else{
            //line will look like "A,10,200"
            Task* newTask = new Task(); // parse through the line to get the correct id executionTime and period
            int place1 = line.find(',',0);
            newTask->id = line.substr(0,place1);
            int place2 = line.find(',',place1+1);
            int len = place2 - 1 - place1;
            newTask->executionTime = stoi(line.substr(place1+1,len));
            newTask->period = stoi(line.substr(place2+1));
            newTask->currentDeadline = newTask->period;
            //newTask->currentExecutionCounter = newTask->executionTime;
            tasks.push_back(newTask); // this adds the pointer to the new task to the end of  the vector.
        }
        whileCount += 1;
    }

//    for(int j=0; j < tasks.size(); j++){
//         cout << tasks[j]->id << " - " << tasks[j]->executionTime << " - " << tasks[j]->period << " - " << tasks[j]->released << endl;
//     }
    
    vector <string> timing;
    timing.resize(totDuration);
    vector <string> timing2;
    timing2.resize(totDuration);

    //cout << "made it here in the execution" << endl;

    // for(int j=0; j < tasks.size(); j++){
    //     cout << tasks[j]->id << " - " << &tasks[j] << endl;
    // }
    // string *testString = new string(""); // to test to see if the string is empty create a testString
    // for(int j=0; j < timing.size(); j += 100){
    //     cout << timing[j] << " - " << &timing[j] << " - " << (timing[j] == *testString)  << " - " << j << " - " << timing.size()  <<  endl;
    // }


    sort(tasks);

    cout << "made it after the sort" << endl;
    for(int j=0; j < tasks.size(); j++){
        cout << tasks[j]->id << " - " << tasks[j]->executionTime << " - " << tasks[j]->period << " - " << tasks[j]->released << endl;
    }

    RMA(tasks, timing);

    cout << "made it past RMA" << endl;

    for(int q = 0; q < timing.size(); q++){
        cout << timing.at(q) << "-";
        if (q%50 == 0 && q !=0){
            cout << endl;
        }
    }
    cout << endl;
    
   for(int i = 0; i<tasks.size(); i++){
        Task* currTask = tasks.at(i);
        currTask->currentDeadline = currTask->period;
        currTask->currentExecutionCounter = 0;
        currTask->numExecutions = 0;
        currTask->numPreemptions = 0;
        currTask->released = true;
    }

    sort(tasks);

    cout << "EDF*****************************************************************************" << endl;
    for(int q = 0; q<timing2.size(); q++){
        cout << timing2.at(q) << " - ";

    }
    cout << endl;
    EDF(tasks, timing2);
    for(int q = 0; q<timing2.size(); q++){
        cout << timing2.at(q) << " - ";

    }
    cout << endl;
    
    cout << "made it past edf" << endl;

    tasks.clear();
    tasks.resize(0);
    
    timing.clear();
    timing.resize(0);

    timing2.clear();
    timing2.resize(0);


    inputFile.close();
    outputFile.close();
    return 0;
}
