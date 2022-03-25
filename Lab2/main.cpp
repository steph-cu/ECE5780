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

void sort(vector<Task> list, int end)// will sort the array by priority (initial period)
{
    int highest = 0;
    Task temp;
    int lowest;
    for (int i = 0; i < end ; i++)
    {
        highest = i;
        for (int j = i; j < end ; j++)
        {
            if (list[j].released)// this will help with the EDF update
            {
                list[j].currentPeriod > list[lowest].currentPeriod ? highest = j : highest = highest;
            }
        }
        if (highest != i)
        {
            temp = list[highest];
            list[highest] = list[i];
            list[i] = temp;
        }
    }
}

void RMA(vector<Task> list, vector<string> timing, int numTasks, int totDuration)
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
            if (timing[j] == "")// see if the schedule has been filled
            {
                finished++;
                timing[j] = list[i].id;
            }
        }
    }
}
void update(vector<Task> list, int numTasks, int time)
{
    for (int i = 0; i < numTasks; i++)
    {
        if (time >= list[i].currentPeriod)
        {
            list[i].released = true;
        }
        if (time >= list[i].currentPeriod + list[i].period)
        {
            cout << list[i].id << " missed its Deadline!" << endl;
            //list[i].currentPeriod += list[i].period; // if we decide to skip missed deadlines
        }
    }
    sort(list, numTasks);
}

void EDF(vector<Task> list, vector<string> timing2, int numTasks, int totDuration)
{
    int consecutive = 0;
    for (int time = 0; time < totDuration; time++)// We go through each point 
    {
        char* currId = &list[0].id[0]; //functionally a cast from string to char*. Gets the ID from the 0 position in list
        timing2[time] = *currId;// If sort works, this should always be top priority
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
    vector<Task> tasks; // implementing the tasks as a dynamic vector.

    while(getline(inputFile, line)){
        if (whileCount == 0){
            numTasks= stoi(line);
        }
        else if(whileCount == 1){
            totDuration = stoi(line);
        }
        else{
            Task newTask; // parse through the line to get the correct id executionTime and period
            int place1 = line.find(',',0);
            newTask.id = line.substr(0,place1);
            int place2 = line.find(',',place1+1);
            int len = place2 - 1 - place1;
            newTask.executionTime = stoi(line.substr(place1+1,len));
            newTask.period = stoi(line.substr(place2+1));
            tasks.push_back(newTask); // this adds the new task to the end of  the vector.
        }
        whileCount += 1;
    }

   for(int j=0; j < tasks.size(); j++){
        cout << tasks[j].id << " - " << tasks[j].executionTime << " - " << tasks[j].period << " - " << tasks[j].released << endl;
    }
    
    vector <string> timing;
    timing.resize(totDuration);
    vector <string> timing2;
    timing2.resize(totDuration);

    cout << "made it here in the execution" << endl;

    sort(tasks, numTasks);

    cout << "made it after the sort" << endl;
    for(int j=0; j < tasks.size(); j++){
        cout << tasks[j].id << " - " << tasks[j].executionTime << " - " << tasks[j].period << " - " << tasks[j].released << endl;
    }

    RMA(tasks, timing, numTasks, totDuration);
    EDF(tasks, timing2, numTasks, totDuration);
 

    inputFile.close();
    outputFile.close();
    return 0;
}
