CXX = g++ -std=c++11 -Wall -O2 
CLEAN = rm -rf

all:
	$(CXX) *.cpp -c
	$(CXX) main.o MyAI.o -o NegaMax
	$(CXX) test.o MyAI.o -o sample

clean:
	$(CLEAN) *.o NegaMax
