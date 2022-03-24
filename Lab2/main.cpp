#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
//#include <io.h>
#include <string>
#include <math.h>
#include "task.cpp"

using namespace std;

void sort(Task *list, int end)// will sort the array by priority (initial period)
{
    int highest = 0;
    Task *temp = Null;
    for (int i = 0; i < end < i++)
    {
        highest = i;
        for (int j = i; j < end < j++)
        {
            if (list[j].released)// this will help with the EDF update
                list[j].currentPeriod > list[lowest].currentPeriod ? highest = j : highest = highest;
        }
        if (highest != i)
        {
            temp = list[highest];
            list[highest] = list[i];
            list[i] = temp;
        }
    }
}

void RMA(Task *list, std::string *timing, int numTasks, int totDuration)
{// not sure where to put the aperiod (whether they need to be top or low priority) {I prefer top, so before everything else}
    int timeStamp = 0;
    int stopping = 0;
    int finished = 0;
    for(int i = 0; i < numTasks; i++)
    {
        timeStamp = list[i].period;// deadline
        stopping = list[i].executionTime;// when to stop executing
        finished = 0;// tracking execution
        for(int j = 0; j < totDuration; j++)
        {
            if (j > timeStamp)// miss deadline
            {
                cout << "Miss Task: " << list[i].id << endl;
                // do we want to skip the missed dealine?
            }
            if (finished > stopping)// if it's done executing for the period
            {
                finished = 0;
                j = timeStamp;
                timeStamp += list[i].period;// next stopping point 
            }
            if (timing[j] == Null)// see if the schedule has been filled
            {
                finished++;
                timing[j] = list[i].id;
            }
        }
    }
}
void update(Task *list, int numTasks, int time)
{
    for (int i = 0; i < numTasks; i++)
    {
        if (time >= list[i].currentPeriod)
            list[i].released = true
        if (time >= list[i].currentPeriod + list[i].period)
        {
            cout << list[i].id << " missed its Deadline!" << endl;
            //list[i].currentPeriod += list[i].period; // if we decide to skip missed deadlines
        }
    }
    sort(list, numTasks);
}

void EDF(Task *list, std::string timing2, int numTasks, int totDuration)
{
    int consecutive = 0;
    for (int time = 0; time < totDuration; time++)// We go through each point 
    {
        timing2[time] = list[0].id;// If sort works, this should always be top priority
        consecutive++;
        if (consecutive > list[0].executionTime)// runs with expectation of FIFO then updates accordingly
        {
            list[0].released = false;
            list[0].currentPeriod += list[0].period;
            update(list, numTasks, time);
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
    Task tasks[20];
    while(getline(inputFile, line)){
        if (whileCount == 0){
            numTasks= stoi(line);
        }
        else if(whileCount == 1){
            totDuration = stoi(line);
        }
        else{
            Task newTask;
            int place1 = line.find(',',0);
            newTask.id = line.substr(0,place1);
            int place2 = line.find(',',place1+1);
            int len = place2 - 1 - place1;
            newTask.executionTime = stoi(line.substr(place1+1,len));
            newTask.period = stoi(line.substr(place2+1));
            tasks[whileCount-2] = newTask;
        }
        whileCount += 1;
    }

    Task lists* = new Task[numTasks]();// integrate above
    std::string timing* = new std::string[totDuration]();// also integrate above
    std::string timing2* = new std::string[totDuration]();

    sort(lists, numTasks);
    RMA(lists, timing, numTasks, totDuration);
    EDF(lists, timing2, numTasks, totDuration);
    // for(int j=0; j < 20; j++){
    //     cout << tasks[j].id << " - " << tasks[j].executionTime << " - " << tasks[j].period << " - " << tasks[j].isRunning << endl;
    // }

    inputFile.close();
    outputFile.close();
    return 0;
}
