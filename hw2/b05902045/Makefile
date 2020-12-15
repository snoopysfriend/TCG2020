CXX = g++ -std=c++11 -Wall -O3
CLEAN = rm -rf

all: 
	$(CXX) src/*.cpp -c
	$(CXX) MCTS.o -o MCTS

clean:
	$(CLEAN) MCTS
	$(CLEAN) .log.* *.o
