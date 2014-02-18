all: callstack

callstack: main.cpp callstack.cpp callstack.h CMutex.cpp
	g++ -Wall -g -rdynamic main.cpp callstack.cpp CMutex.cpp -I. -o $@ -pthread -lrt 

clean:
	rm -f callstack