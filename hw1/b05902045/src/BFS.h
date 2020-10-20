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
    if ((st.board.ball ^ st.board.box) == 0) {
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

void BFS (GAMESTATE& GameState) {
    QUEUE q;
    HASHMAP map;

    STATE init = GameState.state;
    printf("player %d %d\n", init.board.player>>4, init.board.player&0xF);
    printf("ball box %lld %lld\n", init.board.ball, init.board.box);
    q.push(init);
    map[init.board] = true;

    int count = 0;
    while (!q.empty()) {
        STATE now = q.front();
        q.pop();
        count++;
        if(now.step > 25){
            break;
        }
        if (!checkGoal(now)) {
            for (int i = 0; i < 4; i++) {
                go(i, q, map, now);
            }
        } else {
            printf("step %d\n", now.step);
            outputAnswer(now);
            return ;
        }
    }
    printf("QQ no answer! %d\n", count);
    // output the answer
}
