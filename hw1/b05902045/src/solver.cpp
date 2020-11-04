// This define the solver which choose the search strategy 
// to solve the puzzle
//
//
//#include "BFS.h"
#include "AStar.h"
#include "string.h"
/*
void initBoard(STATE* state) { // init the bit map version of board 
    int pos = 0; 
    for (int i = 0; i < state->n; i++) {
        for (int j = 0; j < state->m; j++) {
            switch (state->boardState[i][j]){
                case BALL:
                    state->board.ball |= 1<<pos;
                case BOX:
                    state->board.box |= 1<<pos; 
                case PLAYER:
                    state->board.player |= i;   
                    state->board.player |= j<<4;   
            }
            pos++;
        }
    }
}
STATE* init() {
    STATE* state = new STATE;
    state->n = N;
    state->m = M;
    state->step = 0;
    for (int i = 0; i < 16; i++) {
        strcpy(state->boardState[i], globalBoard[i]);
    }
    initBoard(state);

    return state;
}
*/

void solvePuzzle() {
    //STATE* state =  init();  // init the state information of the board
    // call the searcher
    GAMESTATE init(globalBoard, N, M);
    AStar(init);
    //BFS(init);
}
