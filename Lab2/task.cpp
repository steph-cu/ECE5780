//#include "task.h"
#include <string>
#include <stdio.h>
#include <io.h>


class Task{
    public:
        std::string id;
        int executionTime;
        int period; 
        bool isRunning;
        Task(){
            id = '0';
            executionTime = 0;
            period = 0; 
            isRunning = false;
        } 
};