#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
//#include <io.h>
#include <string>
#include <math.h>
#include "task.cpp"

using namespace std;

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

    // for(int j=0; j < 20; j++){
    //     cout << tasks[j].id << " - " << tasks[j].executionTime << " - " << tasks[j].period << " - " << tasks[j].isRunning << endl;
    // }

    inputFile.close();
    outputFile.close();
    return 0;
}
