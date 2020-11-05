// This file defines the action when push a box or a ball
//
#include "types.h"
#include <assert.h>
const int dirX[] = { 0, 0, -1, 1};
const int dirY[] = { 1, -1, 0, 0};

const char moveChar[] = { '>', '<', '^', 'v'};
const int reverseDir[] = { 1, 0, 3, 2};
void currentPos(BOARD& b, int& x, int& y){
    x = b.player & 0XF; 
    y = b.player >> 4; 
}

bool outofBound(int x, int y) {
    return (x < 0 || y < 0 || x >= N || y >= M);
}

inline bool isExist(LL map, int pos) {
    return (map>>pos) & 1;
}

bool gotBlock(int pos1, int pos2, BOARD now) {
    if ((isExist(now.ball, pos1) && isExist(now.ball, pos2)) || 
       (isExist(now.box, pos1) && isExist(now.box, pos2))) {
        return true;
    } else {
        return false;
    }
    //return ((globalBoard[x1][y1] + globalBoard[x2][y2]) == (BOX+BALL))?false:true;
}

inline bool Restriction(int pos) {
    if (RES>> pos & 1) {
        return true;
    } else {
        return false;
    }
}

void moveBall(STATE &now, int sx, int sy, int dir) {
    int tx = sx; 
    int ty = sy;
    while (true) {
        tx += dirX[dir];
        ty += dirY[dir];
        int pos = tx*M + ty; 
        if (outofBound(tx, ty) || globalBoard[tx][ty] == WALL || isExist(now.board.ball, pos)) {
            tx -= dirX[dir];
            ty -= dirY[dir];
            break;
        } else if (isExist(now.board.box, pos)){
            break;
        }
    }
    //assert((tx != sx) || (ty != sy) );
    int newPos = tx*M+ty;
    now.board.ball ^= 1LL<<(sx*M+sy); // remove old pos
    now.board.ball |= 1LL<<(newPos); // add new pos
    /*
    if(!isExist(now.board.box, newPos) && isExist(RES, newPos)) {
       now.ballRes = true; 
    }*/
}
int deadlockDetection(int x, int y) {
    return 0;
}
void moveBox(STATE &now, int start, int target) {
    if ((now.board.ball>>start) & 1) {
        now.board.ball |= 1LL<<target;
        now.board.ball ^= 1LL<<start; 
    }
    now.board.box |= 1LL<<target;
    now.board.box ^= 1LL<< start; 
    /*
    if(!isExist(now.board.ball, target) && isExist(RES, target)) {
       now.boxRes = true; 
    }*/
}

int boxNum(LL box){
    int count = 0;
    for(int i = 0; i< boardSize; i++){
        if( (box>>i) & 1){
            count++; 
        }
    }
    return count;
}

STATE pureGo(int dir, STATE state) {
    int x, y;    
    currentPos(state.board, x, y);
    int nx = x + dirX[dir];
    int ny = y + dirY[dir];

    int nPos = nx * M + ny;
    bool push = false; 
    bool slide = false;
    if (isExist(state.board.box, nPos)) { // it is occupied by ball or box_ball
        push = true;
        //fprintf(stderr, "dir %d box\n", dir);
    } else if (isExist(state.board.ball, nPos)) { // do not have a box but have ball
        push = true;
        slide = true;
        //fprintf(stderr, "dir %d ball\n", dir);
    }
    if (push) {
        int nnx = nx + dirX[dir];
        int nny = ny + dirY[dir];
        int nnPos = nnx * M + nny;
        if (slide) {
            moveBall(state, nx, ny, dir);
        } else {
            moveBox(state, nPos, nnPos);
        }
    } 
    state.board.player = 0;
    state.board.player |= nx;
    state.board.player |= ny<<4;
    //state.board.showBoard();
    return state;
}

//int go(int dir, QUEUE& q, HASHMAP& map, STATE& state) { // try to push on the direction dir 
int go(int dir, STATE& state) { // try to push on the direction dir 
    int x, y;    
    currentPos(state.board, x, y);
    int nx = x + dirX[dir];
    int ny = y + dirY[dir];
    //BOARD now = state.board;
    //assert(boxNum(now.box) == 2);
    //assert(boxNum(now.ball) == 2);
    if ((!outofBound(nx, ny) && globalBoard[nx][ny] != WALL)) {
        int nPos = nx * M + ny;
        bool push = false; 
        bool slide = false;
        if (isExist(state.board.box, nPos)) { // it is occupied by ball or box_ball
            push = true;
        } else if (isExist(state.board.ball, nPos)) { // do not have a box but have ball
            push = true;
            slide = true;
        }
        if (push) {
            int nnx = nx + dirX[dir];
            int nny = ny + dirY[dir];
            int nnPos = nnx * M + nny;
            if ( outofBound(nnx, nny) || (globalBoard[nnx][nny] == WALL)
                || gotBlock(nPos, nnPos, state.board) ) { // the case has ball ball or box box or * wall
                return 0;
            }
            if (slide) {
                moveBall(state, nx, ny, dir);
                state.penalty++;
            } else {
                if (isExist(state.board.ball, nPos)) { // it is the ball_box
                    state.penalty++;
                }
                state.penalty++;
                moveBox(state, nPos, nnPos);
            }
        } else{
            state.penalty++;
            state.board.player = 0;
            state.board.player |= nx;
            state.board.player |= ny<<4;
            return 2;
        }
        /*
        if (state.ballRes && state.boxRes) { // there is deadlock
            return 0;
        }*/
        state.board.player = 0;
        state.board.player |= nx;
        state.board.player |= ny<<4;
        // check the HashMap
        return 1;
        /*
        if (map.find(state.board) == map.end()) {
            map[state.board] = true;
            state.backward = reverseDir[dir]; 
            state.ans[state.step++] = moveChar[dir]; 
            //q.push(state);
            return 0;
        }
        */
    }
    return 0;
}

    
