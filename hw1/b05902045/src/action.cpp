// This file defines the action when push a box or a ball
//
#include "types.h"
#include <assert.h>
int dirX[] = { 0, 0, -1, 1};
int dirY[] = { 1, -1, 0, 0};

char moveChar[] = { '>', '<', '^', 'v'};

void currentPos(BOARD& b, int& x, int& y){
    x = b.player & 0XF; 
    y = b.player >> 4; 
}

bool outofBound(int x, int y) {
    return (x < 0 || y < 0 || x >= N || y >= M);
}

bool isExist(LL map, int pos) {
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

void moveBall(BOARD &now, int sx, int sy, int dir) {
    int tx = sx; 
    int ty = sy;
    while (true) {
        tx += dirX[dir];
        ty += dirY[dir];
        int pos = tx*M + ty; 
        if (outofBound(tx, ty) || globalBoard[tx][ty] == WALL || isExist(now.ball, pos)) {
            tx -= dirX[dir];
            ty -= dirY[dir];
            break;
        } else if (isExist(now.box, pos)){
            break;
        }
    }
    now.ball ^= 1<<(sx*M+sy);
    now.ball |= 1<<(tx*M+ty);
}

void moveBox(BOARD &now, int start, int target) {
    now.box |= 1<<target;
    now.box ^= 1<< start; 
    if ((now.ball>>start) & 1) {
        now.ball |= 1<<target;
        now.ball ^= 1<< start; 
    }
}
int boxNum(LL box){
    int count = 0;
    for(int i = 0; i< 64; i++){
        if( (box>>i) & 1){
            count++; 
        }
    }
    return count;
}
void go(int dir, QUEUE& q, HASHMAP& map, STATE state) { // try to push on the direction dir 
    int x, y;    
    currentPos(state.board, x, y);
    int nx = x + dirX[dir];
    int ny = y + dirY[dir];
    BOARD now = state.board;
    //assert(boxNum(now.box) == 2);
    //assert(boxNum(now.ball) == 2);
    if ((!outofBound(nx, ny) && globalBoard[nx][ny] != WALL)) {
        int nPos = nx * M + ny;
        bool push = false; 
        bool slide = false;
        if (isExist(now.box, nPos)) { // it is occupied by ball or box_ball
            push = true;
        } else if (isExist(now.ball, nPos)) { // do not have a box but have ball
            push = true;
            slide = true;
        }
        if (push) {
            int nnx = nx + dirX[dir];
            int nny = ny + dirY[dir];
            int nnPos = nnx * M + nny;
            if ( outofBound(nnx, nny) || (globalBoard[nnx][nny] == WALL)
                || gotBlock(nPos, nnPos, now) ) { // the case has ball ball or box box or * wall
                return ;
            }
            if (slide) {
                moveBall(state.board, nx, ny, dir);
            } else {
                moveBox(state.board, nPos, nnPos);
            }
        }
        state.board.player = 0;
        state.board.player |= nx;
        state.board.player |= ny<<4;
        // check the HashMap
        if (map.find(state.board) == map.end()) {
            map[state.board] = true;
            state.ans[state.step++] = moveChar[dir]; 
            q.push(state);
        }
    }
    
    return ;
}

    
