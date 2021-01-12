CXX = g++ -std=c++11 -Wall -O2 
CLEAN = rm -rf

all:
	$(CXX) src/*.cpp -c
	$(CXX) main.o MyAI.o -o NegaScout

clean:
	$(CLEAN) *.o NegaScout
