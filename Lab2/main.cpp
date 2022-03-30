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
    int shortestPeriodIndex=0;
    Task *temp;
    for(int i =0; i<list.size(); i++){
        shortestPeriodIndex = i;
        for(int j = i; j<list.size(); j++){
            if(list[j]->released){
                if(list[j]->Aperiotic == true && list[shortestPeriodIndex]->Aperiotic == false){// aperiotic vs periotic periotic as high priority
                    shortestPeriodIndex = j;
                }
                else if (list[j]->Aperiotic ==  list[shortestPeriodIndex]->Aperiotic){ // aperiotic vs aperiotic or periotic vs periotic
                    if(list[j]->currentDeadline < list[shortestPeriodIndex]->currentDeadline) // this is the original if of the sort
                        shortestPeriodIndex = j;
                }
            }
            else if (j == 0) // if the first spot is released, then it gets stuck and can't find the next unreleased one, this just finds the next released one.
            {
                for(int q = 0; q<list.size(); q++){
                    if(list[q]->released){
                        shortestPeriodIndex = q;
                    }
                }
            }
        }
        temp = list[shortestPeriodIndex];
        list[shortestPeriodIndex] = list[i];
        list[i] = temp;
    }
    
}

void RMA(vector<Task*> &taskList, vector<string> &timing, ofstream &outputFile)
{// not sure where to put the aperiotic (whether they need to be top or low priority) {I prefer top, so before everything else}
    outputFile << "************ RMA SCHEDULE ************" << endl;
    for(int currTaskIndex = 0; currTaskIndex < taskList.size(); currTaskIndex++  ){
        if(taskList.at(currTaskIndex)->Aperiotic){
            for(int currTimingIndex = taskList.at(currTaskIndex)->period; currTimingIndex < taskList.at(currTaskIndex)->period + taskList.at(currTaskIndex)->executionTime ; currTimingIndex++){
                timing.at(currTimingIndex ) = taskList.at(currTaskIndex)->id;
                taskList.at(currTaskIndex)->released == false;
            }
        }
    }
    sort(taskList);
    for(int currTimingIndex = 0; currTimingIndex<timing.size(); currTimingIndex++) // start looking at each timing segment then decide on what goes into that timing segment
    { 
        string& currTiming = timing.at(currTimingIndex);
        outputFile << currTimingIndex ;
        if(currTiming != ""){
            outputFile << " - " << currTiming << " - is scheduled";
        } 
        for(int currTaskIndex = 0; currTaskIndex < taskList.size(); currTaskIndex++){ // look at each task and decide if it should go into that timing segment.
            Task* currTask = taskList.at(currTaskIndex);
            // check each task if it's passed it's deadline to see if it got missed or if it needs to be reset.
            if(currTimingIndex >= currTask->currentDeadline && ((currTask->currentExecutionCounter != 0 && currTask->released == false) || (currTask->released == true))){ // it missed the deadline
                currTask->numMissedDeadlines += 1;
                outputFile << " - " << currTask->id << " - Missed it's deadline";
                currTask->released = false; // makes the task ready to be setup when it misses a deadline.
                currTask->currentExecutionCounter = 0;
            }
            
            if (currTimingIndex >= currTask->currentDeadline && currTask->currentExecutionCounter == 0 && currTask->released == false){ // we've passed its deadline and it doesn't need to finish.
                if(currTask->Aperiotic == true){
                    continue;
                } 
                currTask->released = true; //makes the task ready
                currTask->numExecutions += 1; 
                currTask->currentDeadline += currTask->period; // set the next deadline
            }

            if(currTiming != ""){ //if something has been scheduled, it was put there by a higher priority task and go on to the next timing segment.
                if(currTask->currentExecutionCounter != 0){ //|| currTask->released == true
                    outputFile  << " - " << currTask->id << " - has been preempted by " << currTiming;
                    currTask->numPreemptions += 1;
                }
                continue;
            }
            
            if(currTask->released == false && currTask->currentExecutionCounter != 0){ // if it needs to finnish
                currTask->released = false;
                currTiming = currTask->id; // schedule it
                outputFile << " - " << currTask->id << " - is scheduled";
                currTask->currentExecutionCounter -= 1; // decrement the number of time segments it needs to finish

            }
            else if(currTask->released == true){
                outputFile << " - " << currTask->id << " - is scheduled";
                currTiming = currTask->id; // schedule it
                currTask->released = false; // lock it
                currTask->currentExecutionCounter= currTask->executionTime-1; //set the number of times it needs to execute to finish
            }
            
        }
        outputFile << endl;
    }
    int totalDeadlineMisses = 0;
    int totalPreemptions = 0;
    for(int currTaskIndex = 0; currTaskIndex < taskList.size(); currTaskIndex++){
        Task* currTask = taskList.at(currTaskIndex);
        totalDeadlineMisses += currTask->numMissedDeadlines;
        totalPreemptions += currTask->numPreemptions;
        outputFile << currTask->id << " - got preempted: " << currTask->numPreemptions << " times, - missed its deadline: " << currTask->numMissedDeadlines << " times "<< endl;
    }
    outputFile << "Total number of preemptions for RMA: " << totalPreemptions << endl;
    outputFile << "Total number of deadline misses for RMA: " << totalDeadlineMisses << endl;
}

