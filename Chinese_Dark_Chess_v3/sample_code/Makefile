CXX = g++ -std=c++11 -Wall -O2
CLEAN = rm -rf

all:
	$(CXX) src/*.cpp -c
	$(CXX) main.o MyAI.o board.o hash.o -o NegaScout
	$(CXX) test.o MyAI.o board.o hash.o -o test

clean:
	$(CLEAN) *.o NegaScout
