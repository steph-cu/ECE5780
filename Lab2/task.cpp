//#include "task.h"
#include <string>
#include <stdio.h>
#include <io.h>


class Task{
    public:
        std::string id;
        int executionTime;
        int period; 
        int currentDeadline;
        int currentExecutionCounter;
        int numExecutions;
        int numPreemptions;
        int numMissedDeadlines;
        bool Aperiotic;
        bool released;
        Task(){
            id = '0';
            executionTime = 0;
            period = 0; 
            currentDeadline = 0; //Allows each task to know when it's next deadline is going to be which might be different from period.
            currentExecutionCounter = 0; //Allows each task to know how much longer it needs to execute to finish because it might be preempted.
            numExecutions = 0; // keep track of how many times its executed
            numPreemptions = 0; // keep track of how many times it was preempted
            numMissedDeadlines = 0;
            Aperiotic = false;
            released = true;
        } 
};