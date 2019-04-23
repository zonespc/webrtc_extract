.PHONY: src main
src:
	mkdir build && cd build && cmake ../src && make 
	
main: 
	cd test && mkdir build && cd build && cmake ../ && make 


clean:
	rm -rf build
	rm -rf src/CMakeFiles
