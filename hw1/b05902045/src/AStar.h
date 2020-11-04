#include "action.cpp"
#include "cstdio"
#include <cmath>
#include <vector>
#include "Hungarian.h"

#define LOW 0XFFFFFFFFFFFFF 

/*
bool checkGoal(STATE st) {
    if (((st.board>>60) ^ ((st.board>>8) & LOW)) == 0) {
        return true;
    } else {
        return false;
    }
}
*/
typedef struct Cor{
    int x;
    int y;
    Cor(int x1, int y1) {
        x = x1;
        y = y1;
    }
    Cor(const Cor& c) {
        x = c.x;
        y = c.y;
    }
} COR;

/*
int Min(int a1, int a2) {
    return a1<a2?a1:a2;
}*/

bool checkGoal(STATE st) {
    if ((st.board.ball ^ st.board.box) == 0) {
        return true;
    } else {
        return false;
    }
}

void outputAnswer(STATE& state) {
    fprintf(stderr, "Ans!!! \n");
    for (int i = 0; i < state.step; i++) {
        printf("%c", state.ans[i]);
    }
    printf("\n");
}

int MinManhattan(COR c1, COR c2) {
    if (c1.x-c2.x == 0 || c1.y-c2.y == 0) {
        return 1; 
    } else {
        return Min(abs(c1.x-c2.x), abs(c1.y-c2.y))+1; 
        //return abs(c1.x-c2.x) +abs(c1.y-c2.y); 
    }
    //return Min(abs(c1.x-c2.x),abs(c1.y-c2.y)); 
}

int Manhattan(COR c1, COR c2) {
    return Min(abs(c1.x-c2.x),abs(c1.y-c2.y))+1; 
}

int L2Norm(std::vector<COR>& balls, std::vector<COR>& boxs) {
    int dis = 0;
    for(auto ball: balls) {
       for(auto box: boxs) {
           dis += MinManhattan(ball, box); 
       }
    }
    return dis;
}


inline int COR2int(COR c) {
    return c.x * M + c.y;
}

inline int HeaurPlayer(BOARD& board, COR& player, std::vector<COR>& balls, std::vector<COR>& boxs) {
    int dis = 0;
    /*
    for(auto ball: balls) {
        if (!(board.box>>(COR2int(ball)) & 1)) {
           dis = Min(Manhattan(player, ball), dis); 
        }
    }
    */
    for(auto box: boxs) {
        if (!(board.ball>>(COR2int(box)) & 1)) {
           dis = Min(Manhattan(player, box), dis); 
        }
    }
    return dis;
}

int HeaurManhattan(BOARD& board, std::vector<COR>& balls, std::vector<COR>& boxs) {
    int cost = 0;
    for(auto ball: balls) {
        int tmp = 100;
        for(auto box: boxs) {
            if(ball.x == box.x && ball.y == box.y) {
                tmp = 0;
            }
            else if (!(board.ball>>(COR2int(box)) & 1)) {
               tmp = Min(MinManhattan(ball, box), tmp); 
            }
        }
        cost += tmp;
    }
    return cost;
}

int findUnmatched(BOARD& board, std::vector<COR>& balls) {
    int num = 0;
    for (auto ball: balls) {
        if (!(board.box>>(COR2int(ball)) & 1)) {
           num++; 
        }
    }
    return num;
}
int HeaurMatch(BOARD& board, std::vector<COR>& balls, std::vector<COR>& boxs, COR player) {

    int num = 0;
    std::vector<COR> newball;
    std::vector<COR> newbox;
    for (auto ball: balls) {
        if (!(board.box>>(COR2int(ball)) & 1)) {
            newball.push_back(ball);
            num++; 
        }
    }
    for (auto box: boxs) {
        if (!(board.ball>>(COR2int(box)) & 1)) {
            newbox.push_back(box);
        }
    }

    int cost[num][num];
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            cost[i][j] = Manhattan(newball[i], newbox[j]); 
            //cost[i][j] += Min(Manhattan(player, newbox[j]), Manhattan(newball[i], player)); 
        }
    }

    HungarianAlgorithm H(num); 
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            H.costMatrix[i][j] = cost[i][j];
        }
    }
    H.execute(); 
    int dis = 0;
    for (int i = 0; i < num; i++) {
        //printf("%d\n", H.result[i]);
        dis += cost[i][H.result[i]];
    }
    return dis;

}

int getDist(BOARD board) {
    std::vector<COR> balls;
    std::vector<COR> boxs;
    for (int i = 0; i < boardSize; i++) {
        if (board.ball>>i & 1) {
            COR tem(i/M, i%M);
            balls.push_back(tem);
        }
        if (board.box>>i & 1) {
            COR tem(i/M, i%M);
            boxs.push_back(tem);
        }
    }
    COR player((board.player & 0xF), (board.player>>4));
    /*
    int tmp = 100;
    for(auto ball: balls) {
       tmp = min(tmp, Manhattan(player, ball));
       for(auto box: boxs) {
           //tmp2 = minManhattan(ball, box);
           dis += minManhattan(ball, box); 
       }
       dis += tmp;
    }
    for(auto box: boxs) {
       tmp = min(tmp, Manhattan(player, box));
    }
    */
    int dis = 0;
    //dis += L2Norm(balls, boxs); 
    dis += HeaurMatch(board, balls, boxs, player);
    //dis += HeaurManhattan(board, balls, boxs); 
    //dis += HeaurPlayer(board, player, balls, boxs); 
    //dis += tmp; 

    return dis;
}

void Restriction() {
    RES |=  1LL;
    RES |=  1LL<<(M-1);
    RES |=  1LL<<(M*(N-1));
    RES |=  1LL<<(M*N-1);
}

void AStar (GAMESTATE& GameState) {
    QUEUE q;
    HASHMAP map;
    STATE init = GameState.state;
    q.push(init);
    map[init.board] = true;

    int count = 0;
    int stateCount = 0;
    Restriction();
    while (!q.empty()) {
        //STATE now = q.front();
        STATE now = q.top();
        q.pop();
        count++;
        if (!checkGoal(now)) {
            stateCount++;
            for (int i = 0; i < 4; i++) {
                STATE next = now;
                if (go(i, next) == 1 && map.find(next.board) == map.end()) { // it is a legal move and not been yet
                    // calculate the heuritic function 
                    /*
                    if((next.step % 2) == 0) {
                    } else {
                        next.heuristic = now.heuristic;
                    }*/
                    next.heuristic = getDist(next.board);
                    map[next.board] = true;
                    next.ans[next.step++] = moveChar[i];
                    q.push(next);
                }
            }
        } else {
            fprintf(stderr, "step %d\n", now.step);
            fprintf(stderr, "state %d\n", stateCount);
            printf("%d\n", now.penalty);
            outputAnswer(now);
            return ;
        }
    }
    fprintf(stderr, "QQ no answer! %d\n", count);
    // output the answer
}
