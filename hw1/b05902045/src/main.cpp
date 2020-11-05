#include "utils.cpp"
#include "solver.cpp"


int main() {
    int count = 0;
    while (readFile() != -1) {
        //fprintf(stderr, "Test case %d!\n", count++);
        solvePuzzle();
        //fprintf(stderr, "===========================\n");
    }
    return 0;
}
