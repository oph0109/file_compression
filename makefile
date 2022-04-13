build:
	rm -f program.exe
	g++ -g -std=c++11 -Wall main.cpp hashmap.cpp -I '.guides/secure/' -o program.exe
	
run:
	./program.exe

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./program.exe

test:
	rm -f program.exe
	g++ -g -std=c++11 -Wall test.cpp hashmap.cpp -I '.guides/secure/' -o program.exe
	./program.exe
	
