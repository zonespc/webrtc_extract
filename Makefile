.PHONY: build test clc clean

all: build test clc

build:
	mkdir build_src && cd build_src && cmake ../src && make 

test:
	mkdir build_main && cd build_main && cmake ../test && make 

clc:
	rm -rf build_src
	rm -rf build_main
