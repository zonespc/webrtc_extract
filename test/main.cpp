#include "stdio.h"
#include "../include/ns_simulation.h"

int main(int argc,char *argv[]){
    if(argc < 3){
        printf("Input Error!\n");
        return 0;
    }
    char* infile = argv[1];
    char* outfile = argv[2];
    NsInput input;
    input.input_file = infile;
    input.output_file = outfile;
    input.mode = 2;
    implement_ns(&input);
    return 0;
}