#include "stdio.h"
#include "../include/ns_simulation.h"

int main(){
    NsInput input;
    input.input_file = "src_file.wav";
    input.output_file = "out_file.wav";
    input.mode = 2;
    implement_ns(&input);
    return 0;
}