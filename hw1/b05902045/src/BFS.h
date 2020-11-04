#include "action.cpp"
#include "cstdio"
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
bool checkGoal(STATE st) {
    if ((st.board.ball ^ st.board.box) == 0LL) {
        return true;
    } else {
        return false;
    }
}

void outputAnswer(STATE& state) {
    printf("ans!!! %d\n", state.step);
    for (int i = 0; i < state.step; i++) {
        printf("%c", state.ans[i]);
    }
    printf("\n");
}
int Count(LL num) {
    int ans = 0;
    for(int i = 0; i < 64; i++) {
        if ((num>>i) & 1) {
           
           fprintf(stderr, "x: %d, y: %d ", i/M, i%M); 
           ans++; 
        }
    }
    return ans;
}
int checkCorrect(STATE init, STATE now) {
    STATE tmp = init; 
    for(int i = 0; i < now.step; i++) {
        fprintf(stderr,"step %d \n", i);
        switch (now.ans[i]){
            case '>':
                tmp = pureGo(0, tmp);
                break;
            case '<':
                tmp = pureGo(1, tmp);
                break;
            case '^':
                tmp = pureGo(2, tmp);
                break;
            case 'v':
                tmp = pureGo(3, tmp);
                break;
        }
    }
    assert(checkGoal(tmp));
    assert(now.board == tmp.board);
    return 0;
}
void BFS (GAMESTATE& GameState) {
    QUEUE q;
    HASHMAP map;

    STATE init = GameState.state;
    //printf("player %d %d\n", init.board.player>>4, init.board.player&0xF);
    //printf("ball box %lld %lld\n", init.board.ball, init.board.box);
    q.push(init);
    map[init.board] = true;
    //init.board.showBoard();
    int count = 0;
    int stateCount = 0;
    while (!q.empty()) {
        //STATE now = q.front();
        STATE now = q.top();
        q.pop();
        count++;
        if(now.step > 99){
            break;
        }
        if (!checkGoal(now)) {
            stateCount++;
            for (int i = 0; i < 4; i++) {
                STATE next = now;
                if (go(i, next) == 1) { // the next step is legal
                    if (map.find(next.board) == map.end()) { // check the hashtable
                        map[next.board] = true;
                        next.ans[next.step++] = moveChar[i];
                        q.push(next);
                    }
                }
            }
        } else {
            fprintf(stderr, "step %d\n", now.step);
            fprintf(stderr, "state %d\n", stateCount);
            printf("penalty %d\n", now.penalty);
            outputAnswer(now);
            //checkCorrect(init, now);
            return ;
        }
    }
    fprintf(stderr, "QQ no answer! %d\n", count);
    // output the answer
}
