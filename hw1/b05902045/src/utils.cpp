// This file deine the util functions

#include <cstdio>
#include "global.h"

int readFile() {
    if(scanf("%d %d", &N, &M) == EOF){
        return -1;
    }
    boardSize = N * M;
    for(int i = 0; i < N; i++){
        scanf("%s", globalBoard[i]);
        //printf("%s\n", globalBoard[i]);
    }
    return 0;
}
