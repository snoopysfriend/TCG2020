#include "utils.cpp"
#include "solver.cpp"


int main() {
    int count = 0;
    while (readFile() != -1) {
        printf("Test case %d!\n", count++);
        solvePuzzle();
        printf("===========================\n");
    }
    return 0;
}
