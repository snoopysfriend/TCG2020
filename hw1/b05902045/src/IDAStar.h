#include "AStar.h"
#include <stack>

int minThreshold = 1000;

bool DFID(STATE start, int threshold) {
    std::stack<STATE> S;
    S.push(start);
    HASHMAP map;
    map[start.board] = true;
    while (!S.empty()) {
        STATE next = S.top();
        int tmp;
        while (S.top().dir < 4) {
            tmp = go(S.top().dir, next);
            S.top().dir++;
            if (S.top().dir == 4) {
                /*
                if(S.empty()){
                    fprintf(stderr, "QQQ%d\n", S.top().step);
                }*/
                S.pop();
                break;
            }
            if (tmp == 1) {
                break;
            }
        }
        if (tmp == 1) {
            if (map.find(next.board) != map.end()) {
                continue;
            }
            next.heuristic = getDist(next.board);
            if ((next.penalty+next.heuristic) > threshold) {
                minThreshold = Min(minThreshold, next.penalty + next.heuristic);
                continue;
            } 
            if (checkGoal(next)) {
                fprintf(stderr,"ans\n");
                outputAnswer(next);
            } 
            map[next.board] = true;
            next.ans[next.step++] = moveChar[0];
            S.push(next);
        } 
  
    }
    return false;

}

void IDA(GAMESTATE& GameState) {
    int threshold = 1;
    STATE init = GameState.state;
    int i = 0;
    HASHMAP map;
    while(true) {
        if(DFID(init, threshold)){
            return ;
        }
        threshold = minThreshold;
        minThreshold = 1000;
        printf("%d\n", threshold);
        i++;
    }
}
