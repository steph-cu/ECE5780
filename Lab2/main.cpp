#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
//#include <io.h>
#include <string>
#include <math.h>

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

    string line;
    while(getline(inputFile, line)){
        outputFile << line << endl;
    }

    inputFile.close();
    outputFile.close();
    return 0;
}
