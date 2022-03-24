//#include "task.h"
#include <string>
#include <stdio.h>
#include <io.h>


class Task{
    public:
        std::string id;
        int executionTime;
        int period; 
        int currentPeriod;
        bool Aperiod;
        bool released;
        Task(){
            id = '0';
            executionTime = 0;
            period = 0; 
            currentPeriod = 0;
            Aperiod = false;
            released = false;
        } 
};