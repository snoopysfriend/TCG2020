CXX = g++ -std=c++11 -Wall -O3


ifeq ($(OS),Windows_NT)
    CLEAN = del
else
    CLEAN = rm -rf
endif

all:
	$(CXX) src/*.cpp -c
	$(CXX) main.o getch.o sokoboru.o -o sokoboru
	$(CXX) verifier.o sokoboru.o -o verifier
	$(CLEAN) *.o
clean:
	$(CLEAN) sokoboru verifier
