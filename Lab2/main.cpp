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

    fprintf(stdout, "made it here\n");

    FILE * inputFile = fopen(argv[1], "r");
    if(inputFile == NULL){
        fprintf(stdout, "The program couldn't open the input file.");
        return -1;
    }

    FILE * outputFile = fopen(argv[2],"w");
    if(outputFile = NULL){
        fprintf(stdout, "The program couldn't open and write to the output file.");
        return -1;
    }

    

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
