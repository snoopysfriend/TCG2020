CXX = g++ -std=c++11 -Wall
F_LINUX = -D LINUX
CLEAN = rm -rf

all:
	$(CXX) $(F_LINUX) src/*.cpp -c
	$(CXX) $(F_LINUX) main.o getch.o -o game
	$(CXX) baseline.o -o random
	$(CXX) -D CONSERVATIVE src/baseline.cpp -o conservative
	$(CXX) -D GREEDY src/baseline.cpp -o greedy
	$(CLEAN) -f *.o
clean:
	$(CLEAN) game greedy conservative random
	$(CLEAN) .log.*