void update(vector<Task*> &tasks, int time, ofstream &outputFile)
{
    for (int currTaskIndex  = 0; currTaskIndex < tasks.size(); currTaskIndex++)
    {
        Task* currTask = tasks.at(currTaskIndex);
        if(time >= currTask->period && currTask->Aperiotic && currTask->currentExecutionCounter == 0){
            currTask->released = true;
        }
        if (time >= currTask->currentDeadline && (currTask->released == true || (currTask->currentExecutionCounter  > 0 && currTask->currentExecutionCounter != currTask->executionTime)) && currTask->Aperiotic == false)
        {
            outputFile << " - " << currTask->id << " missed its deadline";
            currTask->currentExecutionCounter = 0;
            currTask->numMissedDeadlines += 1;
            currTask->currentDeadline += currTask->period; // end it and just go to the next deadline.
            currTask->released = true;
        }
        else if (time >= currTask->currentDeadline && currTask->Aperiotic == false)
        {
            currTask->currentDeadline += currTask->period;
            currTask->released = true;
            currTask->currentExecutionCounter = 0;
        }
    }
    sort(tasks);
}

void EDF(vector<Task*> &tasks, vector<string> &timing2, ofstream &outputFile)
{
    outputFile << "************ EDF SCHEDULE ************" << endl;
    int consecutive = 0;
    sort(tasks);
    Task* currTask = tasks.at(0); // If sort works, this should always be top priority
    currTask->currentExecutionCounter = 0;
    for (int currTimeIndex = 0; currTimeIndex < timing2.size(); currTimeIndex++)// We go through each point 
    {
        outputFile << currTimeIndex;
        update(tasks, currTimeIndex, outputFile);
        if(currTask->released == true){//we're executing
            currTask->currentExecutionCounter++;
            timing2.at(currTimeIndex) = currTask->id;
            outputFile << " - " << currTask->id << " is scheduled";
            consecutive++;
        }
        if(currTask->currentExecutionCounter >= currTask->executionTime){ // we're done executing
            currTask->released = false;
            consecutive = 0;
            update(tasks, currTimeIndex, outputFile);
            currTask = tasks.at(0);
        }
        outputFile << endl;
    }
    int totalDeadlineMisses = 0;
    int totalPreemptions = 0;
    for(int currTaskIndex = 0; currTaskIndex < tasks.size(); currTaskIndex++){
        Task* currTask = tasks.at(currTaskIndex);
        totalDeadlineMisses += currTask->numMissedDeadlines;
        totalPreemptions += currTask->numPreemptions;
        outputFile << currTask->id << " - missed its deadline: " << currTask->numMissedDeadlines << " times "<< endl;
    }
    outputFile << "Total number of deadline misses for EDF: " << totalDeadlineMisses << endl;
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
    bool onAperiodic = false;
    string line;
    vector<Task*> tasks; // implementing the tasks as a dynamic vector.

    while(getline(inputFile, line)){
        if (whileCount == 0){
            numTasks = stoi(line);
        }
        else if(whileCount == 1){
            totDuration = stoi(line);
        }
        else if(!onAperiodic && whileCount < numTasks + 2){
            //line will look like "A,10,200"
            Task* newTask = new Task(); // parse through the line to get the correct id executionTime and period
            int place1 = line.find(',',0);
            newTask->id = line.substr(0,place1);
            int place2 = line.find(',',place1+1);
            int len = place2 - 1 - place1;
            newTask->executionTime = stoi(line.substr(place1+1,len));
            newTask->period = stoi(line.substr(place2+1));
            newTask->currentDeadline = newTask->period;
            tasks.push_back(newTask); // this adds the pointer to the new task to the end of  the vector.
        }
        else if(whileCount == numTasks + 2){// will start getting 
            onAperiodic = true;
            numTasks += stoi(line)+1;
        }
        else//****************NEW FOR SETTING UP APERIODIC TASK*********************
        {
            Task* newTask = new Task(); // parse through the line to get the correct id executionTime and period
            int place1 = line.find(',',0);
            newTask->id = line.substr(0,place1);
            int place2 = line.find(',',place1+1);
            int len = place2 - 1 - place1;
            newTask->executionTime = stoi(line.substr(place1+1,len));
            newTask->Aperiotic = true;
            newTask->released = false;
            newTask->period = stoi(line.substr(place2+1));// period will act as release time for aperiodic
            newTask->currentDeadline = totDuration;
            tasks.push_back(newTask); // this adds the pointer to the new task to the end of  the vector.
        }
        whileCount += 1;
    }

    
    vector <string> timing;
    timing.resize(totDuration);
    vector <string> timing2;
    timing2.resize(totDuration);

    sort(tasks);

    RMA(tasks, timing, outputFile);
    
   for(int i = 0; i<tasks.size(); i++){
        Task* currTask = tasks.at(i);
        if (!(currTask->Aperiotic)){ // because period will releasetime so we want their deadline to stay the end of the program
            currTask->currentDeadline = currTask->period;
        }
        currTask->currentExecutionCounter = 0;
        currTask->numExecutions = 0;
        currTask->numPreemptions = 0;
        currTask->released = true;
        if(currTask->Aperiotic){
            currTask->released = false;
        }
    }

    sort(tasks);

    EDF(tasks, timing2, outputFile);

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
